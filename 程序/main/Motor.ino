void Motor(char LR,int S){       //LR变量标志驱动电机  S标志速度
    if(LR){                      //右侧电机为 1
        if(S > 0){
            digitalWrite(BIN1,0);
            digitalWrite(BIN2,1);
        }
        else{
            digitalWrite(BIN1,1);
            digitalWrite(BIN2,0);
            S = -S;
        }
        ledcWrite(B_Channel, S);
    }
    else{
        if(S > 0){
            digitalWrite(AIN1,0);
            digitalWrite(AIN2,1);
        }
        else{
            digitalWrite(AIN1,1);
            digitalWrite(AIN2,0);
            S = -S;
        }
        ledcWrite(A_Channel, S);
    }
}
