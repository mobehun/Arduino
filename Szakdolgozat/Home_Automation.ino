#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <Stepper.h>
#include <MCUFRIEND_kbv.h>
#include "Adafruit_GFX.h"
#include <TouchScreen.h>
#include "Pictures.h" //Holds my arts

#define ONE_WIRE_BUS 30
#define LDR A8

MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(6, A1, A2, 7, 414); //(XP,YP,XM,YM,Resistance between XP-XM)
TSPoint tp;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
RTC_DS1307 rtc;
Stepper myStepper(200,22,23,24,25); //H-Bridge 2,7,10,15 pins
Stepper myOtherStepper(200,26,27,28,29);

extern const unsigned char cloud[];
extern const unsigned char moon[];
extern const unsigned char sun[];
extern const unsigned char sun_low[];
extern const unsigned char therm0[];
extern const unsigned char therm25[];
extern const unsigned char therm50[];
extern const unsigned char therm75[];
extern const unsigned char therm100[];
extern const unsigned char moon_FQ[];
extern const unsigned char moon_F[];
extern const unsigned char moon_LQ[];
extern const unsigned char moon_N[];
extern const unsigned char moon_WC[];
extern const unsigned char moon_WG[];
extern const unsigned char moon_WaC[];
extern const unsigned char moon_WaG[];

String nextNewM = "";
const char* newNextNewMoon=(const char*) nextNewM.c_str();

void drawMoon(){
  DateTime now=rtc.now();
  int moonPhase=moonphase();
  switch (moonPhase){
    case 0: tft.drawBitmap(0, 233,  moon_F, 26, 26, 0xFFFF); break;
    case 1: tft.drawBitmap(0, 233,  moon_WaG, 26, 26, 0xFFFF); break;
    case 2: tft.drawBitmap(0, 233,  moon_LQ, 26, 26, 0xFFFF); break;
    case 3: tft.drawBitmap(0, 233,  moon_WaC, 26, 26, 0xFFFF); break;
    case 4: tft.drawBitmap(0, 233,  moon_N, 26, 26, 0xFFFF); break;
    case 5: tft.drawBitmap(0, 233,  moon_WaxC, 26, 26, 0xFFFF); break;
    case 6: tft.drawBitmap(0, 233,  moon_FQ, 26, 26, 0xFFFF); break;
    case 7: tft.drawBitmap(0, 233,  moon_WaxG, 26, 26, 0xFFFF); break;
  }
}

int moonphase(){
  DateTime now=rtc.now();
  double julianD=0;
  double elapsedD=0;
  int b=0;
  julianD=julian(now.year(),now.month(),now.day());
  julianD=int(julianD-2244116.75);
  julianD/=29.53;
  b=julianD;
  julianD-=b;
  elapsedD=julianD*29.53;
  nextNewM=String((int(29.53-elapsedD)));
  b=julianD*8+0.5;
  b=b&7;
  return b;
}

double julian(int year, int month, int day){
  int Month, Year;
  double a,b,c;
  double j;
  Year=year-int((12-month)/10);
  Month=month+9;
  if(Month>12) Month-=12;
  a=365.25*(Year+4172);
  b=int((30.6001*Month)+0.5);
  c=int((((Year/100)+4)*0.75)-38);
  j=a+b+day+59;
  j-=c;
  return j;
}

void dateWrite(){
  String months[12]={"January","February","March","April","May","June","July","August","September","October","November","December"};
  tft.fillScreen(0x0000);
  tft.setTextSize(2);

  DateTime ido = rtc.now();
  Serial.println(ido.month());
  if(ido.month()>=4 && ido.month()<=9){
    if(ido.hour()>=7 && ido.hour()<=19){
      if(analogRead(LDR)>=200) tft.drawBitmap(0, 0,  sun, 64, 64, 0xFFFF); //(x,y, name, DX, DY, color)
      else tft.drawBitmap(0, 0,  cloud, 64, 64, 0xFFFF); //(x,y, name, DX, DY, color)
    }
    else tft.drawBitmap(0, 0,  moon, 64, 64, 0xFFFF); //(x,y, name, DX, DY, color)
  }
  else{
    if(ido.hour()>=8 && ido.hour()<=16){
      if(analogRead(LDR)>=200) tft.drawBitmap(0, 0,  sun, 64, 64, 0xFFFF); //(x,y, name, DX, DY, color)
      else tft.drawBitmap(0, 0,  cloud, 64, 64, 0xFFFF); //(x,y, name, DX, DY, color)
    }
    else tft.drawBitmap(0, 0,  moon, 64, 64, 0xFFFF); //(x,y, name, DX, DY, color)
  }

  drawMoon();

  String cTime;
  switch ((int)ido.month()){
    case 1: cTime=(String)ido.year()+'/'+months[0]+'/'+(String)ido.day(); break;
    case 2: cTime=(String)ido.year()+'/'+months[1]+'/'+(String)ido.day(); break;
    case 3: cTime=(String)ido.year()+'/'+months[2]+'/'+(String)ido.day(); break;
    case 4: cTime=(String)ido.year()+'/'+months[3]+'/'+(String)ido.day(); break;
    case 5: cTime=(String)ido.year()+'/'+months[4]+'/'+(String)ido.day(); break;
    case 6: cTime=(String)ido.year()+'/'+months[5]+'/'+(String)ido.day(); break;
    case 7: cTime=(String)ido.year()+'/'+months[6]+'/'+(String)ido.day(); break;
    case 8: cTime=(String)ido.year()+'/'+months[7]+'/'+(String)ido.day(); break;
    case 9: cTime=(String)ido.year()+'/'+months[8]+'/'+(String)ido.day(); break;
    case 10: cTime=(String)ido.year()+'/'+months[9]+'/'+(String)ido.day(); break;
    case 11: cTime=(String)ido.year()+'/'+months[10]+'/'+(String)ido.day(); break;
    case 12: cTime=(String)ido.year()+'/'+months[11]+'/'+(String)ido.day(); break;
  }
  tft.setCursor(100,105);
  tft.println("Today is:");
  tft.setCursor(60,125);
  tft.print(cTime);
  tft.setCursor(4,225);
  tft.print("Back");
  int Min=ido.minute();
  int Sec=ido.second();
  bool val=true;
  while(val){
    ido=rtc.now();
    if(ido.minute()>Min && (ido.second()+60)-Sec>10) val=false;
    else if(ido.second()-Sec>10) val=false;
    tp=ts.getPoint();
    pinMode(A2, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    if(tp.z<1000 && tp.z>20){
      if(tp.x>820 && tp.x<890){
        if(tp.y>780 && tp.y<920){
          Serial.println("Back hit!");
          loop();
        }
      }
    }
  }
  dateWrite();
}

void tempWrite(){
  tft.fillScreen(0x0000);
  tft.setTextSize(2);
  sensors.requestTemperatures(); // Send the command to get temperatures

  if(int(sensors.getTempCByIndex(0))<-10) tft.drawBitmap(0, 233,  therm0, 26, 26, 0xFFFF);
  if(int(sensors.getTempCByIndex(0))>=-10 && int(sensors.getTempCByIndex(0))<-5) tft.drawBitmap(0, 233,  therm25, 26, 26, 0xFFFF);
  if(int(sensors.getTempCByIndex(0))>=-5 && int(sensors.getTempCByIndex(0))<10) tft.drawBitmap(0, 233,  therm50, 26, 26, 0xFFFF);
  if(int(sensors.getTempCByIndex(0))>=10 && int(sensors.getTempCByIndex(0))<30) tft.drawBitmap(0, 233,  therm75, 26, 26, 0xFFFF);
  if(int(sensors.getTempCByIndex(0))>=30) tft.drawBitmap(0, 233,  therm100, 26, 26, 0xFFFF);

  tft.setCursor(85,85);
  tft.print("Outside: ");
  float temp=sensors.getTempCByIndex(0);
  if(temp-(int)temp<0.5) tft.print((int)temp);
  else tft.print((int)temp+1);
  //drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
  tft.print(" C");

  tft.setCursor(85,105);
  tft.print("Inside: "); //Average of inside temperature
  temp = ((sensors.getTempCByIndex(1)+sensors.getTempCByIndex(2))/2);
  if(temp-(int)temp<0.5) tft.print((int)temp);
  else tft.print((int)temp+1);
  //drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
  tft.print(" C");

  int sensorReading=analogRead(LDR); // LDR data pin connected to A0
  String out="Light level "+(String)sensorReading+" lx";
  tft.setCursor(60,125);
  tft.print(out);
  tft.setCursor(4,225);
  tft.print("Back");

  DateTime ido=rtc.now();
  int Min=ido.minute();
  int Sec=ido.second();
  bool val=true;
  while(val){
    ido=rtc.now();
    if(ido.minute()>Min && (ido.second()+60)-Sec>10) val=false;
    else if(ido.second()-Sec>10) val=false;
    tp=ts.getPoint();
    pinMode(A2, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    if(tp.z<1000 && tp.z>20){
      if(tp.x>820 && tp.x<890){
        if(tp.y>780 && tp.y<920){
          Serial.println("Back hit!");
          loop();
        }
      }
    }
  }
  tempWrite();
}

void plusTurn(int turns){
  myStepper.setSpeed(100);
  myStepper.step(200/100*turns);
}

void minusTurn(int turns){
  myStepper.setSpeed(100);
  myStepper.step(-200/100*turns);
}

void otherPlusTurn(int turns){
  myOtherStepper.setSpeed(100);
  myOtherStepper.step(200/100*turns);
}

void otherMinusTurn(int turns){
  myOtherStepper.setSpeed(100);
  myOtherStepper.step(-200/100*turns);
}

void setBlinders(){ 
  tft.fillScreen(0x0000);
  DateTime ido = rtc.now();
  int sensorReading=analogRead(LDR);   // LDR data pin connected to A0
  Serial.print("Light level: ");
  Serial.println(sensorReading);

  tft.setTextSize(2);
  String out="Light level: "+(String)sensorReading+" lx";
  tft.setCursor(40,80);
  tft.print(out);
  tft.setTextSize(3);
  tft.setCursor(90,155);
  tft.print("+");
  tft.setCursor(210,155);
  tft.print("-");
  tft.setTextSize(2);
  tft.setCursor(4,225);
  tft.print("Back");
  
  int Min=ido.minute();
  int Sec=ido.second();
  bool val=true;
  while(val){
    ido=rtc.now();
    if(ido.minute()>Min && (ido.second()+60)-Sec>10) val=false;
    else if(ido.second()-Sec>10) val=false;
    tp=ts.getPoint();
    pinMode(A2, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    if(tp.z<1000 && tp.z>20){
      if(tp.x>660 && tp.x<710){
        if(tp.y>720 && tp.y<730){
          Serial.println("Plus sign hit");
          plusTurn(10);
        }
      }
      if(tp.x>660 && tp.x<710){
        if(tp.y>390 && tp.y<445){
          Serial.println("Minus sign hit");
          minusTurn(10);
        }
      }
      if(tp.x>820 && tp.x<890){
        if(tp.y>780 && tp.y<920){
          Serial.println("Back hit!");
          loop();
        }
      }
    }
  }
  setBlinders();
}

void setHeaters(){
  tft.fillScreen(0x0000);
  sensors.requestTemperatures();
  tft.setCursor(50,80);
  tft.setTextSize(2);
  tft.print("Inside temperature: ");
  float temp = ((sensors.getTempCByIndex(1)+sensors.getTempCByIndex(2))/2);
  tft.setCursor(135,100);
  if(temp-(int)temp<0.5) tft.print((int)temp);
  else tft.print((int)temp+1);
  tft.print(" C");
  tft.setTextSize(3);
  tft.setCursor(90,155);
  tft.print("+");
  tft.setCursor(210,155);
  tft.print("-");
  tft.setTextSize(2);
  tft.setCursor(4,225);
  tft.print("Back");
  
  DateTime ido=rtc.now();
  int Min=ido.minute();
  int Sec=ido.second();
  bool val=true;
  while(val){
    ido=rtc.now();
    if(ido.minute()>Min && (ido.second()+60)-Sec>10) val=false;
    else if(ido.second()-Sec>10) val=false;
    tp=ts.getPoint();
    pinMode(A2, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    if(tp.z<1000 && tp.z>20){
      
      if(tp.x>660 && tp.x<710){
        if(tp.y>720 && tp.y<730){
          Serial.println("Plus sign hit");
          otherPlusTurn(10);
        }
      }
      if(tp.x>660 && tp.x<710){
        if(tp.y>390 && tp.y<445){
          Serial.println("Minus sign hit");
          otherMinusTurn(10);
        }
      }
      if(tp.x>820 && tp.x<890){
        if(tp.y>780 && tp.y<920){
          Serial.println("Back hit!");
          loop();
        }
      }
    }
  }
  setHeaters();
}

void setup() {
  Serial.begin(9600);
  rtc.begin();
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//Needs to run only one time!
  
  tft.begin(tft.readID());
  tft.setRotation(1);
}

void loop() {
  tft.invertDisplay(true);
  tft.fillScreen(0x0000);
  DateTime ido = rtc.now();
  tft.setTextSize(2);
  tft.setCursor(45, 45);
  tft.print("Date");
  tft.setCursor(180,45);
  tft.print("Temperature");
  tft.setCursor(45,130);
  tft.print("Blinders");
  tft.setCursor(180,130);
  tft.print("Heating");
  tft.setCursor(180,210);

  String cTime;
  if(ido.minute()<10){
    String minute="0"+(String)ido.minute();
    cTime=(String)ido.hour()+":"+minute;
  }
  else cTime=(String)ido.hour()+":"+(String)ido.minute();
  tft.print(cTime);

  int Min=ido.minute();
  int Sec=ido.second();
  bool val=true;
  while(val){
    ido=rtc.now();
    if(ido.minute()>Min && (ido.second()+60)-Sec>10) val=false;
    else if(ido.second()-Sec>10) val=false;
    tp=ts.getPoint();
    pinMode(A2, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    if(tp.z<1000 && tp.z>20){
      if(tp.x>240 && tp.x<330){
        if(tp.y>700 && tp.y<850){
          Serial.println("Date hit!");
          dateWrite();
        }
      }
      if(tp.x>240 && tp.x<330){
        if(tp.y>205 && tp.y<495){
          Serial.println("Temperature hit!");
          tempWrite();
        }
      }
      if(tp.x>560 && tp.x<590){
        if(tp.y>615 && tp.y<850){
          Serial.println("Blinders hit!");
          setBlinders();
        }
      }
      if(tp.x>560 && tp.x<590){
        if(tp.y>275 && tp.y<510){
          Serial.println("Heating hit!");
          setHeaters();
        }
      }
    }
  }
  delay(2000);
}
