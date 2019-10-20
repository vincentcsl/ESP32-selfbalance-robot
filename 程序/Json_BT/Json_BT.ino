// 包含蓝牙串口库
#include <BluetoothSerial.h>
// 包含Json库
#include <ArduinoJson.h>
// 一定要用char变量接收串口数据，再传递给string
// 不然一定会解析失败

DynamicJsonDocument doc(JSON_OBJECT_SIZE(2) + 10);
BluetoothSerial SerialBT;
void BT_uart(){
    if (SerialBT.available()) {
        // string不要定义为全局变量，否则有解析失败风险
        String uart_string;
        char tmp;
        do {
            tmp = SerialBT.read();
            uart_string = uart_string + tmp;
        } while(SerialBT.available());
        // 进行错误检查
        DeserializationError error = deserializeJson(doc, uart_string);
        if (!error) {
            Serial.write("Json_OK.");
            int P = doc["P"];
            Serial.write(P);
        }
        else{
            Serial.write("Json_Error!!!");
        }
    }
}

void setup() {
    //设置波特率
    Serial.begin(115200);
    SerialBT.begin("Balance_Robot");
}

void loop() {
    BT_uart();
}
