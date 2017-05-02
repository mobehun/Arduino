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

#define ONE_WIRE_BUS 23
#define LDR A8

MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(6, A1, A2, 7, 414); //(XP,YP,XM,YM,Resistance between XP-XM)
TSPoint tp;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
RTC_DS1307 rtc;
Stepper myStepper(200,25,27,29,31); //H-Bridge 2,7,10,15 pins
Stepper myOtherStepper(200,33,35,37,39);

extern const unsigned char PROGMEM cloud[];
extern const unsigned char PROGMEM moon[];
extern const unsigned char PROGMEM sun[];
extern const unsigned char PROGMEM sunLow[];
extern const unsigned char PROGMEM thermometerZero[];
extern const unsigned char PROGMEM thermometer25[];
extern const unsigned char PROGMEM thermometer50[];
extern const unsigned char PROGMEM thermometer75[];
extern const unsigned char PROGMEM thermometer100[];
extern const unsigned char PROGMEM moonFirstQuarter[];
extern const unsigned char PROGMEM moonFull[];
extern const unsigned char PROGMEM moonLastQuarter[];
extern const unsigned char PROGMEM moonNew[];
extern const unsigned char PROGMEM moonWaningCrescent[];
extern const unsigned char PROGMEM moonWaningGibbous[];
extern const unsigned char PROGMEM moonWaxingCrescent[];
extern const unsigned char PROGMEM moonWaxingGibbous[];

String nextNewM = "";
const char* newNextNewMoon=(const char*) nextNewM.c_str();

void drawMoon(){
  DateTime now=rtc.now();
  int moonPhase=moonphase();
  switch (moonPhase){
    case 0: tft.drawBitmap(293, 0,  moonFull, 24, 24, 0xFFFF); break;
    case 1: tft.drawBitmap(293, 0,  moonWaxingGibbous, 24, 24, 0xFFFF); break;
    case 2: tft.drawBitmap(293, 0,  moonLastQuarter, 24, 24, 0xFFFF); break;
    case 3: tft.drawBitmap(293, 0,  moonWaxingCrescent, 26, 26, 0xFFFF); break;
    case 4: tft.drawBitmap(293, 0,  moonNew, 24, 24, 0xFFFF); break;
    case 5: tft.drawBitmap(293, 0,  moonWaningCrescent, 24, 24, 0xFFFF); break;
    case 6: tft.drawBitmap(293, 0,  moonFirstQuarter,24, 24, 0xFFFF); break;
    case 7: tft.drawBitmap(293, 0,  moonWaningGibbous, 24, 24, 0xFFFF); break;
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
  /* Need to rethink this part...
  switch((int)ido.month()){
  	case 1:{
  		if((ido.hour()>=6 && ido.minute()>40)&&(ido.hour()<7)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=16)&&(ido.hour()>=16 && ido.minute()<45)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if(ido.hour()>=7 && ido.hour()<16){
      		if(analogRead(LDR)>=200) tft.drawBitmap(0, 0,  sun, 48, 48, 0xFFFF); //(x,y, name, DX, DY, color)
      		else tft.drawBitmap(0, 0,  cloud, 47, 32, 0xFFFF); //(x,y, name, DX, DY, color)
    	}
    	else tft.drawBitmap(0, 0,  moon, 24, 24, 0xFFFF); //(x,y, name, DX, DY, color)
  		break;
  		}
  	case 2:{
  		if(ido.hour()>=6 && (ido.hour()>=6 && ido.minute()<30)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if(ido.hour()>=17 && (ido.hour()>=17 && ido.minute()<30)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=6 && ido.minute()>=30) && ido.hour()<17){
      		if(analogRead(LDR)>=200) tft.drawBitmap(0, 0,  sun, 48, 48, 0xFFFF); //(x,y, name, DX, DY, color)
      		else tft.drawBitmap(0, 0,  cloud, 47, 32, 0xFFFF); //(x,y, name, DX, DY, color)
    	}
    	else tft.drawBitmap(0, 0,  moon, 24, 24, 0xFFFF); //(x,y, name, DX, DY, color)
  		break;
  		}
  	case 3:{
  		if((ido.hour()>=5 && ido.minute()>15) && (ido.hour()>=5 && ido.minute()<45)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=17 && ido.minute()>40) && (ido.hour()>=18 && ido.minute()<10)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=5 && ido.minute()>=47) && (ido.hour()<17 && ido.minute()<=40)){
      		if(analogRead(LDR)>=200) tft.drawBitmap(0, 0,  sun, 48, 48, 0xFFFF); //(x,y, name, DX, DY, color)
      		else tft.drawBitmap(0, 0,  cloud, 47, 32, 0xFFFF); //(x,y, name, DX, DY, color)
    	}
    	else tft.drawBitmap(0, 0,  moon, 24, 24, 0xFFFF); //(x,y, name, DX, DY, color)
  		break;
  		}
  	case 4:{
  		if((ido.hour()>=5 && ido.minute()>10) && (ido.hour()>=5 && ido.minute()<45)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=19 && ido.minute()>20) && (ido.hour()>=19 && ido.minute()<55)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=5 && ido.minute()>=45) && (ido.hour()<19 && ido.minute()<=20)){
      		if(analogRead(LDR)>=200) tft.drawBitmap(0, 0,  sun, 48, 48, 0xFFFF); //(x,y, name, DX, DY, color)
      		else tft.drawBitmap(0, 0,  cloud, 47, 32, 0xFFFF); //(x,y, name, DX, DY, color)
    	}
    	else tft.drawBitmap(0, 0,  moon, 24, 24, 0xFFFF); //(x,y, name, DX, DY, color)
  		break;
  		}
  	case 5:{
  		if((ido.hour()=4 && ido.minute()>15) && (ido.hour()=4 && ido.minute()<55)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()=20 && ido.minute()>5) && (ido.hour()=20 && ido.minute()<45)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=4 && ido.minute()>=55) && (ido.hour()<20 && ido.minute()<=5)){
      		if(analogRead(LDR)>=200) tft.drawBitmap(0, 0,  sun, 48, 48, 0xFFFF); //(x,y, name, DX, DY, color)
      		else tft.drawBitmap(0, 0,  cloud, 47, 32, 0xFFFF); //(x,y, name, DX, DY, color)
    	}
    	else tft.drawBitmap(0, 0,  moon, 24, 24, 0xFFFF); //(x,y, name, DX, DY, color)
  		break;
  		}
  	case 6:{
  		if((ido.hour()>=3 && ido.minute()>55) && (ido.hour()>=4 && ido.minute()<35)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=20 && ido.minute()>30) && (ido.hour()>=21 && ido.minute()<15)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=4 && ido.minute()>=35) && (ido.hour()<20 && ido.minute()<=30)){
      		if(analogRead(LDR)>=200) tft.drawBitmap(0, 0,  sun, 48, 48, 0xFFFF); //(x,y, name, DX, DY, color)
      		else tft.drawBitmap(0, 0,  cloud, 47, 32, 0xFFFF); //(x,y, name, DX, DY, color)
    	}
    	else tft.drawBitmap(0, 0,  moon, 24, 24, 0xFFFF); //(x,y, name, DX, DY, color)
  		break;
  		}
  	case 7:{
  		if((ido.hour()>=4 && ido.minute()>15) && (ido.hour()>=4 && ido.minute()<55)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=20 && ido.minute()>25) && (ido.hour()>=21 && ido.minute()<1)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=4 && ido.minute()>=55) && (ido.hour()<20 && ido.minute()<=25)){
      		if(analogRead(LDR)>=200) tft.drawBitmap(0, 0,  sun, 48, 48, 0xFFFF); //(x,y, name, DX, DY, color)
      		else tft.drawBitmap(0, 0,  cloud, 47, 32, 0xFFFF); //(x,y, name, DX, DY, color)
    	}
    	else tft.drawBitmap(0, 0,  moon, 24, 24, 0xFFFF); //(x,y, name, DX, DY, color)
  		break;
  		}
  	case 8:{
  		if((ido.hour()>=4 && ido.minute()>55) && (ido.hour()>=5 && ido.minute()<30)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=19 && ido.minute()>45) && (ido.hour()>=20 && ido.minute()<15)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=5 && ido.minute()>=30) && (ido.hour()<19 && ido.minute()<=45)){
      		if(analogRead(LDR)>=200) tft.drawBitmap(0, 0,  sun, 48, 48, 0xFFFF); //(x,y, name, DX, DY, color)
      		else tft.drawBitmap(0, 0,  cloud, 47, 32, 0xFFFF); //(x,y, name, DX, DY, color)
    	}
    	else tft.drawBitmap(0, 0,  moon, 24, 24, 0xFFFF); //(x,y, name, DX, DY, color)
  		break;
  		}
  	case 9:{
  		if((ido.hour()>=5 && ido.minute()>40) && (ido.hour()>=6 && ido.minute()<15)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=18 && ido.minute()>45) && (ido.hour()>=19 && ido.minute()<15)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=6 && ido.minute()>=15) && (ido.hour()<18 && ido.minute()<=45)){
      		if(analogRead(LDR)>=200) tft.drawBitmap(0, 0,  sun, 48, 48, 0xFFFF); //(x,y, name, DX, DY, color)
      		else tft.drawBitmap(0, 0,  cloud, 47, 32, 0xFFFF); //(x,y, name, DX, DY, color)
    	}
    	else tft.drawBitmap(0, 0,  moon, 24, 24, 0xFFFF); //(x,y, name, DX, DY, color)
  		break;
  		}
  	case 10:{
  		if((ido.hour()>=6 && ido.minute()>20) && (ido.hour()>=6 && ido.minute()<55)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=17 && ido.minute()>45) && (ido.hour()>=18 && ido.minute()<15)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=6 && ido.minute()>=55) && (ido.hour()<17 && ido.minute()<=45)){
      		if(analogRead(LDR)>=200) tft.drawBitmap(0, 0,  sun, 48, 48, 0xFFFF); //(x,y, name, DX, DY, color)
      		else tft.drawBitmap(0, 0,  cloud, 47, 32, 0xFFFF); //(x,y, name, DX, DY, color)
    	}
    	else tft.drawBitmap(0, 0,  moon, 24, 24, 0xFFFF); //(x,y, name, DX, DY, color)
  		break;
  		}
  	case 11:{
  		if((ido.hour()>=6 && ido.minute()>5) && (ido.hour()>=6 && ido.minute()<40)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=15 && ido.minute()>55) && (ido.hour()>=16 && ido.minute()<30)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=6 && ido.minute()>=40) && (ido.hour()<15 && ido.minute()<=55)){
      		if(analogRead(LDR)>=200) tft.drawBitmap(0, 0,  sun, 48, 48, 0xFFFF); //(x,y, name, DX, DY, color)
      		else tft.drawBitmap(0, 0,  cloud, 47, 32, 0xFFFF); //(x,y, name, DX, DY, color)
    	}
    	else tft.drawBitmap(0, 0,  moon, 24, 24, 0xFFFF); //(x,y, name, DX, DY, color)
  		break;
  		}
  	case 12:{
  		if((ido.hour()>=6 && ido.minute()>40) && (ido.hour()>=7 && ido.minute()<15)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=15 && ido.minute()>40) && (ido.hour()>=16 && ido.minute()<15)) tft.drawBitmap(0, 0, sunLow, 48, 36, 0xFFFF);
  		if((ido.hour()>=7 && ido.minute()>=15) && (ido.hour()<15 && ido.minute()<=40)){
      		if(analogRead(LDR)>=200) tft.drawBitmap(0, 0,  sun, 48, 48, 0xFFFF); //(x,y, name, DX, DY, color)
      		else tft.drawBitmap(0, 0,  cloud, 47, 32, 0xFFFF); //(x,y, name, DX, DY, color)
    	}
    	else tft.drawBitmap(0, 0,  moon, 24, 24, 0xFFFF); //(x,y, name, DX, DY, color)
  		break;
  		}
  }
*/
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
  tft.fillRect(3,224,45,17,0x1E90FF);
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

  if(int(sensors.getTempCByIndex(0))<-10) tft.drawBitmap(293, 0,  thermometerZero, 16, 36, 0xFFFF);
  if(int(sensors.getTempCByIndex(0))>=-10 && int(sensors.getTempCByIndex(0))<-5) tft.drawBitmap(293, 0,  thermometer25, 16, 36, 0xFFFF);
  if(int(sensors.getTempCByIndex(0))>=-5 && int(sensors.getTempCByIndex(0))<10) tft.drawBitmap(293, 0,  thermometer50, 16, 36, 0xFFFF);
  if(int(sensors.getTempCByIndex(0))>=10 && int(sensors.getTempCByIndex(0))<30) tft.drawBitmap(293, 0,  thermometer75, 16, 36, 0xFFFF);
  if(int(sensors.getTempCByIndex(0))>=30) tft.drawBitmap(293, 0,  thermometer100, 16, 36, 0xFFFF);

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
  tft.fillRect(3,224,45,17,0x1E90FF);
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
  tft.fillRect(87,156,20,20,0x1E90FF);
  tft.print("+");
  tft.setCursor(210,155);
  tft.fillRect(207,156,20,20,0x1E90FF);
  tft.print("-");
  tft.setTextSize(2);
  tft.fillRect(3,224,45,17,0x1E90FF);
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
  tft.fillRect(87,156,20,20,0x1E90FF);
  tft.print("+");
  tft.setCursor(210,155);
  tft.fillRect(207,156,20,20,0x1E90FF);
  tft.print("-");
  tft.setTextSize(2);
  tft.fillRect(3,224,45,18,0x1E90FF);
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
  tft.fillRect(43,43,50,18,0x1E90FF); //fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  tft.fillRect(178,43,133,18,0x1E90FF);
  tft.fillRect(43,129,99,18,0x1E90FF);
  tft.fillRect(178,129,86,18,0x1E90FF);
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
