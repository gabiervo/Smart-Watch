#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <EEPROM.h>
#include "sprite.hpp"

#define WHITE SSD1306_WHITE
#define BLACK SSD1306_BLACK

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3c

#define SDA 0 
#define SCL 1

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

void setup() {
  Wire.setSDA(SDA);
  Wire.setSCL(SCL);
  Wire.begin();
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.clearDisplay();
  oled.fillScreen(BLACK);
  oled.drawBitmap(64, 32, bmp, 40, 15, WHITE);
  oled.display();
}

void loop(){
  }
