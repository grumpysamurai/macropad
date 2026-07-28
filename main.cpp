#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SCL 26
#define SDA 27

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define CLK 32
#define DT 33
#define SW 25

#define DIN 19

Adafruit_NeoPixel led_strip(19, DIN, NEO_GRB + NEO_KHZ800);

#define KEY0 8
#define KEY1 15
#define KEY2 2
#define KEY3 0
#define KEY4 4
#define KEY5 16
#define KEY6 17
#define KEY7 5
#define KEY8 18

int keys[] = {KEY0, KEY1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8};

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 is running!");

  led_strip.begin();
  setLedColor(150, 0, 0);

  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  displaySetup();

  setupKeys();

  updateLine(0, "test 1 ikhidu");
  delay(1000);
  updateLine(0, "         ");

}

int lastPress = millis();

void loop() {
  readDial();
  readKeys();
}

void displaySetup(){
  Wire.begin(SDA, SCL);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void updateLine(int line, String text){
  display.setCursor(0, (line * 8));
  display.print("                 ");
  display.setCursor(0, (line * 8));
  display.print(text);
  display.display();
}

void setLedColor(int a, int b, int c){
    for(int i = 0; i < 8; i++) {
    led_strip.setPixelColor(i, led_strip.Color(a, b, c));
  }
  led_strip.show();
}

int currentSetting = 0;

int lastCLK = HIGH;
void readDial(){
  int newCLK = digitalRead(CLK);
  if(lastCLK != newCLK){
    int dtValue = digitalRead(DT);

    if(newCLK == LOW && dtValue == HIGH){
      Serial.println("clockwise");
      lastCLK = newCLK;
    }
    if(newCLK == LOW && dtValue == LOW){
      Serial.println("counterclockwise");
    }
    lastCLK = newCLK;
  }

  int swValue = digitalRead(SW);
  if(swValue == LOW && (millis() - lastPress) > 200){
    if(currentSetting == 2){
        currentSetting = 0;
      }else{
        currentSetting += 1;
      }
      Serial.println("current setting:" + String(currentSetting));
      updateMenu(currentSetting);
    lastPress = millis();
  }

}

void setupKeys(){
  for(int key : keys){
    pinMode(key, INPUT_PULLUP);
  }
  Serial.println("Keys have been setup!");
}

int currentKey = 0;
void readKeys(){
  currentKey = 0;
  for(int key : keys){
    int keyValue = digitalRead(key);
    if(keyValue == LOW && (millis() - lastPress) > 200){
      Serial.println(String(currentKey) + " was pressed");
      lastPress = millis();
    }
    currentKey += 1;
  }
}

String currentLightColor = "red";
int currentMacro = 0;

void updateMenu(int setting){

    switch(setting){
      case 0:
        updateLine(0, "SETTINGS");
        updateLine(1, "--------");
        updateLine(2, "Macro: " + String(currentMacro));
        updateLine(3, "Lights " + currentLightColor);
      break;

      case 1:
        updateLine(2, "Macro: " + String(currentMacro) + " <-");
        updateLine(3, "Lights " + currentLightColor);
      break;

      case 2:
        updateLine(2, "Macro: " + String(currentMacro));
        updateLine(3, "Lights " + currentLightColor + " <-");
      break;
    }
  }
    