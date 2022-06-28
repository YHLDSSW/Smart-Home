#define BLINKER_WIFI
#define BLINKER_DUEROS_SENSOR

#include <Wire.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <Blinker.h>
#include <DHT.h>

char auth[] = "xxxxxxxxxx";//key
char ssid[] = "「Elysia」";//SSID
char pswd[] = "xxxxxxxxx";//password

BlinkerNumber HUMI("humi");
BlinkerNumber TEMP("temp");
BlinkerNumber PM25("pm25");
BlinkerButton Button1("btn-abc");
BlinkerText Text1("TextKey");

//显示器驱动
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

float humi_read = 0, temp_read = 0, pm25_read = 0;

int dustPin = 0; //pm模块A0引脚
int ledPower = D0; // pm模块 LED引脚
//计算pm2.5
int delayTime = 280;
int delayTime2 = 40;
float offTime = 9680;
float dustVal = 0;
float calcVoltage = 0;
float dustDensity = 0;
float pm25_data()//PM_data 计算pm2.5的值
{
  digitalWrite(ledPower, LOW); // power on the LED
  delayMicroseconds(delayTime);
  dustVal = analogRead(dustPin); // read the dust value
  delayMicroseconds(delayTime2);
  digitalWrite(ledPower, HIGH); // turn the LED off
  delayMicroseconds(offTime);
  //delay(1000);
  //dustVal>36.455;
  dustVal = analogRead(dustPin); // read the dust value
  calcVoltage = dustVal * (5.0 / 1024.0);
  dustDensity = 12000 * 0.035 * calcVoltage - 0.0356;
  return dustDensity;
}



#define DHTPIN D5

#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

//手动开关
void button1_callback(const String & state) {
    BLINKER_LOG("get button state: ", state);
    digitalWrite(D7, !digitalRead(D7));
    Blinker.vibrate();
}

void heartbeat()//心跳
{
  HUMI.print(humi_read);
  TEMP.print(temp_read);
  PM25.print(pm25_read);
}

void dataStorage()//数据储存
{
  Blinker.dataStorage("temp", temp_read);
  Blinker.dataStorage("humi", humi_read);
  Blinker.dataStorage("pm25", pm25_read);
}

void duerQuery(int32_t queryCode)
{
    BLINKER_LOG("DuerOS Query codes: ", queryCode);
    float humi2_read = humi_read/100; //小度湿度缩小100

    switch (queryCode)
    {
        //case BLINKER_CMD_QUERY_AQI_NUMBER :
        //    BLINKER_LOG("DuerOS Query AQI");
        //    BlinkerDuerOS.aqi(20);
        //    BlinkerDuerOS.print();
        //  break;
        //case BLINKER_CMD_QUERY_CO2_NUMBER :
        //    BLINKER_LOG("DuerOS Query CO2");
        //    BlinkerDuerOS.co2(20);
        //    BlinkerDuerOS.print();
        //    break;
        //case BLINKER_CMD_QUERY_PM10_NUMBER :
        //    BLINKER_LOG("DuerOS Query PM10");
        //    BlinkerDuerOS.pm10(20);
        //    BlinkerDuerOS.print();
        //    break;
        case BLINKER_CMD_QUERY_PM25_NUMBER :
            BLINKER_LOG("DuerOS Query PM25");
            BlinkerDuerOS.pm25(pm25_read);
            BlinkerDuerOS.print();
            break;
        case BLINKER_CMD_QUERY_HUMI_NUMBER :
            BLINKER_LOG("DuerOS Query HUMI");
            BlinkerDuerOS.humi(humi2_read);
            BlinkerDuerOS.print();
            break;
        case BLINKER_CMD_QUERY_TEMP_NUMBER :
            BLINKER_LOG("DuerOS Query TEMP");
            BlinkerDuerOS.temp(temp_read);
            BlinkerDuerOS.print();
            break;
        case BLINKER_CMD_QUERY_TIME_NUMBER :
            BLINKER_LOG("DuerOS Query time");
            BlinkerDuerOS.time(millis());
            BlinkerDuerOS.print();
            break;
        default :
            BlinkerDuerOS.temp(20);
            BlinkerDuerOS.humi(20);
            BlinkerDuerOS.pm25(20);
            //BlinkerDuerOS.pm10(20);
            //BlinkerDuerOS.co2(20);
            //BlinkerDuerOS.aqi(20);
            BlinkerDuerOS.time(millis());
            BlinkerDuerOS.print();
            break;
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
  Serial.begin(115200);
  BLINKER_DEBUG.stream(Serial);
  // BLINKER_DEBUG.debugAll();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  digitalWrite(D7, HIGH);
  digitalWrite(D6, HIGH);  
  pinMode(ledPower, OUTPUT);
  pinMode(dustPin, INPUT);
  //屏幕
  Wire.begin();
  u8g2.begin();
  u8g2.enableUTF8Print(); 
  
  Button1.attach(button1_callback);
  Blinker.begin(auth, ssid, pswd);
  Blinker.attachHeartbeat(heartbeat);
  Blinker.attachDataStorage(dataStorage);
  Blinker.attachData(dataRead);
  BlinkerDuerOS.attachQuery(duerQuery);
  dht.begin();
}


void loop()
{
  Blinker.run();
  pm25_read = pm25_data();
  

  float h = dht.readHumidity();
  float t = dht.readTemperature();


 
  if (isnan(h) || isnan(t))
  {
    BLINKER_LOG("Failed to read from DHT sensor!");
  }
  else
  {
    BLINKER_LOG("Humidity: ", h, " %");
    BLINKER_LOG("Temperature: ", t, " *C");
    humi_read = h;
    temp_read = t;

    float hic = dht.computeHeatIndex(t,h, false);
    u8g2.clearBuffer();                 //屏幕初始化
    u8g2.setFont(u8g2_font_wqy16_t_gb2312);   //字体选择u8g2_font_pingfang/u8g2_font_unifont_t_chinese1
    u8g2.setCursor(0, 20);
    u8g2.print("温度: " + String(t) + "℃"); //输出空气温度
    u8g2.setCursor(0, 40);
    u8g2.print("湿度: " + String(h) + "％"); //输出空气温度
     u8g2.setCursor(0, 60);
    u8g2.print("PM2.5: " + String(pm25_read)+ "ug/m3");
    u8g2.sendBuffer();                   //提交到屏幕

    
  }
//自动开启
    if (pm25_read >= 200) {
    Text1.print("新风开启");
    digitalWrite(D6,LOW);
  }
  Text1.print("正常");
  Blinker.delay(2000);
}