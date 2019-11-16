void BT_uart(){
    if (SerialBT.available()) {
        char BT_date;
        String uart_string;
        do {
            BT_date = SerialBT.read();
            uart_string = uart_string + BT_date;
        } while(SerialBT.available());
        // 解析并进行错误检查
        DeserializationError error = deserializeJson(doc, uart_string);

        // JSON数据形如:
        // {"UP":1,"Y":10,"X":0,"A": [33,-0.004],"S": 0.0625}

        if (!error) {
            // 重新启动
            if (doc.containsKey("UP")) {
                if ((int)doc["UP"]){
                    FALLDOWN = 0;
                }
                else {
                    FALLDOWN = 1;
                }
            }

            // 更新控制参数
            if (doc.containsKey("Y")) {
                double tmp_speed = doc["Y"];
                speed_setpoint += tmp_speed;
                speed_setpoint = constrain(speed_setpoint, SPEED_MIN, SPEED_MAX);
            }
            if (doc.containsKey("X")) {
                double tmp_turn = doc["X"];
                turn_output += tmp_turn;
                turn_output = constrain(turn_output, TURN_MIN, TURN_MAX);
            }

            #ifdef ADJUST_PID
            // 更新角度,速度,转向环PID参数
            if (doc.containsKey("A")) {
                P_angle = doc["A"][0];
                D_angle = doc["A"][1];
                // Serial.println("Do!!!");
            }
            if (doc.containsKey("S")) {
                P_speed = doc["S"];
            }
            #endif

        }
    }
}
