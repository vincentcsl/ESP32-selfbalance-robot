// MPU6050初始化函数
void MPU6050_init(void){
    //打开I2C总线
    Wire.begin();
    //设置I2C频率
#if ARDUINO >= 157
    Wire.setClock(400000UL); // Set I2C frequency to 400kHz
#else
    TWBR = ((F_CPU / 400000UL) - 16) / 2; // Set I2C frequency to 400kHz
#endif
    //初始化陀螺仪
    i2cData[0] = 7; // Set the sample rate to 1000Hz - 8kHz/(7+1) = 1000Hz
    i2cData[1] = 0x00; // Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering, 8 KHz sampling
    i2cData[2] = 0x00; // Set Gyro Full Scale Range to ±250deg/s
    i2cData[3] = 0x00; // Set Accelerometer Full Scale Range to ±2g
    while (i2cWrite(0x19, i2cData, 4, false)); // Write to all four registers at once
    while (i2cWrite(0x6B, 0x01, true)); // PLL with X axis gyroscope reference and disable sleep mode
    while (i2cRead(0x75, i2cData, 1));
    if (i2cData[0] != 0x68) { // Read "WHO_AM_I" register
      while (1){
          SerialBT.print(F("Error reading sensor"));
          delay(800);
      }
    }
    delay(100); // Wait for sensor to stabilize
    while (i2cRead(0x3B, i2cData, 6));
    accX = (int16_t)((i2cData[0] << 8) | i2cData[1]);
    accY = (int16_t)((i2cData[2] << 8) | i2cData[3]);
    accZ = (int16_t)((i2cData[4] << 8) | i2cData[5]);
    //计算pitch
    double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
    //初始化卡尔曼滤波参数
    kalmanY.setAngle(pitch);
    //获取时钟数
    timer = micros();
}

// 获取MPU6050数据并进行卡尔曼滤波
void IMU_fillter(void){
    // 尽可能的串入角度平衡计算
    angle_PID_compute();
    //读取原始数据
    while (i2cRead(0x3B, i2cData, 14));
    accX = (int16_t)((i2cData[0] << 8) | i2cData[1]);
    accY = (int16_t)((i2cData[2] << 8) | i2cData[3]);
    accZ = (int16_t)((i2cData[4] << 8) | i2cData[5]);
    tempRaw = (int16_t)((i2cData[6] << 8) | i2cData[7]);
    // gyroX = (int16_t)((i2cData[8] << 8) | i2cData[9]);
    gyroY = (int16_t)((i2cData[10] << 8) | i2cData[11]);
    gyroZ = (int16_t)((i2cData[12] << 8) | i2cData[13]);;

    // 尽可能的串入角度平衡计算
    angle_PID_compute();
    speed_PID_compute();
    //时间处理
    double dt = (double)(micros() - timer) / 1000000;
    timer = micros();
    //使用卡尔曼滤波处理数据
    double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
    double gyroYrate = gyroY / 131.0;

    // 尽可能的串入角度平衡计算
    angle_PID_compute();

    // gyroYangle += gyroYrate * dt;
    // 小车方向问题，用负号
    kalAngleY = - kalmanY.getAngle(pitch, gyroYrate, dt);
    //输出数据
    // Serial.print("kalAngleY:");
    // Serial.println(kalAngleY);
    speed_PID_compute();
 }
