#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Thread.h>
//#include <Fonts/FreeSansBoldOblique12pt7b.h>
#include <Fonts/final_pixel_revised9pt7b.h>
#include <Fonts/final_pixel_art11pt7b.h>
#include <Fonts/Vermin_Vibes_198920pt7b.h>
#include <Fonts/Vermin_Vibes_198913pt7b.h>
#define WHITE SSD1306_WHITE
#define BLACK SSD1306_BLACK

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3c

#define SDA 4 
#define SCL 5

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

//workoutRelated variables
String workoutList[15][5] = {
  //ordem: exercicio, peso, repeticoes, ajustes, num
  {"Rotador", "Elastico", "2:10", "Nada", ""},
  {"Gluteo", "Mini Band", "2:10", "Nada", ""},
  {"Remada", "Elastico", "2:10", "Nada", ""},
  {"Leg", "20KG", "4:10", "ALT:4", "3"},
  {"Chest", "17.5", "3:10", "ASS:4", "29"},
  {"Ext.", "22.5", "3:10", "APO:3", "5"},
  {"Row.", "22.5", "3:10", "ASS:3", "18"},
  {"Abd.", "55.0", "3:10", "Nada", "11"},
  {"Ombr.", "10.0", "3:10", "ASS:6", "15"},
  {"Flex.", "20.0", "3:10", "APO:4", "8"},
  {"Hammer", "15.0", "3:10", "ASS:2", "21"},
  {"Tricep", "20.0", "3:10", "ASS:6", "30"},
  {"Calf", "15.0", "3:10", "ENC:3", "6"},
  {"Curl", "20.0", "3:10", "ASS:5", "33"},
  {"Decli.", "Banco", "3:15", "ALT:3", ""}
  };
bool isFirst = true;
int exCount = 0;
const int btn1 = 17;

//clockFunc related variables
unsigned long currTime;
int lastTime = 0;
int lastClockTime = millis();
String days[] = {"SUN", "MON","TUE","WED","THU","FRI","SAT"};

//will be used as index for displaying the day
int dayCountWeek = 5;
int monthLength[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//The month is used as the index for monthLength, in this case this is december
int currMonth = 11;
int currIntDay = 27;

int clockT[2] = {23, 50};

String state[3] = {"workout", "time", "chronograph"};
int stateIndex = 0;
bool isSleeping = false;

Thread thr = Thread();

void setup() {
  Wire.setSDA(SDA);
  Wire.setSCL(SCL);
  Wire.begin();
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.clearDisplay();
  oled.fillScreen(BLACK);
  nextExercise(exCount);
  exToGo();
  printTime();
  pinMode(btn1, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  thr.onRun(clockFunc);
  thr.setInterval(0);
}

void centerAlign(String text){
  int16_t x, y;
  uint16_t w, h;
  oled.setTextColor(WHITE);
  oled.setTextSize(1);
  oled.getTextBounds(text, 0, 0, &x, &y, &w, &h);

  int cursorX = (oled.width()/2)-(w/2);
  int yPos = oled.getCursorY();
  oled.setCursor(cursorX, yPos);
  oled.print(text);
  }

void rightAlign(String text){
  int16_t x1, y1;
  uint16_t w1, h1;
  oled.setTextColor(WHITE);
  oled.setTextSize(1);
  oled.getTextBounds(text, 0, 0, &x1, &y1, &w1, &h1);

  int cursorX = oled.width() - w1;
  int yPos = oled.getCursorY();
  oled.setCursor(cursorX, yPos);
  oled.print(text);
}

void exToGo(){ 
  int16_t x3, y3;
  uint16_t w3, h3;
  oled.setFont(&final_pixel_revised9pt7b);

  String tgText = String((sizeof(workoutList)/sizeof(workoutList[0]))-(exCount)) + "TG";
  oled.getTextBounds(tgText, 0, 0, &x3, &y3, &w3, &h3);

  //fixed some dumb shit that the font did for some reason
  //it seemed that the textBounds function recognized it to have a different length than
  //it actually did
  if(exCount != 4){oled.setCursor(128-w3-2, 56);}
  else{oled.setCursor(128-w3-1, 56);}
  oled.fillRect(64, 40, 80, 40, BLACK);
  oled.print(tgText);
  oled.setFont();
}

void nextExercise(int exerciseCount){
  //reset the display and print the main exercise (with a prettier font)
  oled.fillRect(0, 0, 128, 38, BLACK);
  oled.setCursor(0, 0);
  oled.setTextColor(WHITE);
  oled.setTextSize(1);
  oled.setCursor(0, 15);
  oled.setFont(&final_pixel_art11pt7b);
  String currStr = workoutList[exerciseCount][0];
  oled.print(currStr + " ");
  oled.setFont();
  oled.setFont(&final_pixel_revised9pt7b);
  oled.println(workoutList[exerciseCount][4]);
  
 //sets the base configurations
  oled.setFont();
  oled.setTextSize(1);
  
  int16_t x2, y2;
  uint16_t w2, h2;
  oled.getTextBounds(workoutList[exerciseCount][1], 0, 0, &x2, &y2, &w2, &h2);
  int smallShitHeight = h2;
  oled.setCursor(0, 22 + smallShitHeight);
  
  oled.print(workoutList[exerciseCount][1]);
  centerAlign(workoutList[exerciseCount][2]);
  rightAlign(workoutList[exerciseCount][3]);
  oled.setCursor(0, oled.getCursorY() + smallShitHeight);
  if(isFirst){
    oled.drawFastHLine(0, oled.getCursorY(), 128, WHITE);
  isFirst = false;
    }
 }

void printTime(){
  oled.fillRect(0, 40, 70, 25, BLACK);
  oled.setFont(&final_pixel_revised9pt7b);
  oled.setTextColor(WHITE);
  oled.setTextSize(1);
  
  oled.setCursor(0, 56);
  if(clockT[0] > 9){
    oled.print(clockT[0]);
  }
  else{
    oled.print(0);
    oled.print(clockT[0]);
    }
  oled.print(":");
  if(clockT[1] > 9){
    oled.print(clockT[1]); 
    }
  else{
    oled.print("0");
    oled.print(clockT[1]);
  }
  oled.setFont();
  oled.setCursor(0, 0);
  oled.display();
}

void workoutState(){
 if(digitalRead(btn1) != true && (currTime - lastTime) > 300){
      exCount++;
      if(exCount == sizeof(workoutList)/sizeof(workoutList[0])){
        exCount = 0;
      }
      nextExercise(exCount);
      exToGo(); 
      oled.display();
  lastTime = millis();
   }
 }

void dayMonthFunc(){
    dayCountWeek++;
    currIntDay++;
    //note: we check dayCount considering Sunday to be index 0
    if(dayCountWeek == 7){dayCountWeek = 0;}
    if(currIntDay > monthLength[currMonth]){
        currIntDay = 0;
        if(currMonth == 11){currMonth = 0;}
        else{currMonth++;}
      }
  }

void clockFunc(){
  if((currTime - lastClockTime) > 5000){
    lastClockTime = millis();
    if(clockT[1] == 59){
      clockT[1] = 0;
      
      if(clockT[0] == 23){clockT[0] = 0; dayMonthFunc();}
      
      else{clockT[0]++;}
      }
    else{clockT[1]++;}
   if(state[stateIndex] == "workout"){printTime();}
   if(state[stateIndex] == "time"){printClockTime();}
    }
}

void sleepModeWorkout(){
  if((currTime-lastTime) > 300000 && isSleeping == false){
    isSleeping = true;
    lowBatteryFilter();
    while(digitalRead(btn1) == true){thr.run(); currTime = millis();}
    lastTime = millis();
    wakeModeWorkout();
    }
  }

void offMode(){
   oled.clearDisplay();
   oled.fillScreen(BLACK);
   oled.display();
      while(digitalRead(18) == true){thr.run();}
      shutDownCurrMode();
}

void wakeModeWorkout(){
  isFirst = true;
  nextExercise(exCount);
  exToGo();
  printTime();
  isSleeping = false;
  }

void lowBatteryFilter(){
    int coord = 0;
    int shift = 0;
    while(coord < 64){
        for(int x = 0; x < 200;){
          x = x + 2;
          oled.drawPixel(x-shift, coord, BLACK);
          }
        oled.display();  
        coord++;
        if(shift < 5){shift++;}
        else{shift = 0;}
      }
}

void printClockTime(){
  oled.setFont();
  oled.setFont(&Vermin_Vibes_198920pt7b);
  oled.setTextSize(1);
  
  String timeMessage = "";
  if(clockT[0] > 9){
    timeMessage = timeMessage + String(clockT[0]);
  }
  else{
    timeMessage = timeMessage + "0" + String(clockT[0]);
    }
  timeMessage = timeMessage + ":";
  if(clockT[1] > 9){
    timeMessage = timeMessage + String(clockT[1]);
    }
  else{
    timeMessage = timeMessage + "0" + String(clockT[1]);
  }
  int16_t x4, y4;
  uint16_t w4, h4;
  oled.getTextBounds(timeMessage, 30, 0, &x4, &y4, &w4, &h4);
  oled.fillRect(0, 0, 95, h4+2, BLACK);
  oled.fillRect(97, 0, 40, 64, BLACK);  
  oled.setCursor((48-(w4/2)), h4);
  oled.print(timeMessage);
  oled.drawFastVLine(96, 0, 80, WHITE);
  oled.drawFastHLine(0, h4+5, 95, WHITE);
  
  //space for clock = 95 horizontal & h4+5 vertical
  oled.setFont();
  oled.setFont(&final_pixel_revised9pt7b);
  
  oled.setRotation(1);
  oled.setCursor(0, 28);
  oled.print(days[dayCountWeek]);

  oled.setFont();
  oled.setFont(&Vermin_Vibes_198913pt7b);

  int16_t x5, y5;
  uint16_t w5, h5;
  oled.getTextBounds(String(currIntDay), 30, 0, &x5, &y5, &w5, &h5);
  oled.setCursor(64-w5, h5);
  oled.print(currIntDay);
  int c = 4;
  //top line decorations
  //while(oled.getCursorY() >= 0){
    //oled.setCursor(0, (h5/2)-c);
    //if(c >= 6){
      //oled.drawFastHLine(oled.getCursorX(), oled.getCursorY(), 64, WHITE);
      //}
    //else{
      //oled.drawFastHLine(oled.getCursorX(), oled.getCursorY(), 64-w5-3, WHITE);
    //}
    //c = c + 2;
  //}
  
  //c = 8;

  //bottom line decorations
  //while(oled.getCursorY() < 28){
    //oled.setCursor(64-w5, h5+c);
    
    //if(c >= 12){
      //oled.drawFastHLine(0, oled.getCursorY(), 64, WHITE);
      //}
    //else{
      //oled.drawFastHLine(oled.getCursorX(), oled.getCursorY(), 32, WHITE);
   // }
    //c = c + 2;
   //}
  oled.setFont();
  oled.setRotation(4);
  oled.display();
  }

void shutDownCurrMode(){
  oled.clearDisplay();
  oled.fillScreen(BLACK);
  stateIndex++;
  if((sizeof(state)/sizeof(state[0])) == stateIndex){stateIndex = 0;}

  if(state[stateIndex] == "workout"){
    //how tf are there only 3 functions to boot this and like 20 written
    //kill me
    isFirst = true;
    nextExercise(exCount);
    exToGo();
    printTime();
    }
   if(state[stateIndex] == "time"){
    printClockTime();
    }
    if(state[stateIndex] == "chronograph"){oled.display();}
  }

void loop() {
  currTime = millis();
  thr.run();
  if(state[stateIndex] == "workout"){
    sleepModeWorkout();
    workoutState();
    }
  else{
  if(digitalRead(btn1) != true){
    int shutOffCount = 0;
    while(digitalRead(btn1) != true){
      thr.run();
      if((millis()%5000) == 0){
        shutOffCount++;
        if(shutOffCount > 20){offMode();}
        }
      }    
    }
  }
  if(digitalRead(18) != true && (currTime - lastTime) > 300){
    lastTime = millis();
    shutDownCurrMode();
    }
}
