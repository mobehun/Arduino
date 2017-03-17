#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "RTClib.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>
#include <SPI.h>
#include <Stepper.h>

#define ONE_WIRE_BUS 7
#define TS_MINX 150     // This is calibration data for the raw touch data to the screen coordinates
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

const int stepsPerRevolution=200; // change this to fit the number of steps per revolution for your motor

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
RTC_DS1307 rtc;
Stepper myStepper(stepsPerRevolution,2,3,4,5); //H-Bridge 2,7,10,15 pins

void dateWrite(){
  tft.setTextSize(2);
  tft.fillScreen(ILI9341_BLACK);
  DateTime ido = rtc.now();
  tft.setCursor(105,105);
  tft.print(ido.year());
  tft.print('/');
  tft.print(ido.month());
  tft.print('/');
  tft.print(ido.day());

  tft.setCursor(4,225);
  tft.print("Vissza");
  
  if(!ts.bufferEmpty()){
    TS_Point p=ts.getPoint(); //Retrieve a point
    p.x=map(p.x,TS_MINY,TS_MAXY,0,tft.height());
    p.y=map(p.y,TS_MINX,TS_MAXX,0,tft.width());
    int y=tft.height()-p.x;
    int x=p.y;

    if(x>4 && x<60){
      if(y>220 && y<240){
        Serial.println("Back hit!");
        loop();
      }
    }
  }
  delay(10000);
  dateWrite();
}

void tempWrite(){
  tft.setTextSize(2);
  tft.fillScreen(ILI9341_BLACK);
  sensors.requestTemperatures(); // Send the command to get temperatures
  tft.setCursor(105,105);
  tft.print("Kulso: ");
  float temp=sensors.getTempCByIndex(0);
  if(temp-(int)temp<0.5) tft.print((int)temp);
  else tft.print((int)temp+1);
  tft.print(" C");
  
  tft.setCursor(105,120); //Average of inside temperature
  tft.print("Belso: ");
  temp = ((sensors.getTempCByIndex(1)+sensors.getTempCByIndex(2))/2);
  if(temp-(int)temp<0.5) tft.print((int)temp);
  else tft.print((int)temp+1);
  tft.print(" C");

  tft.setCursor(4,225);
  tft.print("Vissza");
  
  if(!ts.bufferEmpty()){
    TS_Point p=ts.getPoint(); //Retrieve a point
    p.x=map(p.x,TS_MINY,TS_MAXY,0,tft.height());
    p.y=map(p.y,TS_MINX,TS_MAXX,0,tft.width());
    int y=tft.height()-p.x;
    int x=p.y;

    if(x>160 && x<190){                                     //Area needs to be calculated
      if(y>160 && y<180){
        Serial.println("Back hit!");
        loop();
      }
    }
  }
  delay(10000);
  tempWrite();
}

void plusTurn(){
  int sensorReading=analogRead(A1);
  int motorSpeed=map(sensorReading,0,1023,0,100);
  if(motorSpeed>0){
    myStepper.setSpeed(motorSpeed);
    myStepper.step(stepsPerRevolution/100);
    }
  delay(500);
}

void plusTurn(int turns){
  int sensorReading=analogRead(A1);
  int motorSpeed=map(sensorReading,0,1023,0,100);
  if(motorSpeed>0){
    myStepper.setSpeed(motorSpeed);
    myStepper.step(stepsPerRevolution*turns);
    }
  delay(500);
}

void minusTurn(){
  int sensorReading=analogRead(A1); //10K Pot data pin connected to A1
  int motorSpeed=map(sensorReading,0,1023,0,100); //Mapping it to range 0-100
  if(motorSpeed>0){
    myStepper.setSpeed(motorSpeed);
    myStepper.step(-stepsPerRevolution/100); //Step 1/100 of a revolution
  }
  delay(500);
}

void minusTurn(int turns){
  int sensorReading=analogRead(A1); //10K Pot data pin connected to A1
  int motorSpeed=map(sensorReading,0,1023,0,100); //Mapping it to range 0-100
  if(motorSpeed>0){
    myStepper.setSpeed(motorSpeed);
    myStepper.step(-stepsPerRevolution*turns); //Step 1/100 of a revolution
  }
  delay(500);
}

void setBlinders(){
  
  bool manualMode = false;
  int sensorReading=analogRead(A0); // LDR data pin connected to A0
  Serial.println("Light level: ");
  Serial.println(sensorReading);

  tft.setTextSize(2);
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(20,80);
  tft.print("Current light level: ");
  tft.print(sensorReading);

  tft.setCursor(20,100);
  tft.print("Manual mode: ");

  if(!ts.bufferEmpty()){ //This SHOULD manually lower the blinders!!
    TS_Point p=ts.getPoint(); //Retrieve a point
    p.x=map(p.x,TS_MINY,TS_MAXY,0,tft.height());
    p.y=map(p.y,TS_MINX,TS_MAXX,0,tft.width());
    int y=tft.height()-p.x;
    int x=p.y;

    if(x>160 && x<190){
      if(y>160 && y<180){
        Serial.println("Manual mode hit");
        manualMode=true;
      }
    }
  }
  
  if(!manualMode){
    tft.println("OFF");
    if(sensorReading>=700)   //This SHOULD automatically lower the blinders!!
      plusTurn(2000); //Needs to be turned multipletimes 200=1 full turn!!
    sensorReading=analogRead(A0);
    if(sensorReading<=200)  //This SHOULD automatically lift the blinders!!
      minusTurn(2000); //Needs to be turned multipletimes 200=1 full turn!!
    yield();//What does this do??
  }
  else {
    tft.println("ON");
    tft.setCursor(90,155);
    tft.print("+");
    tft.setCursor(210,155);
    tft.print("-");
  }
  
  tft.setCursor(4,225);
  tft.print("Vissza");
  
  if(!ts.bufferEmpty()){ //This SHOULD manually lower the blinders!!
    TS_Point p=ts.getPoint(); //Retrieve a point
    p.x=map(p.x,TS_MINY,TS_MAXY,0,tft.height());
    p.y=map(p.y,TS_MINX,TS_MAXX,0,tft.width());
    int y=tft.height()-p.x;
    int x=p.y;
    
    if(manualMode){
      if(x>50 && x<110){
        if(y>30 && y<50){
          Serial.println("Plus sign hit");
          plusTurn();
        }
      }
      if(x>50 && x<110){
        if(y>30 && y<50){
          Serial.println("Minus sign hit");
          minusTurn();
        }
      }
  }
  if(x>50 && x<110){                                              //Area needs to be calculated
        if(y>30 && y<50){
          Serial.println("Back button hit");
          loop();
      }
    }
  }
  delay(10000);
  setBlinders();
}

void setHeaters(){
  tft.setTextSize(2);
  sensors.requestTemperatures();
  tft.setCursor(50,80);
  tft.print("Belso homerseklet: ");
  tft.setCursor(105,100);
  float temp=sensors.getTempCByIndex(0);
  if(temp-(int)temp<0.5) tft.print((int)temp);
  else tft.println((int)temp+1);
  tft.print(" C");

  tft.setCursor(90,155);
  tft.print("+");
  tft.setCursor(210,155);
  tft.print("-");

  tft.setCursor(4,225);
  tft.print("Vissza");
  
  if(!ts.bufferEmpty()){
    TS_Point p=ts.getPoint(); //Retrieve a point
    p.x=map(p.x,TS_MINY,TS_MAXY,0,tft.height());
    p.y=map(p.y,TS_MINX,TS_MAXX,0,tft.width());
    int y=tft.height()-p.x;
    int x=p.y;
    if(x>65 && x<85){
      if(y>175 && y<185){
        Serial.println("Plus sign hit");
        plusTurn();
      }
    }
    if(x>245 && x<260){
      if(y>175 && y<185){
        Serial.println("Minus sign hit");
        minusTurn();
      }
    }
    if(x>50 && x<110){                                              //Area needs to be calculated
          if(y>30 && y<50){
            Serial.println("Back button hit");
            loop();
        }
      }
    }
  setHeaters();
}

void setup() {
  Serial.begin(9600);
  sensors.begin(); // Print a message to the LCD.
  rtc.begin();
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//Needs to run only one time!
  
  tft.begin();
  if(!ts.begin()) Serial.println("Unable to start touchscreen!");
  else Serial.println("Touchscreen started.");
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(1);
}

void loop() {
  DateTime ido = rtc.now();
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(45,45);
  tft.println("Date");

  tft.setCursor(180,45);
  tft.println("Temperature");

  tft.setCursor(45,130);
  tft.println("Blinders");

  tft.setCursor(180,130);
  tft.println("Heating");

  tft.setCursor(180,210);
  tft.print(ido.hour());
  tft.print(":");
  tft.print(ido.minute());

  if(!ts.bufferEmpty()){
    TS_Point p=ts.getPoint(); //Retrieve a point
    p.x=map(p.x,TS_MINY,TS_MAXY,0,tft.height());
    p.y=map(p.y,TS_MINX,TS_MAXX,0,tft.width());
    int y=tft.height()-p.x;
    int x=p.y;

    if(x>50 && x<110){
      if(y>30 && y<50){
        Serial.println("Date hit!");
        dateWrite();
      }
    }
    if(x>175 && x<305){
      if(y>30 && y<50){
        Serial.println("Temperature hit!");
        tempWrite();
      }
    }
    if(x>40 && x<120){
      if(y>110 && y<130){
        Serial.println("Blinders hit!");
        setBlinders();
      }
    }
    if(x>200 && x<255){
      if(y>110 && y<130){
        Serial.println("Heating hit!");
        setHeaters();
      }
    }
  }
  delay(2000);
}
