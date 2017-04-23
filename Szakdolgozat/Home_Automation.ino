#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <Stepper.h>
#include <MCUFRIEND_kbv.h>
#include "Adafruit_GFX.h"
#include <TouchScreen.h>

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

void dateWrite(){
  tft.fillScreen(0x0000);
  tft.setTextSize(2);
  DateTime ido = rtc.now();
  String cTime="Today is:"+(String)ido.year()+'/'+(String)ido.month()+'/'+(String)ido.day();
  tft.setCursor(40,105);
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
  delay(10000);
  dateWrite();
}

void tempWrite(){
  tft.fillScreen(0x0000);
  tft.setTextSize(2);
  sensors.requestTemperatures(); // Send the command to get temperatures
  tft.setCursor(85,85);
  tft.print("Outside: ");
  float temp=sensors.getTempCByIndex(0);
  if(temp-(int)temp<0.5) tft.print((int)temp);
  else tft.print((int)temp+1);
  tft.print(" C");

  tft.setCursor(85,105);
  tft.print("Inside: "); //Average of inside temperature
  temp = ((sensors.getTempCByIndex(1)+sensors.getTempCByIndex(2))/2);
  if(temp-(int)temp<0.5) tft.print((int)temp);
  else tft.print((int)temp+1);
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
  delay(10000);
  tempWrite();
}

void plusTurn(int turns){
  myStepper.setSpeed(100);
  myStepper.step(stepsPerRevolution/100*turns);   //Step 1/100 of a revolution
}

void minusTurn(int turns){
  myStepper.setSpeed(100);
  myStepper.step(-stepsPerRevolution/100*turns);   //Step 1/100 of a revolution
}

void otherPlusTurn(int turns){
  myOtherStepper.setSpeed(100);
  myOtherStepper.step(stepsPerRevolution/100*turns);   //Step 1/100 of a revolution
}

void otherMinusTurn(int turns){
  myOtherStepper.setSpeed(100);
  myOtherStepper.step(-stepsPerRevolution/100*turns);   //Step 1/100 of a revolution
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
  tft.setCursor(90,155);
  tft.print("+");
  tft.setCursor(210,155);
  tft.print("-");
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
  delay(10000);
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
  tft.setCursor(90,155);
  tft.print("+");
  tft.setCursor(210,155);
  tft.print("-");
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
  delay(10000);
  setHeaters();
}

void setup() {
  Serial.begin(9600);
  sensors.begin(); // Print a message to the LCD.
  rtc.begin();
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//Needs to run only one time!
  
  tft.begin(tft.readID());
  tft.setRotation(1);
}

void loop() {
  tft.fillScreen(0x0000);
  DateTime ido = rtc.now();
  String cTime=(String)ido.hour()+":"+(String)ido.minute();
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
