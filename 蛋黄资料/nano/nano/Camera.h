const uint8_t slaveAddress = 0xD8;

struct Target
{
  int X, Y, Size;
} camera_Target;

void Write_2bytes(byte d1, byte d2)
{
  Wire.beginTransmission(slaveAddress);
  Wire.write(d1); Wire.write(d2);
  Wire.endTransmission();
}

void camera_init ()
{
  Wire.begin();
  Write_2bytes(0x30, 0x01); delay(10);
  Write_2bytes(0x30, 0x08); delay(10);
  Write_2bytes(0x06, 0x90); delay(10);
  Write_2bytes(0x08, 0xC0); delay(10);
  Write_2bytes(0x1A, 0x40); delay(10);
  Write_2bytes(0x33, 0x33); delay(10);
  delay(100);
}

byte camera_read()
{
  byte result;
  byte data_buf[16];
  int i, s;

  Wire.beginTransmission(slaveAddress);
  Wire.write(0x36);
  Wire.endTransmission();
  Wire.requestFrom(slaveAddress, 16);
  for (i = 0; i < 16; i++)
    data_buf[i] = 0;

  i = 0;

  while (Wire.available() && i < 16)
  {
    data_buf[i] = Wire.read();
    i++;
  }
  result = 0;

  camera_Target.X = data_buf[1];
  camera_Target.Y = data_buf[2];
  s = data_buf[3];
  camera_Target.X += (s & 0x30) << 4;
  camera_Target.Y += (s & 0xC0) << 2;
  camera_Target.Size = (s & 0x0F);
  result = ( camera_Target.Size < 10) ? 1 : 0;

  return result;
}
