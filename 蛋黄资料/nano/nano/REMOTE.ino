void get_uart(void)
{
  while (Serial.available() > 0)
  {
    char nullByte = char(Serial.read());
    if (nullByte == ';')
    {
      comdata[data_p] = nullByte;
      data_p = 0;
      FLAG |= COMDONE;
    }
    else
    {
      comdata[data_p] = nullByte ;
      data_p++ ;
    }
  }
}

void set_value(void)
{
  if (FLAG & COMDONE)
  {
    strtok(comdata, ",");
    joy_x = atof(strtok(NULL, ","));
    joy_y = atof(strtok(NULL, ";"));
    switch (comdata[1])
    {
      case 'J':
        FLAG |= CTRLING;
        speed_setpoint = joy_y * 30;
        if (joy_x > 0.2 || joy_x < -0.2)
        {
          FLAG |= TURNING;
          if (speed_setpoint > 0)
            turn_output = joy_x * 15;
          else
            turn_output = -joy_x * 15;
        }
        else
        {
          FLAG &= ~TURNING;
          turn_output = 0;
        }
        break;
      /******************************************/
      case 'M':
        FLAG |= CTRLING;
        speed_setpoint = (joy_x + 20);
        if (speed_setpoint > 30)
          speed_setpoint = 30;
        else if (speed_setpoint < -30)
          speed_setpoint = -30;
        if (joy_y > 10 || joy_y < -10)
        {
          FLAG |= TURNING;
          if (speed_setpoint > 0)
            turn_output = joy_y * 0.3;
          else
            turn_output = -joy_y * 0.3;
        }
        else
        {
          FLAG &= ~TURNING;
          turn_output = 0;
        }
        break;
      case 'S':
        speed_setpoint = 0;
        turn_output = 0;
        FLAG &= ~TURNING;
        FLAG &= ~CTRLING;
        break;
    }
    FLAG &= ~COMDONE;
  }
}





