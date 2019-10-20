void Motor(char LR, int SPEED)
{
  if (LR)
  {
    if (SPEED < 0)
    {
      direct_LR &= ~0x02;
      bitClear(PORTB, 4);
      analogWrite(10, SPEED);
    }
    else
    {
      direct_LR |= 0x02;
      bitSet(PORTB, 4);
      analogWrite(10, 255 - SPEED);
    }
  }
  else
  {
    if (SPEED < 0)
    {
      direct_LR &= ~0x01;
      bitSet(PORTD, 6);
      analogWrite(9, 255 - SPEED);
    }
    else
    {
      direct_LR |= 0x01;
      bitClear(PORTD, 6);
      analogWrite(9, SPEED);
    }
  }
}

