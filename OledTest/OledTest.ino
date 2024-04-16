#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <SPI.h>

#define WHITE SSD1306_WHITE

const int btn1 = 0;
const int btn2 = 1;
const int btn3 = 2;
const int btn4 = 3;

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

void setup()
{
  Wire.begin();
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.clearDisplay();
  oled.fillScreen(WHITE);
  oled.display();

  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  pinMode(btn4, INPUT_PULLUP);

  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
}

byte x = 0;

void loop()
{
  if(digitalRead(btn1) == LOW || digitalRead(btn2) == LOW || digitalRead(btn3) == LOW || digitalRead(btn4) == LOW){
    digitalWrite(6, HIGH);
  }
  else{
    digitalWrite(6, LOW);
  }
}
