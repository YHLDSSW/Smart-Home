/*
v2.3
已完成
模式选择
自动调节 pwm/leds
手动调节 pwm/leds
颜色调节
高频pwm调光
8833驱动
人体感应夜灯
未完成

D0 GPIO16 LED IN
D1 GPIO5  SCL
D2 GPIO4  SDA
D3 GPIO0  LED PWM
D4 GPIO2  IR_LED
D5 GPIO14  AIN1
D6 GPIO12  AIN2
D8 GPIO15  IR
*/

#define BLINKER_WIFI
#define NUM_LEDS 60               // LED灯珠数量
#define DATA_PIN D0              // Arduino输出控制信号引脚
#define LED_TYPE WS2812         // LED灯带型号
#define COLOR_ORDER GRB         // RGB灯珠中红色、绿色、蓝色LED的排列顺序
#define LED D3
#define IR_LED D4
#define moto1 D5
#define moto2 D6
#define IR D8
#define BUTTON_1 "btn-abc"      //新建开关
#define BUTTON_2 "btn-win"      //新建开关
#define Slider_1 "ran-sgj"      //新建滑块 
#define RGB_1 "col-oh0"         //新建色盘


#include <Arduino.h>
#include <Blinker.h>
#include <FastLED.h>  
#include <Wire.h>
#include <math.h>

char auth[] = "xxxxxxxxxx";//key
char ssid[] = "「Elysia」";//SSID
char pswd[] = "xxxxxxxxx";//password

// int LED=4;//端口
int i;//亮度
// int b;//bh1750
int a=0;//电机启动时间
int t;//缓慢亮灭延时

int IR_Flag;

int r=255;//r
int g=255;//g
int b=255;//b
int bright=255;//亮度

int AT_bright;//auto
int MT_bright;//manual

int AT_LED;
int MT_LED;
int Mode=1;//模式
// SCL= D7;
// SDA= D6;
int BH1750address = 0x23;//bh1750地址

uint16_t val=0;//bh1750

byte buff[2];

CRGB leds[NUM_LEDS];            // 建立光带leds


BlinkerSlider Slider1(Slider_1);//led调光
void slider1_callback(int32_t value)//按下按钮执行函数
{   
    MT_LED=value;
    // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    BLINKER_LOG("get slider value: ", value);
    
}

BlinkerRGB RGB1(RGB_1);//灯带调光
void rgb1_callback(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value)
{
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    BLINKER_LOG("R value: ", r_value);
    BLINKER_LOG("G value: ", g_value);
    BLINKER_LOG("B value: ", b_value);
    BLINKER_LOG("Rrightness value: ", bright_value);
    r=r_value;
    b=b_value;
    g=g_value;
    MT_bright=bright_value;
}

int BH1750_Read(int address) //
{
  int l=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()) //
  {
    buff[l] = Wire.read();  // receive one byte
    l++;
  }
  Wire.endTransmission();   
  return l; 
}

void moto_forword()
{
   for (a;a<20;a++)
  {
    analogWrite(moto1,200);
    digitalWrite(moto2,LOW);
    Blinker.delay(10);
  }

  digitalWrite(moto1,LOW);
  digitalWrite(moto2,LOW);

}

void moto_back()
{
   for (a;a>0;a--)
  {
    analogWrite(moto2,200);
    digitalWrite(moto1,LOW);
    Blinker.delay(10);
  }

  digitalWrite(moto1,LOW);
  digitalWrite(moto2,LOW);

}

void IR_Light()
{

  for (t; t < 200; t++)
  {
    analogWrite(IR_LED,t);
    Blinker.delay(10);
  }
}
void IR_Out()
{
  for (t;t > 0; t--)
  {
  analogWrite(IR_LED,t);
  Blinker.delay(10);
  }
  if (t==0)
  {
   digitalWrite(IR_LED,LOW);
  }
}

void IR_IN()
{
  if (IR_Flag==1)
{
  if (val<100)
  {
    IR_Light();
    Blinker.delay(10);
  }
}

  else
  {
    IR_Out();
    Blinker.delay(10);
  }

}



void BH1750_Init(int address) 
{
  Wire.beginTransmission(address);
  Wire.write(0x10);
  //1lx reolution 120ms
  Wire.endTransmission();
}
void BH1750()
{
    // int l;
    // uint16_t val=0;
    BH1750_Init(BH1750address);
    Blinker.delay(1000);
    if(2==BH1750_Read(BH1750address))
    val=((buff[0]<<8)|buff[1])/1.2;
    BLINKER_LOG("LX:",val);     

    if (val>2500)
    {
      AT_bright=0; 
      AT_LED=0;
      moto_forword();
    }
    else
    {
      AT_bright=255-(val/10);
      AT_LED=250-(val/10);
      
    } 
    if (val<100)
    {
      moto_back();
    }
    
}

BlinkerButton Button1(BUTTON_1);
void button1_callback(const String & state)
{
    // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    BLINKER_LOG("get button state: ", state);
     if (state == BLINKER_CMD_ON) {
        BLINKER_LOG("AUTO on!");

        Button1.icon("icon_1");
        Button1.color("#FF69B4");
        Button1.text("AUTO");
        // Button1.text("Your button name", "describe");
        Button1.print("on");
        Mode=1;
    }
    else if (state == BLINKER_CMD_OFF) {
        BLINKER_LOG("AUTO off!");

        Button1.icon("icon_1");
        Button1.color("#FFFFFF");
        Button1.text("AUTO");
        // Button1.text("Your button name", "describe");
        Button1.print("off");
        Mode=0;
    }
    else {
        BLINKER_LOG("Get user setting: ", state);

        Button1.icon("icon_1");
        Button1.color("#FFFFFF");
        Button1.text("Your button name or describe");
        // Button1.text("Your button name", "describe");
        Button1.print();
        Mode=1;//默认自动
    }
}

BlinkerButton Button2(BUTTON_2);
void button2_callback(const String & state)
{
    // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    BLINKER_LOG("get button state: ", state);
     if (state == BLINKER_CMD_ON) {
        BLINKER_LOG("on!");
        Button2.icon("fad fa-blinds-open");
        Button2.color("#A4A4A4");
        // Button1.text("Your button name", "describe");
        Button2.print("on");
        moto_forword();
    }
    else if (state == BLINKER_CMD_OFF) {
        BLINKER_LOG("off!");
        Button2.color("#A4A4A4");
        Button2.icon("fad fa-blinds-raised");
        // Button1.text("Your button name", "describe");
        Button2.print("off");
         moto_back();
       
    }
    else {
        BLINKER_LOG("Get user setting: ", state);
        Button2.color("#A4A4A4");
        // Button3.text("Your button name or describe");
        // Button1.text("Your button name", "describe");
        Button2.print();
    }
}

void Auto_mode()//模式选择
{
  if (Mode==1)
  {
    bright=AT_bright;
    i=AT_LED;
  }
  else
  {
    bright=MT_bright;
    i=MT_LED;
  }
}

void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);

    Blinker.vibrate();
    
    uint32_t BlinkerTime = millis();
    
    Blinker.print("millis", BlinkerTime);

}




void setup()
{
    Wire.begin();//i2c

    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);

    pinMode (LED,OUTPUT);//设置输出
    pinMode (moto1,OUTPUT);
    pinMode (moto2,OUTPUT);
    pinMode (IR_LED,OUTPUT);
    pinMode (IR,INPUT_PULLUP);

    analogWriteFreq(30000);//PWM频率

    // pinMode(LED_BUILTIN, OUTPUT);
    // digitalWrite(LED_BUILTIN, LOW);

    Blinker.begin(auth, ssid, pswd);
    // Blinker.attachData(dataRead);
    Blinker.attachData(dataRead);

    Button1.attach(button1_callback);//模式切换
    Button2.attach(button2_callback);//模式切换
    Slider1.attach(slider1_callback);//注册按钮
    RGB1.attach(rgb1_callback);

    LEDS.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS); // 初始化光带  
    FastLED.setBrightness(bright);  
}

void loop()
{   IR_Flag=digitalRead(IR);
    IR_IN();
    BH1750();
    Auto_mode();
    analogWrite(LED,i); //PWM调节
    FastLED.setBrightness(bright); //亮度调节
    fill_solid(leds, 60, CRGB(r,g,b));//RGB颜色调节
    FastLED.show(); //刷新颜色
    // BLINKER_LOG("LED:",i); 
    // BLINKER_LOG("LEDS:",bright); 
    BLINKER_LOG("IR:",IR_Flag); 
    BLINKER_LOG("T:",t); 
    Blinker.delay(150);
    Blinker.run();
}