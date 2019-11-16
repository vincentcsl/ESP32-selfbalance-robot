// 代码重构，常量全部大写


#include <WiFi.h>
#include <Wire.h>
#include <Kalman.h>
#include <Encoder.h>
#include <BluetoothSerial.h>
#include <ArduinoJson.h>
/*******************调试宏定义********************/
#define WifiPrintValue
#define ADJUST_PID
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
double  P_speed = 0.0625, I_speed = 0;
double  speed_setpoint = 0, speed_output, speed, speed_integral;
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
double kalAngleY;
int16_t tempRaw;
uint32_t timer;
uint8_t i2cData[14];
/***************编码器变量定义**********************/
long encoder_L, encoder_R;
    // 构造编码器对象
Encoder knobRight(Encoder_A_1,Encoder_A_2);
Encoder knobLeft(Encoder_B_1,Encoder_B_2);
/********************WIFI************************/
const char* ssid = "Flutter_app";
const char* password =  "12345678";
WiFiServer server; //声明服务器对象
IPAddress local_IP(192, 168, 43, 101);
IPAddress gateway(192, 168, 43, 1);
IPAddress subnet(255, 255, 255, 0);
WiFiClient client;
/********************蓝牙声明************************/
BluetoothSerial SerialBT;
/******************Json************************/
DynamicJsonDocument doc(JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(5) + 20);
/***************主要功能函数声明**********************/
void MPU6050_init(void);
void blueInit(void);
void ioInit(void);
void wifiInit(void);
void Motor(char LR,int S);
void IMU_fillter(void);     // MPU6050数据获取和卡尔曼滤波函数
void angle_PID_compute(void);
void speed_PID_compute(void);
void BT_uart(void);
/***************Setup函数**********************/
void setup() {
    MPU6050_init();
    blueInit();
    ioInit();
#ifdef WifiPrintValue
    wifiInit();
#endif

    FALLDOWN = true;        // 默认不启动，等待蓝牙连接唤醒
}

void loop(){
#ifdef Motor_enable
    isFalldown();
#endif
    IMU_fillter();
    BT_uart();
    speed_PID_compute();
    angle_PID_compute();
#ifdef WifiPrintValue
    wifiPrintValue();
#endif
}
