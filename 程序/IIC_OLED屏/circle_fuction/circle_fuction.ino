#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example


void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    while(1){
        Serial.println(F("SSD1306 allocation failed"));
        delay(500);
      }
  }
}

void loop() {
  
  display.clearDisplay();
  /*
  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=3) {
    display.fillCircle(display.width() / 2, display.height() / 2, i, WHITE);
    display.display(); // Update screen with each newly-drawn circle
    delay(1);
  }*/
//  画圆:display.fillCircle();输出到屏幕:display()
  display.fillCircle(display.width() / 2, display.height() / 2, 32, WHITE);
  display.fillCircle(0,0,32, WHITE);
  display.display(); // Update screen with each newly-drawn circle
}
