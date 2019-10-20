#include <Encoder.h>

#define Encoder_A_1 27
#define Encoder_A_2 14
#define Encoder_A_1_interrupt 7
#define Encoder_A_2_interrupt 6

#define Encoder_B_1 33
#define Encoder_B_2 13
#define Encoder_B_1_interrupt 8
#define Encoder_B_2_interrupt 4



// Change these pin numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder knobRight(Encoder_A_1,Encoder_A_2);//构造编码器对象
Encoder knobLeft(Encoder_B_1,Encoder_B_2);

int flag=0;
void ENCODER_L(){
    flag=1;
  }

void setup() {
  Serial.begin(115200);
  Serial.println("TwoKnobs Encoder Test:");
  attachInterrupt(Encoder_A_1_interrupt, ENCODER_L, FALLING); 
  //attachInterrupt(1, ENCODER_R, FALLING); 
  //attachInterrupt(0, ENCODER_L, FALLING); 
  //attachInterrupt(1, ENCODER_R, FALLING); 
}

long positionLeft  = -999;
long positionRight = -999;

void loop() {
  /*
  long newLeft, newRight;
  newLeft = knobLeft.read();
  newRight = knobRight.read();
  if (newLeft != positionLeft || newRight != positionRight) {
    Serial.print("Left = ");
    Serial.print(newLeft);
    Serial.print(", Right = ");
    Serial.print(newRight);
    Serial.println();
    positionLeft = newLeft;
    positionRight = newRight;
  }
  // if a character is sent from the serial monitor,
  // reset both back to zero.
  if (Serial.available()) {
    Serial.read();
    Serial.println("Reset both knobs to zero");
    knobLeft.write(0);
    knobRight.write(0);
  } */
  if(flag){
      Serial.println("YES!");
      flag=0;
    }
}
