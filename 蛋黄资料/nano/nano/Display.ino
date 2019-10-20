void drawRect(int16_t x, int16_t y, int16_t sx, int16_t sy)
{
  int16_t temp = x;
  while (x--)
  {
    display.drawPixel(sx + x, sy, WHITE);
    display.drawPixel(sx + x, sy + y, WHITE);
  }
  while (y--)
  {
    display.drawPixel(sx, sy + y, WHITE);
    display.drawPixel(sx + temp, sy + y, WHITE);
  }
}

void dis_camera(void)
{
  if (camera_read())
  {
    int sx = map(camera_Target.X, 0, 1000, 0, 128);
    int sy = map(camera_Target.Y, 0, 800, 0, 128);
    display.clearDisplay();
    drawRect(camera_Target.Size * 5, camera_Target.Size * 5, sx, sy);
    display.display();
  }
}

void dis_menu(void)
{
  if (FLAG & NONSTART)
  {
    while (BUTTON_OFF)
    {
      dis_camera();
      while (i2cRead(0x3B, i2cData, 14));
      accX = ((i2cData[0] << 8) | i2cData[1]);
      accY = ((i2cData[2] << 8) | i2cData[3]);
      accZ = ((i2cData[4] << 8) | i2cData[5]);
      tempRaw = (i2cData[6] << 8) | i2cData[7];
      gyroY = (i2cData[10] << 8) | i2cData[11];
      gyroZ = (i2cData[12] << 8) | i2cData[13];

      double dt = (double)(micros() - timer) / 1000000;
      timer = micros();

      double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
      double gyroYrate = gyroY / 131.0;
      gyroYangle += gyroYrate * dt;

      compAngleY = 0.93 * (compAngleY + gyroYrate * dt) + 0.07 * pitch;
    }
    delay(1000);
    if (BUTTON_ON)
    {
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(12, 0);
      display.print("-SETTING-");
      display.setTextSize(3);
      display.setCursor(20, 30);
      display.print(pre_angle.data);
      display.display();
      while (BUTTON_ON);
      while (1)
      {
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(12, 0);
        display.print("-SETTING-");
        display.setTextSize(3);
        display.setCursor(20, 30);
        display.print(pre_angle.data);
        display.display();
        if (FLAG & ENCL)
        {
          pre_angle.data -= 0.1;
          FLAG &= ~ENCL;
        }
        if (FLAG & ENCR)
        {
          pre_angle.data += 0.1;
          FLAG &= ~ENCR;
        }
        if (BUTTON_ON)
        {
          write_eeprom(pre_angle.data);
          display.clearDisplay();
          display.setTextSize(3);
          display.setCursor(20, 20);
          display.print("DONE!");
          display.display();
          delay(1000);
        }
        delay(300);
      }
    }
    // brightness = 255;
    FLAG &= ~NONSTART;
  }
  else
  {
    if (BUTTON_ON)
      FLAG |= FALLDOWN;
  }
}


