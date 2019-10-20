#define interruptPin 27
volatile int flag = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void handleInterrupt() {
 
  portENTER_CRITICAL_ISR(&mux);
 
  flag = 1;
 
  portEXIT_CRITICAL_ISR(&mux);
 
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("SET UP！");
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(flag){
      Serial.println("YES!!!");
      portENTER_CRITICAL(&mux);
 
      flag = 0;
 
      portEXIT_CRITICAL(&mux);
 
  }
}
