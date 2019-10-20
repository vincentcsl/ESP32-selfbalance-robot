/**********************************************************************
说明:这是我做的一个小项目-自平衡机器人Nano的固件程序,机器人的实际演示
可以看这里http://pengzhihui.xyz/2015/12/09/nano/
项目使用方法:把Adafruit_GFX和Adafruit_SSD1306文件夹放入IDE的libraries
文件夹，打开Nano.ino就可以编译成功了，推荐使用最新版本IDE.引脚定义在程
序中用宏标出，需要修改PID参数以适应不同的机械硬件，更多疑问可以联系邮
箱 593245898@qq.com                            pz_cloud     2015.6.25
**********************************************************************/

/*****************************调试选项********************************/
//#define TIMING_DEBUG   //PID周期调试,开启后打印时间信息
//#define PARAM_DEBUG    //PID参数调试,关闭后用宏取代变量值节省动态内存
#define IMU_OUTPUT  //输出6050数据
//#define SONIC_OUTPUT  //输出超声波数据
//#define SPEED_LOOP       //速度环开关
//#define MOTOR_ENABLE     //电机使能
//#define SONIC_ENABLE     //超声波使能
//#define CAMERA_ENABLE  //摄像头使能
/*********************************************************************/
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <EEPROM.h>
#include <Wire.h>
#include "Camera.h"
#include "Bitmap.h"

#define BUTTON_ON !(PIND&(1<<5))             //读取按键
#define BUTTON_OFF (PIND&(1<<5))             //读取按键
#define FACE_LED(x) analogWrite(LED, 255-x)  //LED亮度函数
/*******************注册标志位********************/
#define NONSTART 0x0001    //启动标志
#define TURNING 0x0002     //转向中标志
#define FALLDOWN 0x0004    //摔倒标志
#define ENCL 0x0008        //左编码器变化标志
#define ENCR 0x0010        //右编码器变化标志
#define COMDONE 0x0020     //帧数据结束标志
#define CTRLING 0x0040     //遥控中标志

/*********************引脚定义*********************/
#define LFT 0
#define RHT 1
#define BUZZER 4    //蜂鸣器
#define BUTTON 5    //按钮
#define LED 11      //脸颊LED
//#define SERVO 13    //舵机,小机器人不推荐使用,电流易过载
#define TRIG_PIN 8  //超声波模块触发脚
#define ECHO_PIN 7  //超声波模块接收脚
//实例化OLED
Adafruit_SSD1306 display(-1);
/***************MPU6050变量定义**********************/
double accX, accY, accZ;
double gyroY, gyroZ;
double compAngleY, gyroYangle;
int16_t tempRaw;
uint32_t timer;
uint8_t i2cData[14];

/***************PID变量定义**********************/
#ifdef PARAM_DEBUG  //角度环数据
double  P_angle = 0, I_angle = 0, D_angle = 0;
#else
#define P_angle  0
#define I_angle  0
#define D_angle  0
#endif
double  angle_setpoint = 0, angle_output, angle_integral;
uint32_t angle_PID_timer;

#ifdef PARAM_DEBUG  //速度环数据
double  P_speed = 0, I_speed = 0;
#else
#define P_speed  0
#define I_speed  0
#endif
double  speed_setpoint = 0, speed_output, speed_integral;
uint32_t speed_PID_timer;

#ifdef PARAM_DEBUG  //方向环数据
double  P_turn = 0, I_turn = 0;
#else
#define P_turn  0
#define I_turn  0
#endif
double  turn_integral = 0 , turn_output ;

/***************编码器变量定义**********************/
volatile int count_L, count_R;  //左右编码器计数
volatile char direct_LR;        //记录PWM方向
volatile uint32_t ECL_timer, ECR_timer;

/*********************其他变量**********************/
uint32_t FLAG , CAM_timer , SONIC_timer ;
float distance_cm, joy_x, joy_y;
char comdata[19], data_p; //串口缓存数据
union EEPROM_DATA {       //联合体用于分段储存EEPROM数据
  double data;
  uint8_t dchar[4];
} pre_angle;

/*********************函数声明***********************/
void (* resetFunc) (void) = 0;    //重启函数
void Motor(char LR, int SPEED);   //电机驱动函数
void angle_PID_compute(void);     //角度环计算
void speed_PID_compute(void);     //速度环计算
void IMU_fillter(void);           //IMU角度融合
void get_uart(void);              //读取蓝牙串口指令
void drawRect(int16_t x, int16_t y, int16_t sx, int16_t sy);  //画矩形
void dis_camera(void);            //显示目标追踪画面
void write_eeprom(double);        //写入EEPROM
void dis_menu(void);              //轮询按键
void set_value(void);             //解析遥控指令
void run_sonic(void);             //超声波执行函数

void setup()
{
  /******************读取EEPROM数据*****************/
  // for (char i = 0; i < 4; i++)
  //  pre_angle.dchar[i] = EEPROM.read(i);
  //angle_setpoint = pre_angle.data; //给平衡角度赋初值

  /*********************初始化IO********************/
  pinMode(12, OUTPUT);//电机引脚
  pinMode(6, OUTPUT); //电机引脚
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUTTON, INPUT);
  digitalWrite(BUTTON, HIGH);  //按钮需上拉
  digitalWrite(BUZZER, LOW);   //蜂鸣器低电平响
  digitalWrite(TRIG_PIN, LOW);
  FACE_LED(0);
  Motor(LFT, 0);
  Motor(RHT, 0);

  /*********************初始化通信********************/
  Serial.begin(115200);  //蓝牙串口
  Wire.begin();           //I2C总线

  /*********************开机logo********************/
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(20, 20);
  display.print("NANO");
  display.display();
  digitalWrite(BUZZER, HIGH);  //开机响一声

  delay(500);
  /*********************MPU6050初始化********************/
  i2cData[0] = 7;
  i2cData[1] = 0x00;
  i2cData[2] = 0x00;
  i2cData[3] = 0x00;
  while (i2cWrite(0x19, i2cData, 4, false));
  while (i2cWrite(0x6B, 0x01, true));
  while (i2cRead(0x75, i2cData, 1));
  if (i2cData[0] != 0x68)
    while (1);
  delay(100);

  while (i2cRead(0x3B, i2cData, 6));
  accX = (i2cData[0] << 8) | i2cData[1];
  accY = (i2cData[2] << 8) | i2cData[3];
  accZ = (i2cData[4] << 8) | i2cData[5];
  tempRaw = (i2cData[6] << 8) | i2cData[7];

  double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
  gyroYangle = pitch;
  compAngleY = pitch;

  timer = micros();
  /*********************其他初始化********************/
  camera_init();  //初始化摄像头
  attachInterrupt(0, ENCODER_L, FALLING); //开启编码器中断
  attachInterrupt(1, ENCODER_R, FALLING); //开启编码器中断
  digitalWrite(BUZZER, HIGH);
  FLAG |= NONSTART;

  /*********************显示自检信息********************/
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  for (char k = 0; k < strlen(info_check); k++)
  {
    char myChar =  pgm_read_byte_near(info_check + k);
    display.print(myChar);
  }
  for (char k = 0; k < strlen(info_temperature); k++)
  {
    char myChar =  pgm_read_byte_near(info_temperature + k);
    display.print(myChar);
  }

  display.print((double)tempRaw / 340.0 + 31.53);
  display.display();
}

void loop()
{
  /*********************检查是否摔倒********************/
  if (FLAG & FALLDOWN)
  {
    Motor(LFT, 0); //关电机
    Motor(RHT, 0); //关电机
    display.clearDisplay();
    display.drawBitmap(0, 0, fall_down, 128, 64, 1);
    display.display();
    while (1)  //等待按键重启
    {
      delay(500);
      if (BUTTON_ON)
        resetFunc();
    }
  }
  dis_menu();  //扫描按键检查是否进入设置模式/是否停机

#ifdef TIMING_DEBUG
  Serial.print(" 1.");
  Serial.print(micros());
#endif
  /*******************获取摄像头数据(2.5ms)****************/
#ifdef CAMERA_ENABLE
  if (micros() - CAM_timer > 50000)
  {
    camera_read();
    CAM_timer = micros();
    angle_PID_compute();
  }
#endif

  /*************************控制计算*******************/
#ifdef SPEED_LOOP
  speed_PID_compute();//0.5ms
#endif
  IMU_fillter();//3ms
  angle_PID_compute();

  /*********************获取超声波数据******************/
#ifdef SONIC_ENABLE
  if (micros() - SONIC_timer > 100000)
    run_sonic();
#endif

#ifdef TIMING_DEBUG
  Serial.print(" 2.");
  Serial.print(micros());
#endif
  /*********************串口打印数据**********************/
#ifndef TIMING_DEBUG
  Serial.print(F("V,"));
  Serial.print(atan2(accX, accZ) * RAD_TO_DEG + 180.0);
  Serial.print(F(","));
  Serial.print(gyroY / 131.0 + 180.0);
  Serial.print(F(","));
  Serial.print("0");
  Serial.print("\n");
#endif

#if 0
  Serial.print("\t");
  double temperature = (double)tempRaw / 340.0 + 36.53;
  Serial.print(temperature);
  Serial.print("\t");
#endif

  /*********************接收要看数据**********************/
  get_uart();  //获取蓝牙串口数据
  set_value(); //解析数据

#ifdef TIMING_DEBUG
  Serial.print(" 3.");
  Serial.print(micros());
#endif
  /*************************控制计算*******************/
#ifdef SPEED_LOOP
  speed_PID_compute();//0.5ms
#endif
  angle_PID_compute();
}
//end of loop

/********************中断函数*******************/
void ENCODER_L()
{
  if (micros() - ECL_timer > 1000)
  {
    if (direct_LR & 0x01)
      count_L++;
    else
      count_L--;
    ECL_timer = micros();
    FLAG |= ENCL;
  }
}

void ENCODER_R()
{
  if (micros() - ECR_timer > 1000)
  {
    if (direct_LR & 0x02)
      count_R++;
    else
      count_R--;
    ECR_timer = micros();
    FLAG |= ENCR;
  }
}
















