void IMU_fillter(void)
{
  while (i2cRead(0x3B, i2cData, 14));
  accX = ((i2cData[0] << 8) | i2cData[1]);
  accY = ((i2cData[2] << 8) | i2cData[3]);
  accZ = ((i2cData[4] << 8) | i2cData[5]);
  tempRaw = (i2cData[6] << 8) | i2cData[7];
  gyroY = (i2cData[10] << 8) | i2cData[11];
  gyroZ = (i2cData[12] << 8) | i2cData[13];

  angle_PID_compute();

  double dt = (double)(micros() - timer) / 1000000;
  timer = micros();

  double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
  double gyroYrate = gyroY / 131.0;

  angle_PID_compute();

  gyroYangle += gyroYrate * dt;
  compAngleY = 0.93 * (compAngleY + gyroYrate * dt) + 0.07 * pitch;
  if (abs(compAngleY - pre_angle.data) > 15)
    FLAG |= FALLDOWN;

#ifdef IMU_OUTPUT
  Serial.println(compAngleY);
#endif
}

void write_eeprom(double)
{
  uint8_t  *p;
  p = pre_angle.dchar;
  for (char i = 0; i < 4; i++)
  {
    EEPROM.write(i, *p);
    p++;
  }
}
