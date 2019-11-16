#ifdef WifiPrintValue
    void wifiPrintValue(){
        if (micros() - tmp_timer > 120000) {  //flutter帧数max = 60Hz 16.6ms一次重构  50ms一次表现良好 40ms达到视觉残留标准
            client.print(kalAngleY);
            client.print(",");
            client.print(gyroY);
            client.print(";");
            // Serial.println(kalAngleY);
            tmp_timer=micros();
        }
    }
#endif
