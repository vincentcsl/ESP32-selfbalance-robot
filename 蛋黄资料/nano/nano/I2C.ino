const uint8_t IMUAddress = 0x68;
const uint16_t I2C_TIMEOUT = 1000;

uint8_t i2cWrite(uint8_t registerAddress, uint8_t data, bool sendStop)
{
  return i2cWrite(registerAddress, &data, 1, sendStop);
}

uint8_t i2cWrite(uint8_t registerAddress, uint8_t *data, uint8_t length, bool sendStop)
{
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  Wire.write(data, length);
  uint8_t rcode = Wire.endTransmission(sendStop);

  if (rcode)
  {
    Serial.print(F("i2cWrite failed: "));
    Serial.println(rcode);
  }
  return rcode;
}

uint8_t i2cRead(uint8_t registerAddress, uint8_t *data, uint8_t nbytes)
{
  uint32_t timeOutTimer;
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  uint8_t rcode = Wire.endTransmission(false);
  if (rcode)
  {
    Serial.print(F("i2cRead failed: "));
    Serial.println(rcode);
    return rcode;
  }
  Wire.requestFrom(IMUAddress, nbytes, (uint8_t)true);
  for (uint8_t i = 0; i < nbytes; i++)
  {
    if (Wire.available())
      data[i] = Wire.read();
    else
    {
      timeOutTimer = micros();
      while (((micros() - timeOutTimer) < I2C_TIMEOUT) && !Wire.available());
      if (Wire.available())
        data[i] = Wire.read();
      else
        return 5;
    }
  }
  return 0;
}
