#define BLINKER_WIFI
#define moto1 D5
#define moto2 D0

#include <Arduino.h>
#include "Adafruit_VL53L0X.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Blinker.h>

#define BUTTON_1 "btn-abc"//出食

char auth[] = "xxxxxxxxxx";//key
char ssid[] = "「Elysia」";//SSID
char pswd[] = "xxxxxxxxx";//password

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
Adafruit_SSD1306 oled(128,64,&Wire,-1);
int DIST;
int b;//启动时间

BlinkerNumber quantity("quan");



void heartbeat()
{
  quantity.print(DIST);
}

void number()
{
  if (DIST>90)
  {
    oled.println("##########");
  }
  else if (DIST>80&&DIST<90)
  {
    oled.println("#########");
  }
  else if (DIST>70&&DIST<80)
  {
    oled.println("########");
  }
    else if (DIST>60&&DIST<70)
  {
    oled.println("#######");
  }
      else if (DIST>50&&DIST<60)
  {
    oled.println("######");
  }
        else if (DIST>40&&DIST<50)
  {
    oled.println("#####");
  }
         else if (DIST>30&&DIST<40)
  {
    oled.println("####");
  }
          else if (DIST>20&&DIST<30)
  {
    oled.println("###");
  }
            else if (DIST>10&&DIST<20)
  {
    oled.println("##");
  }
              else if (DIST>0&&DIST<10)
  {
    oled.println("#");
  }
}

void VL53L0X()
{
    // if (lox.isRangeComplete())
    // {
    DIST=lox.readRange()-20;
    // } 
}

void moto()
{
     for (b=40;b > 0;b--)
  { 
    oled.setCursor(0,50);
    oled.print("M");
    analogWrite(moto1,400);
    digitalWrite(moto2,LOW);
    BLINKER_LOG(b);
    Blinker.delay(10);

  }
  digitalWrite(moto1,LOW);
  digitalWrite(moto2,LOW);
}

BlinkerButton Button1(BUTTON_1);
void button1_callback(const String & state) {
    BLINKER_LOG("get button state: ", state);
    // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    moto();
}

void setup() {
  
  Serial.begin(115200);
  BLINKER_DEBUG.stream(Serial);
  pinMode (moto1,OUTPUT);
  pinMode (moto2,OUTPUT);
  analogWriteFreq(30000);//PWM频率
 
  Blinker.begin(auth, ssid, pswd);

  Button1.attach(button1_callback);
  Blinker.attachHeartbeat(heartbeat);
  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
  lox.startRangeContinuous();
  Blinker.delay(10);
  oled.begin(SSD1306_SWITCHCAPVCC,0x3c);
  oled.setTextColor(WHITE);
  oled.clearDisplay();
  oled.setTextSize(2);
}


void loop() {
  Blinker.run();
  VL53L0X_RangingMeasurementData_t measure;
  VL53L0X();
  delay(10);
  // Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  oled.display();
  oled.clearDisplay();
  oled.setCursor(5,10);
  number();
  oled.setCursor(50,40);
  oled.println(DIST);
 
  Serial.println(DIST);
  // oled.println(lnmeasure.RangeMilliMeter);
  Blinker.delay(100);
}