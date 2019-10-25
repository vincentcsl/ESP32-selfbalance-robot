

/*     包含库      */
#include <Wire.h>
// 包含卡尔曼滤波库
#include <Kalman.h>
// 包含编码器库
#include <Encoder.h>
// 包含蓝牙串口库
#include <BluetoothSerial.h>
// 包含Json库
#include <ArduinoJson.h>
/*******************调试宏定义********************/
// #define Print_Value
// #define ADJUST_PID
#define Motor_enable
/*******************常量定义********************/
#define L_Motor 0
#define R_Motor 1
// PWM相关常量
#define Freq 5000
#define A_Channel 0
#define B_Channel 1
#define Resolution 8
// L298N模块相关引脚
#define PWMA 15
#define AIN2 2
#define AIN1 0
#define STBY 4
#define BIN1 16
#define BIN2 17
#define PWMB 5
// 编码器引脚定义
#define Encoder_A_1 27
#define Encoder_A_2 14
#define Encoder_B_1 33
#define Encoder_B_2 13
// 电机最小驱动pwm
#define PWM_MIN_LEFT  5
#define PWM_MIN_RIGHT 6
// 最大速度值
#define SPEED_MAX 300
#define SPEED_MIN -300
#define TURN_MAX 500
#define TURN_MIN -500
// 摔倒角度
#define FALLDOWN_ANGLE 45
// 平衡角度
#define BALANCE_ANGLE -1.5
// 调试时的时间计数
#ifdef Print_Value
    uint32_t tmp_timer;
#endif
/***************标志变量**********************/
bool FALLDOWN;
/***************PID变量和常量定义**********************/
    // 角度环数据
    // 放弃角度环积分,积分项难以调节，且需要限幅
    // {"P":0,"A": [33,-0.004],"S": 0.0625}
    // {"UP":1,"Y":10,"X":0,"A": [33,-0.004],"S": 0.0625}
double  P_angle = 33, D_angle = -0.004;
    // 这里的初始角度在加入速度环后就不重要了，速度环会纠正平衡角度的偏差
double  angle_setpoint = BALANCE_ANGLE, angle_output;
uint32_t angle_PID_timer = 0;
    // 速度环数据
double  P_speed = 0.0625;//, I_speed = 0;
double  speed_setpoint = 0, speed_output;
uint32_t speed_PID_timer = 0;
    // 方向环数据
double  turn_output;
    // 电机PWM值
int output_L, output_R;
/***************MPU6050变量定义**********************/
    // 构造卡尔曼滤波对象
Kalman kalmanY;
double accX, accY, accZ;
double gyroY, gyroZ;
double kalAngleY;;
int16_t tempRaw;
uint32_t timer;
uint8_t i2cData[14];
/***************编码器变量定义**********************/
long encoder_L, encoder_R;
    // 构造编码器对象
Encoder knobRight(Encoder_A_1,Encoder_A_2);
Encoder knobLeft(Encoder_B_1,Encoder_B_2);
/********************蓝牙声明************************/
    // 构造蓝牙串口对象
BluetoothSerial SerialBT;
/******************Json************************/
    // arduinoJSON助理 https://arduinojson.org/v6/assistant/
    // 不会用arduinoJson的同学可以看我博客 http://vincc.me/267/
DynamicJsonDocument doc(JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(5) + 20);
/***************主要功能函数声明**********************/
    // MPU6050初始化函数
void MPU6050_init(void);
    // 电机驱动函数
void Motor(char LR,int S);
    // MPU6050数据获取和卡尔曼滤波函数
void IMU_fillter(void);
    // PID计算
void angle_PID_compute(void);
void speed_PID_compute(void);
    // 蓝牙串口通信
void BT_uart(void);



/***************Setup函数**********************/
void setup() {
    // 设置波特率
    Serial.begin(115200);
    // 蓝牙设备名称
    SerialBT.begin("Balance-Robot");
/********MPU6050相关********/
    MPU6050_init();
/*********电机相关*********/
    // 设置IO口模式
    pinMode(AIN1,OUTPUT);
    pinMode(AIN2,OUTPUT);
    pinMode(BIN1,OUTPUT);
    pinMode(BIN2,OUTPUT);
    pinMode(STBY,OUTPUT);
    // 设置并绑定通道和引脚
    ledcSetup(A_Channel, Freq, Resolution);
    ledcSetup(B_Channel, Freq, Resolution);
    ledcAttachPin(PWMA, A_Channel);
    ledcAttachPin(PWMB, B_Channel);
    // 开启模块驱动功能
    digitalWrite(STBY,HIGH);
/*********其它*********/
    // 默认不启动，等待蓝牙连接唤醒
    FALLDOWN = true;
    // 输出初始化成功信息
    SerialBT.println("Setup is OK!!!");
}



/***************Loop函数********************/
void loop(){
/***********检查是否摔倒*********/
#ifdef Motor_enable
    if ((kalAngleY > FALLDOWN_ANGLE || kalAngleY < - FALLDOWN_ANGLE) || FALLDOWN){
        // 关电机
        Motor(L_Motor, 0);
        Motor(R_Motor, 0);
        output_L = 0;
        output_R = 0;
        angle_setpoint = BALANCE_ANGLE;
        speed_setpoint = 0;
        turn_output = 0;
        // 设置摔倒标志
        FALLDOWN = true;
        // 等待蓝牙指令
        while (FALLDOWN) {
            BT_uart();
        }
    }
#endif
/***********控制计算************/
    // 获取角度数据
    IMU_fillter();
    // 蓝牙串口控制
    BT_uart();
    // 速度和角度环的计算
    speed_PID_compute();
    angle_PID_compute();
/***********调试输出************/
#ifdef Print_Value
    if (micros()-tmp_timer>300000) {
        // 在这里可以随意添加想看的输出值
        // 建议使用串口绘图器观察输出值
        // 有利于排除错误
        Serial.println(kalAngleY);
        tmp_timer=micros();
    }
#endif
}
