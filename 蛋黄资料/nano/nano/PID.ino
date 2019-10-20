void angle_PID_compute(void)
{
  double output_L, output_R;
  if (micros() - angle_PID_timer > 10000)
  {
#ifdef TIMING_DEBUG
    Serial.print(" A.");
    Serial.print(millis());
#endif
    /***************angle calculate**************/
    double angle_error = compAngleY - angle_setpoint ;
    angle_integral += angle_error;
    angle_output = P_angle * angle_error + I_angle * angle_integral + D_angle * gyroY;

    /***************turn calculate**************/
    if (FLAG & TURNING)
      turn_integral = 0;
    else
    {
      turn_integral += gyroZ;
      turn_output = P_turn * gyroZ + I_turn * turn_integral;
    }

    /***************output**************/
    output_L = angle_output;
    output_R = angle_output;

    if (output_L > 255)
      output_L = 255;
    if (output_L < -255)
      output_L = -255;
    if (output_R > 255)
      output_R = 255;
    if (output_R < -255)
      output_R = -255;

#ifdef MOTOR_ENABLE
    Motor(LFT, output_L);
    Motor(RHT, output_R);
#endif

    angle_PID_timer = micros();
  }
}

void speed_PID_compute(void)
{
  if (micros() - speed_PID_timer > 50000)
  {
#ifdef TIMING_DEBUG
    Serial.print(" S.");
    Serial.print(millis());
#endif
    double speed_error = (count_L + count_R ) * 0.5 - speed_setpoint ;
    speed_integral += speed_error;
    angle_setpoint = P_speed * speed_error + I_speed * speed_integral;

    speed_PID_timer = micros();
  }
}



