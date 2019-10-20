void angle_PID_compute(void){
    if (micros() - angle_PID_timer > 10000){
        /***************角度PID计算**************/
        double angle_error = kalAngleY - angle_setpoint ;
        angle_output = P_angle * angle_error + D_angle * gyroY;
        /************输出计算(叠加转向)************/
        output_L = (int)angle_output - turn_output;
        output_R = (int)angle_output + turn_output;
        // 累加最小电机PWM驱动值
        if (output_L > 0) {
            output_L += PWM_MIN_LEFT;
        }
        else {
            output_L -= PWM_MIN_LEFT;
        }
        if (output_R > 0) {
            output_R += PWM_MIN_RIGHT;
        }
        else {
            output_R -= PWM_MIN_RIGHT;
        }
        // 输出限幅
        output_L = constrain(output_L, -255, 255);
        output_R = constrain(output_R, -255, 255);

    #ifdef Motor_enable
        Motor(L_Motor, output_L);
        Motor(R_Motor, output_R);
    #endif

        angle_PID_timer = micros();
    }
}

void speed_PID_compute(void){
    if (micros() - speed_PID_timer > 40000){
        double speed_error = (knobLeft.read() - knobRight.read()) * 0.5 - speed_setpoint ;
        // speed_integral += speed_error;
        speed_output = P_speed * speed_error;
        angle_setpoint = speed_output;// + I_speed * speed_integral;

        knobLeft.write(0);
        knobRight.write(0);
        speed_PID_timer = micros();
    }
}
