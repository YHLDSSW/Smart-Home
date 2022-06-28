

#define BLINKER_WIFI
#define DHTPIN D3
#define LED D4
#define ADC A0
#define MotoA_1 D5
#define MotoA_2 D0
#define MotoB_1 D6
#define MotoB_2 D7

#define BUTTON_1 "btn-35a" //自动开关
#define BUTTON_2 "btn-8dn" //浇水
#define BUTTON_3 "btn-hv5" //遮阳
#define BUTTON_4 "btn-abc" //补光

#define TEXTE_1 "TextKey"

#define DHTTYPE DHT11

#include <Arduino.h>
#include <Blinker.h>
#include <Wire.h>
#include <math.h>
#include <DHT.h>

char auth[] = "xxxxxxxxxx";//key
char ssid[] = "「Elysia」";//SSID
char pswd[] = "xxxxxxxxx";//password

int a = 0; //遮阳电机启动时间
int b;     //浇水电机启动时间

int AT_Mode;
int MT_Mode; //模式
int Mode;

BlinkerNumber HUMI("humi");
BlinkerNumber TEMP("temp");
DHT dht(DHTPIN, DHTTYPE);
float humi_read = 0, temp_read = 0;

BlinkerNumber lx("lx");
int BH1750address = 0x23;
uint16_t val = 0; // bh1750
byte buff[2];

BlinkerNumber soil("soil");
int s;

BlinkerText Text1(TEXTE_1);
void warning()
{
  if (humi_read > 70)
  {
    Text1.print("空气湿度过高");
    Text1.icon("fas fa-exclamation-triangle");
    Text1.color("#FF0000");
  }
  else if (temp_read > 30)
  {
    Text1.print("空气温度过高");
    Text1.icon("fas fa-exclamation-triangle");
    Text1.color("#FF0000");
  }
  else if (s > 70)
  {
    Text1.print("土壤湿度过高");
    Text1.icon("fas fa-exclamation-triangle");
    Text1.color("#FF0000");
  }
  else if (val > 5000)
  {
    Text1.print("暴晒");
    Text1.icon("fas fa-exclamation-triangle");
    Text1.color("#FF0000");
  }
  else
  {
    Text1.print("状态正常");
    Text1.color("#D8D8D8");
    Text1.icon("fas fa-exclamation-triangle");
  }
  Blinker.delay(1000);
}

void heartbeat()
{
  HUMI.print(humi_read);
  TEMP.print(temp_read);
  lx.print(val);
  soil.print(s);
}

void soil_read()
{
  s = analogRead(ADC);
  s = (660 - s) / 3.6;
  BLINKER_LOG(s);
}

void dht_read()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t))
  {
    BLINKER_LOG("Failed to read from DHT sensor!");
  }
  else
  {
    humi_read = h;
    temp_read = t;
  }
}

void LED_ON()
{
  digitalWrite(LED, HIGH);
}

void LED_OFF()
{
  digitalWrite(LED, LOW);
}

void motoA_On()
{
  for (a; a < 400; a++)
  {
    analogWrite(MotoA_1, 500);
    digitalWrite(MotoA_2, LOW);
    Blinker.delay(10);
  }

  digitalWrite(MotoA_1, LOW);
  digitalWrite(MotoA_2, LOW);
}

void motoA_Off()
{
  for (a; a > 0; a--)
  {

    digitalWrite(MotoA_1, LOW);
    analogWrite(MotoA_2, 500);
    Blinker.delay(10);
  }

  digitalWrite(MotoA_1, LOW);
  digitalWrite(MotoA_2, LOW);
}

void motoB_On()
{
  for (b = 100; b > 0; b--)
  {
    analogWrite(MotoB_1, 300);
    digitalWrite(MotoB_2, LOW);
    BLINKER_LOG(b);
    Blinker.delay(10);
  }
  digitalWrite(MotoB_1, LOW);
  digitalWrite(MotoB_2, LOW);
}

int BH1750_Read(int address) //
{
  int l = 0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while (Wire.available()) //
  {
    buff[l] = Wire.read(); // receive one byte
    l++;
  }
  Wire.endTransmission();
  return l;
}

void BH1750_Init(int address)
{
  Wire.beginTransmission(address);
  Wire.write(0x10);
  // 1lx reolution 120ms
  Wire.endTransmission();
}

void BH1750()
{
  // int l;
  // uint16_t val=0;
  BH1750_Init(BH1750address);
  Blinker.delay(1000);
  if (2 == BH1750_Read(BH1750address))
    val = ((buff[0] << 8) | buff[1]) / 1.2;
  BLINKER_LOG("LX:", val);
}

/***********按键***************/
BlinkerButton Button2(BUTTON_2);
void button2_callback(const String &state)
{
  BLINKER_LOG("get button state: ", state);
  // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  motoB_On();
}
BlinkerButton Button3(BUTTON_3);
void button3_callback(const String &state)
{
  // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  BLINKER_LOG("get button state: ", state);
  if (state == BLINKER_CMD_ON)
  {
    BLINKER_LOG("on!");
    Button3.icon("fad fa-blinds-open");
    Button3.color("#A4A4A4");
    // Button1.text("Your button name", "describe");
    Button3.print("on");
    motoA_Off();
  }
  else if (state == BLINKER_CMD_OFF)
  {
    BLINKER_LOG("off!");
    Button3.color("#A4A4A4");
    Button3.icon("fad fa-blinds-raised");
    // Button1.text("Your button name", "describe");
    Button3.print("off");
    motoA_On();
  }
  else
  {
    BLINKER_LOG("Get user setting: ", state);
    Button3.color("#A4A4A4");
    // Button3.text("Your button name or describe");
    // Button1.text("Your button name", "describe");
    Button3.print();
  }
}

BlinkerButton Button4(BUTTON_4);
void button4_callback(const String &state)
{
  // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  BLINKER_LOG("get button state: ", state);
  if (state == BLINKER_CMD_ON)
  {
    BLINKER_LOG("on!");
    // Button4.icon("fad fa-blinds-open");
    Button4.color("#A4A4A4");
    // Button1.text("Your button name", "describe");
    Button4.print("on");
    LED_ON();
  }
  else if (state == BLINKER_CMD_OFF)
  {
    BLINKER_LOG("off!");
    Button4.color("#A4A4A4");
    // Button4.icon("fad fa-blinds-raised");
    // Button1.text("Your button name", "describe");
    Button4.print("off");
    LED_OFF();
  }
  else
  {
    BLINKER_LOG("Get user setting: ", state);
    Button4.color("#A4A4A4");
    // Button3.text("Your button name or describe");
    // Button1.text("Your button name", "describe");
    Button4.print();
  }
}

BlinkerButton Button1(BUTTON_1);
void button1_callback(const String &state)
{
  // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  BLINKER_LOG("get button state: ", state);
  if (state == BLINKER_CMD_ON)
  {
    BLINKER_LOG("AUTO on!");

    Button1.icon("icon_1");
    Button1.color("#FF69B4");
    Button1.text("AUTO");
    // Button1.text("Your button name", "describe");
    Button1.print("on");
    Mode = 1;
  }
  else if (state == BLINKER_CMD_OFF)
  {
    BLINKER_LOG("AUTO off!");

    Button1.icon("icon_1");
    Button1.color("#FFFFFF");
    Button1.text("AUTO");
    // Button1.text("Your button name", "describe");
    Button1.print("off");
    Mode = 0;
  }
  else
  {
    BLINKER_LOG("Get user setting: ", state);

    Button1.icon("icon_1");
    Button1.color("#FFFFFF");
    Button1.text("Your button name or describe");
    // Button1.text("Your button name", "describe");
    Button1.print();
    Mode = 1; //默认自动
  }
}

void dataStorage()
{
  //     Blinker.dataStorage("temp", temp_read);
  //     Blinker.dataStorage("humi", humi_read);
  Blinker.dataStorage("soil", s);
}

void dataRead(const String &data)
{
  BLINKER_LOG("Blinker readString: ", data);

  Blinker.vibrate();

  uint32_t BlinkerTime = millis();

  Blinker.print("millis", BlinkerTime);
}

void Auto_mode() //模式选择
{
  if (Mode == 1)
  {
    if (val > 5000)
    {
      motoA_On();
    }
    else if (val < 3000)
    {
      motoA_Off();
    }
    if (s < 30)
    {
      motoB_On();
    }
  }
  else
  {
  }
}

void setup()
{
  Wire.begin();
  Serial.begin(115200);
  BLINKER_DEBUG.stream(Serial);
  pinMode(ADC, INPUT);
  pinMode(MotoA_1, OUTPUT);
  pinMode(MotoA_2, OUTPUT);
  pinMode(MotoB_1, OUTPUT);
  pinMode(MotoB_2, OUTPUT);
  pinMode(LED, OUTPUT);
  analogWriteFreq(30000); // PWM频率

  Blinker.begin(auth, ssid, pswd);
  Blinker.attachData(dataRead);

  Button1.attach(button1_callback);
  Button2.attach(button2_callback);
  Button3.attach(button3_callback);
  Button4.attach(button4_callback);

  Blinker.attachHeartbeat(heartbeat);
  Blinker.attachDataStorage(dataStorage);
  dht.begin();
  // put your setup code here, to run once:
}

void loop()
{
  Blinker.run();
  // BLINKER_LOG("adc=");
  soil_read();
  dht_read();
  BH1750();
  Auto_mode();
  warning();

  Blinker.delay(2000);
  // put your main code here, to run repeatedly:
}