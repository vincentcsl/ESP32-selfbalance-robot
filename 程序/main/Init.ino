void ioInit(){
    pinMode(AIN1,OUTPUT);
    pinMode(AIN2,OUTPUT);
    pinMode(BIN1,OUTPUT);
    pinMode(BIN2,OUTPUT);
    pinMode(STBY,OUTPUT);
    // 设置并绑定通道和引脚
    ledcSetup(A_Channel, Freq, Resolution);
    ledcSetup(B_Channel, Freq, Resolution);
    ledcAttachPin(PWMA, A_Channel);
    ledcAttachPin(PWMB, B_Channel);
    // 开启模块驱动功能
    digitalWrite(STBY,HIGH);
}

void blueInit(){
    // 设置波特率
    Serial.begin(115200);
    // 蓝牙设备名称
    SerialBT.begin("Balance-Robot");
}

void wifiInit(){
    if (!WiFi.config(local_IP, gateway, subnet)) {
        Serial.println("STA Failed to configure");
    }
    WiFi.mode(WIFI_STA);
    // WiFi.setSleep(false); //关闭STA模式下wifi休眠，提高响应速度
    WiFi.begin(ssid, password);
    Serial.println("Connect WIFI");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");
    Serial.print("IP Address:");
    Serial.println(WiFi.localIP());
    server.begin(4322); //服务器启动监听端口号2333
    while (1) {
        client = server.available();
        if (client){
            Serial.println("[Client connected]");
            // if (client.connected()) {
                // client.print("OK!!!"); //向客户端发送
            // }
            // client.stop(); //结束当前连接:
            break;
        }
    }
}
