//Bot2Coin V.1.0.2
//D0  16  
//D1      lcd scl
//D2      lcd
//D3      sonic trig
//D4      sonic
//D5      servo1
//D6      servo2
//D7  13  sw2
//D8  15  
//D9  3   sw1
//D10     -
//D11 9   -
//D12 10  -
//A0      -
#include <Servo.h>
#include <Wire.h>
#include <TridentTD_LineNotify.h>
#include <LiquidCrystal_I2C.h>
#include <MicroGear.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);//ถ้าจอไม่แสดงผล ให้ลองเปลี่ยน 0x3F

#define SSID        "Mi6Odto"
#define PASSWORD    "0969982988"
#define LINE_TOKEN  "9z1n5SETrWsnYlPBYAwlYSh5I5YKqS4FUtmBwFs36Vc"

#define APPID   "ESPDot"
#define KEY     "xgolZoMTFA8N4yl"
#define SECRET  "C5YiRPnhYsv3y11fkKAHs2U3c" 
#define ALIAS   "Distance"

const char* ssid     = "Mi6Odto";
const char* password = "0969982988";


Servo servo_b;//เซอร์โวปัดขวด
Servo servo_c;//เซอร์โวปัดเหรียญ

WiFiClient client;

int timer = 0;
char str[32];

const int SoundOutPin = D3; //ขา Trig
int SoundInPin = D4; // ขา Echo
const int sw1pin = D9; //switch เริ่มใส่ขวด สีเหลือง
const int sw2pin = D7; //switch ใส่ขวดเสร็จ สีแดง

int valsw1, valsw2;
int buy; //กำหนดจำนวนขวดและฝาเพื่อแลก 1 เหรียญ
int coin_balance; //เหรียญคงเหลือบนคลาวนด์ตอนเริ่มโปรแกรม

int servo_coin;//จำนวนปัดแจกเหรียญ
int bottle_limit;//กำหนดลิมิตขวดที่รับได้
int sb=35;
int sc=135;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    Serial.print("Incoming message -->");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    microgear.setAlias(ALIAS);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  microgear.on(MESSAGE,onMsghandler);
  microgear.on(CONNECTED,onConnected);
  Serial.println("Starting...");
      if (WiFi.begin(ssid, password)) {
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
                                              }
                                      }

    Serial.println("WiFi connected");  
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());

   microgear.init(KEY,SECRET,ALIAS);
   microgear.connect(APPID);


  Serial.println();
  Serial.println(LINE.getVersion());
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while(WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(400); }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());   
  LINE.setToken(LINE_TOKEN);

  coin_balance = 50;//กำหนดเหรียญที่มี 
  bottle_limit = 20; //กำหนดลิมิตขวดที่รับได้
  buy = 3;//กำหนดจำนวนขวดต่อ 1 เหรียญ
  servo_b.attach(D5);      // ระบุการต่อ servo ปล่อยขวด ที่ขา D3
  servo_b.write(sb);
  servo_c.attach(D6);    //ระบุ servo ปัดเหรียญ ขา D4
  servo_c.write(sc);
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(4,0); // ไปที่ตัวอักษรที่ 4 บรรทัดที่ 0
  lcd.print("Hello SKP");
  lcd.setCursor(3,1); // ไปตัวอักษรที่ 2 บรรทัดที่ 1
  lcd.print("Bottle2Coin");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BotLimit: ");
  lcd.setCursor(10,0);
  lcd.print(bottle_limit);
  delay(3000); 
  
  pinMode(sw1pin, INPUT);
  pinMode(sw2pin, INPUT);
  pinMode(D0,INPUT);

}

void loop() {
   if (microgear.connected()) {
        Serial.println("connected");
        microgear.loop();

  if (timer >= 1000) {
   sprintf(str,"%d,%d",bottle_limit,coin_balance);
   Serial.println("Sending -->");
    microgear.publish("/Bottle",str);
  servo_c.write(sc);
  valsw1 = digitalRead(sw1pin);
  delay(100);
  Serial.println("sw1");
  Serial.println(valsw1);
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Press Yellow");
  lcd.setCursor(4,1);
  lcd.print("To Start");

  if(valsw1==0)// กด SW1 เพื่อเริ่ม
  {
    int c_bc = 0;
    int c_b = 0;
    int sumb = 0;
    int ucoin = 0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("StartBotCoin");
    delay(2000);
    lcd.clear();
    
    
    valsw2=digitalRead(sw2pin);
    delay(200);
    Serial.println("valsw2");
    Serial.println(valsw2);
    
    while(valsw2==1)
    {
      //Ultrasonic Part
      long duration, cm;
      pinMode(SoundOutPin, OUTPUT);
      digitalWrite(SoundOutPin, LOW);
      delayMicroseconds(2);
      digitalWrite(SoundOutPin, HIGH);
      delayMicroseconds(5);
      digitalWrite(SoundOutPin, LOW);
      pinMode(SoundInPin, INPUT);
      duration = pulseIn(SoundInPin, HIGH);
      cm = microsecondsToCentimeters(duration);
      Serial.print("cm: ");
      Serial.println(cm);
     
      
      delay(200);
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Bottle: ");
      lcd.setCursor(8,1);
      lcd.print(c_b);
    
      servo_b.write(sb);
   
      
      valsw2=digitalRead(sw2pin);
      delay(100);
      if((valsw2==0)||(bottle_limit<=1)){ // กด sw2 สีแดง ออกจากลูป
        break;
       }

      
      if((cm<=5) && (cm>1) )
        { //นับขวด
        //Ultrasonic Part       
        pinMode(SoundOutPin, OUTPUT);
        digitalWrite(SoundOutPin, LOW);
        delayMicroseconds(2);
        digitalWrite(SoundOutPin, HIGH);
        delayMicroseconds(5);
        digitalWrite(SoundOutPin, LOW);
        pinMode(SoundInPin, INPUT);
        duration = pulseIn(SoundInPin, HIGH);
        cm = microsecondsToCentimeters(duration);
        Serial.print("cmif: ");
        Serial.println(cm);
        servo_b.write(90);
        delay(1500);
          Serial.print("Bottle");
          c_b+=1;
          bottle_limit-=1;
          sprintf(str,"%d,%d",bottle_limit,coin_balance);
          Serial.println(c_b);
          servo_b.write(sb);
        
       
        }
      else
      {
        c_b+=0;
        servo_b.write(35);
      }
        
        
     }
      //เมื่อ sw2pin ถูกกด
      //หาจำนวนขวด
     // bottle_limit=bottle_limit - c_b;//หาลิมิตที่สามารรับขวดได้คงเหลือ 
                               // ส่งข้อมูลที่สามารรับขวดได้ขึ้นคลาวนด์
      sumb = c_b/buy;
    
      servo_coin = sumb;//จำนวนปัดเหรียญ

      
      while((servo_coin>0))
      {
        if(bottle_limit<=1)
        {
        Serial.println("full");
        LINE.notify("ขวดใกล้เต็มกรุณานำขวดออก");
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("Get:");
        lcd.setCursor(5,1);
        lcd.print(sumb);
        servo_c.write(140); //ปัดจากมุม 90 ไป 135
        delay(1000);
        servo_c.write(110); //ปัดคืน
        delay(1000);
        servo_c.write(140); //ปัดจากมุม 90 ไป 135
        delay(1000);
        servo_coin--;
        }
        Serial.println("coin");
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("Get:");
        lcd.setCursor(5,1);
        lcd.print(sumb);
        servo_c.write(140); //ปัดจากมุม 90 ไป 135
        delay(1000);
        servo_c.write(110); //ปัดคืน
        delay(1000);
        servo_c.write(140); //ปัดจากมุม 90 ไป 135
        delay(1000);
        servo_coin--;
        
        }
      //ส่งข้อมูลไปลบเหรียญในคลาวนด์
      coin_balance=coin_balance-sumb;
      sprintf(str,"%d,%d",bottle_limit,coin_balance);
      Serial.println("FN");
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("Thank You");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Get:");
      lcd.setCursor(6,0);
      lcd.print(sumb);
   

    
    
    }
    Serial.println("Sending -->");
    microgear.publish("/Bottle",str);

      timer = 0;
  }//time1000
  else timer+=100;
   }//microgear
   else {
    Serial.println("connection lost, reconnect...");
            if (timer >= 5000) {
                microgear.connect(APPID);
                timer = 0;
            }
            else timer += 100;
        }
        delay(100);
  
}//loop

long microsecondsToCentimeters(long microseconds)
{
  // ความเร็วเสียงในอากาศประมาณ 340 เมตร/วินาที หรือ 29 ไมโครวินาที/เซนติเมตร
  // ระยะทางที่ส่งเสียงออกไปจนเสียงสะท้อนกลับมาสามารถใช้หาระยะทางของวัตถุได้
  // เวลาที่ใช้คือ ระยะทางไปกลับ ดังนั้นระยะทางคือ ครึ่งหนึ่งของที่วัดได้
  return microseconds / 29 / 2;
}
