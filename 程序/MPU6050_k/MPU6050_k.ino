#include <Wire.h>
#include <Kalman.h>

/***************MPU6050变量定义**********************/
Kalman kalmanY;
double accX, accY, accZ;
double gyroY, gyroZ;
double gyroYangle,kalAngleY;;
int16_t tempRaw;
uint32_t timer;
uint8_t i2cData[14];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  //蓝牙串口
  Wire.begin();           //I2C总线
  
#if ARDUINO >= 157
  Wire.setClock(400000UL); // Set I2C frequency to 400kHz
#else
  TWBR = ((F_CPU / 400000UL) - 16) / 2; // Set I2C frequency to 400kHz
#endif

  i2cData[0] = 7;
  i2cData[1] = 0x00;
  i2cData[2] = 0x00;
  i2cData[3] = 0x00;
  while (i2cWrite(0x19, i2cData, 4, false));
  while (i2cWrite(0x6B, 0x01, true));
  while (i2cRead(0x75, i2cData, 1));
  if (i2cData[0] != 0x68) { // Read "WHO_AM_I" register
    Serial.print(F("Error reading sensor"));
    while (1);
  }
  delay(100);

  while (i2cRead(0x3B, i2cData, 6));
  accX = (i2cData[0] << 8) | i2cData[1];
  accY = (i2cData[2] << 8) | i2cData[3];
  accZ = (i2cData[4] << 8) | i2cData[5];
  tempRaw = (i2cData[6] << 8) | i2cData[7];

  double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
  gyroYangle = pitch;

  timer = micros();
}

void loop() {
  // put your main code here, to run repeatedly:
  while (i2cRead(0x3B, i2cData, 14));
  accX = (int16_t)((i2cData[0] << 8) | i2cData[1]);
  accY = (int16_t)((i2cData[2] << 8) | i2cData[3]);
  accZ = (int16_t)((i2cData[4] << 8) | i2cData[5]);
  tempRaw = (int16_t)((i2cData[6] << 8) | i2cData[7]);
 // gyroX = (int16_t)((i2cData[8] << 8) | i2cData[9]);
  gyroY = (int16_t)((i2cData[10] << 8) | i2cData[11]);
  gyroZ = (int16_t)((i2cData[12] << 8) | i2cData[13]);;

  double dt = (double)(micros() - timer) / 1000000;
  timer = micros();

  double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
  double gyroYrate = gyroY / 131.0;
  gyroYangle += gyroYrate * dt;
  kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);
  Serial.println(kalAngleY);
}
