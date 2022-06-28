#include <Servo.h>
#include <MFRC522.h>
#include <SPI.h>
#define BLINKER_PRINT Serial
#define BLINKER_WIFI
#include <Blinker.h>
char auth[] = "xxxxxxxxxx";//key
char ssid[] = "「Elysia」";//SSID
char pswd[] = "xxxxxxxxx";//password

//定义接口

//  SPI_MOSI D7
//  SPI_MISO D6
//  SPI_SCK D5
#define RST_PIN D1
#define SS_PIN D2
#define RAIN_PIN D4
#define SERVO_PIN D0 //舵机端口
#define SERVO_PIN2 D8
#define BUZZ_PIN D3
#define CARDS 2 //已验证的卡片数目
//手动开关

BlinkerButton Button1("btn-max");
BlinkerButton Button2("btn-win");
BlinkerButton Button3("btn-auto");

Servo myservo;
Servo myservo1;
int servo_max = 180;
int servo_min = 90;
int a; //模式

void button3_callback(const String &state)
{
  // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  BLINKER_LOG("get button state: ", state);
  if (state == BLINKER_CMD_ON)
  {
    BLINKER_LOG("on!");
    // Button3.icon("fad fa-blinds-open");
    Button3.color("#A4A4A4");
    // Button1.text("Your button name", "describe");
    Button3.print("on");
    a = 0;
  }
  else if (state == BLINKER_CMD_OFF)
  {
    BLINKER_LOG("off!");
    Button3.color("#A4A4A4");
    // Button3.icon("fad fa-blinds-raised");
    // Button1.text("Your button name", "describe");
    Button3.print("off");
    a = 1;
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

void button1_callback(const String &state)
{
  BLINKER_LOG("get button state: ", servo_max);
  myservo.write(servo_max);
  Blinker.vibrate();
  Blinker.delay(3000);
  myservo.write(servo_min);
}

// Servo开关门
void OpenDoor()
{
  myservo.write(180);
}

void CloseDoor()
{
  myservo.write(0);
}

void OpenWindow()
{
  myservo1.write(180);
}

void CloseWindow()
{
  myservo1.write(0);
}

void button2_callback(const String &state)
{
  // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  BLINKER_LOG("get button state: ", state);
  if (state == BLINKER_CMD_ON)
  {
    BLINKER_LOG("on!");
    Button2.icon("fad fa-blinds-open");
    Button2.color("#A4A4A4");
    // Button1.text("Your button name", "describe");
    Button2.print("on");
    if (a == 1)
    {
      OpenWindow();
    }
  }
  else if (state == BLINKER_CMD_OFF)
  {
    BLINKER_LOG("off!");
    Button2.color("#A4A4A4");
    Button2.icon("fad fa-blinds-raised");
    // Button1.text("Your button name", "describe");
    Button2.print("off");
    if (a == 1)
    {
      CloseWindow();
    }
  }
  else
  {
    BLINKER_LOG("Get user setting: ", state);
    Button2.color("#A4A4A4");
    // Button3.text("Your button name or describe");
    // Button1.text("Your button name", "describe");
    Button2.print();
  }
}

uint32_t detect_os_time = millis();
bool isWarn = false;

void sensor_init()
{
  pinMode(RAIN_PIN, INPUT_PULLUP);
}

void BeepChecked()
{
  digitalWrite(BUZZ_PIN, LOW);
  Blinker.delay(100);
  digitalWrite(BUZZ_PIN, HIGH);
  Blinker.delay(200);
  digitalWrite(BUZZ_PIN, LOW);
  Blinker.delay(100);
  digitalWrite(BUZZ_PIN, HIGH);
}

void BeepFailed()
{
  digitalWrite(BUZZ_PIN, LOW);
  Blinker.delay(1000);
  digitalWrite(BUZZ_PIN, HIGH);
}

MFRC522 mfrc522(SS_PIN, RST_PIN); //新建RC522对象

bool isAuthed = false;                                                                //验证是否通过
const byte AuthedID[CARDS][4] = {{0x08, 0x0B, 0xBD, 0xA4}, {0x23, 0xD2, 0x8C, 0xA3}}; //卡片UID值

void setup()
{
  Serial.begin(115200);
  SPI.begin(); //初始化SPI总线
               // 初始化blinker
  Blinker.begin(auth, ssid, pswd);
  Button1.attach(button1_callback);
  Button2.attach(button2_callback);
  Button3.attach(button3_callback);
  sensor_init();

  myservo.attach(SERVO_PIN); //连接舵机
  myservo1.attach(SERVO_PIN2);

  Blinker.delay(100);
  delay(100);
  mfrc522.PCD_Init();        //初始化MFRC522卡
  pinMode(BUZZ_PIN, OUTPUT); //初始化蜂鸣器
  digitalWrite(BUZZ_PIN, HIGH);
  myservo.write(45);
  // Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

//验证卡片
void Authenticate()
{
  //检测是否有新卡片
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
  {
    Blinker.delay(100);
    return;
  }

  //进行卡片验证
  for (byte num = 0; num < CARDS; num++) //卡片循环
  {
    byte i;
    for (i = 0; i < mfrc522.uid.size; i++)
    {
      if (mfrc522.uid.uidByte[i] != AuthedID[num][i])
        break;
    }
    if (i == mfrc522.uid.size)
    {
      isAuthed = true; //验证通过
      break;
    }
    else if (num == CARDS - 1)
    {
      BeepFailed();
    }
  }
}

void sensor_detect()
{
  if (a == 0)
  {
    /* code */

    if (!digitalRead(RAIN_PIN))
    {
      if (!isWarn)
      {
        Blinker.notify("下雨啦");
        isWarn = true;
        detect_os_time = millis();
      }
      else if (millis() - detect_os_time >= 10000)
      {
        isWarn = false;
      }
      CloseWindow();
    }
    else
    {
      isWarn = false;
      OpenWindow();
    }
  }
}

void loop()
{
  Blinker.run();
  sensor_detect();
  Authenticate();
  if (isAuthed)
  {
    OpenDoor();
    BeepChecked();
    Blinker.delay(3000);
    isAuthed = false;
  }
  CloseDoor();
  isAuthed = false;
  Blinker.delay(1000);
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}
