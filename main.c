#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "RTClib.h"

#define ONE_WIRE_BUS 9

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal lcd(5, 7, 10, 11, 12, 13);
RTC_DS1307 rtc;
int sensorPin=A0;
int sensorValue;

void setup() {
  Serial.begin(9600);
  lcd.begin(20, 4); // set up the LCD's number of columns and rows:
  sensors.begin(); // Print a message to the LCD.
  rtc.begin();
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//Needs to run only one time!
}

void loop() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  
  lcd.setCursor(0,0);
  lcd.print("Temp: ");

  lcd.setCursor(0+6,0); // Outside temperature
  lcd.print("Kulso: ");
  float temp=sensors.getTempCByIndex(0);
  if(temp-(int)temp<0.5) lcd.print((int)temp);
  else lcd.print((int)temp+1);
  if(temp<-9){ lcd.setCursor(11+6,0);lcd.print((char)223); lcd.print("C"); } //(char)223=='°'
  else { lcd.setCursor(10+6,0);lcd.print((char)223); lcd.print("C"); }

  lcd.setCursor(0+6,1); //Average of inside temperature
  lcd.print("Belso: ");
  temp = ((sensors.getTempCByIndex(1)+sensors.getTempCByIndex(2))/2);
  if(temp-(int)temp<0.5) lcd.print((int)temp);
  else lcd.print((int)temp+1); lcd.setCursor(10+6,1); lcd.print((char)223); lcd.print("C");

  delay(5000);
  /*
  lcd.setCursor(0,3);
  int deviceCount = sensors.getDeviceCount();
  lcd.print(deviceCount);*/
  
  DateTime ido = rtc.now();
  lcd.clear();
  lcd.print("Date: ");
  lcd.setCursor(6,0);
  lcd.print(ido.year());
  lcd.print('/');
  lcd.print(ido.month());
  lcd.print('/');
  lcd.print(ido.day());
  lcd.setCursor(0,1);
  lcd.print("Time: ");
  lcd.setCursor(6,1);
  lcd.print(ido.hour());
  lcd.print(':');
  lcd.print(ido.minute());
  delay(5000);

/*
  lcd.clear();
  sensorValue=analogRead(sensorPin); //NOT YET IMPLEMENTED!
  lcd.print("Light level: ");
  lcd.print(sensorValue);*/
}
