//PWM相关常量
#define Freq 5000
#define A_Channel 0
#define B_Channel 1
#define Resolution 8

//L298N模块相关引脚
#define PwmA 32
#define Ain2 33
#define Ain1 25
#define STBY 26
#define Bin1 27
#define Bin2 14
#define PwmB 12



//电机驱动函数
void Motor(bool M,bool D,int S){ //M==0时驱动A   D==1时向前
    if(M){
        ledcWrite(B_Channel, S);
        if(D){
            digitalWrite(Bin1,0);
            digitalWrite(Bin2,1);
          }
        else{
            
            digitalWrite(Bin1,1);
            digitalWrite(Bin2,0);
          }
      }
    else{
        ledcWrite(A_Channel, S);
        if(D){
            digitalWrite(Ain1,0);
            digitalWrite(Ain2,1);
          }
        else{
            
            digitalWrite(Ain1,1);
            digitalWrite(Ain2,0);
          }
      }
  }

void setup() {
  pinMode(Ain1,OUTPUT);
  pinMode(Ain2,OUTPUT);
  pinMode(Bin1,OUTPUT);
  pinMode(Bin2,OUTPUT);
  pinMode(STBY,OUTPUT);

  //设置并绑定通道和引脚
  ledcSetup(A_Channel, Freq, Resolution);
  ledcSetup(B_Channel, Freq, Resolution);
  ledcAttachPin(PwmA, A_Channel);
  ledcAttachPin(PwmB, B_Channel);
  //开启模块驱动功能
  digitalWrite(STBY,HIGH);
}
 
void loop(){
  Motor(0,1,255);
  Motor(1,1,255);
}
