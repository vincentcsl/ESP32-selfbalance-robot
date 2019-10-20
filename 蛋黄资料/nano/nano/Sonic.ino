void run_sonic(void)
{
  bitClear(PORTB, 0);
  bitSet(PORTB, 0);
  delayMicroseconds(10);
  bitClear(PORTB, 0);
  distance_cm = pulseIn(ECHO_PIN, HIGH, 1500) * 0.017;
  SONIC_timer = micros();
#ifdef SONIC_OUTPUT
  Serial.println(distance_cm);
#endif
  if (!(FLAG & CTRLING))
  {
    if (distance_cm > 10 && distance_cm < 15 )
    {
      speed_setpoint = 30;
      FACE_LED(255);
    }
    else if (distance_cm < 8  && distance_cm > 0)
    {
      speed_setpoint = -30;
      FACE_LED(255);
    }
    else
    {
      speed_setpoint = 0;
      FACE_LED(0);
    }
  }

  angle_PID_compute();
}
