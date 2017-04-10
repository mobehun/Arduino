#include <SD.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "RTClib.h"

#include <memorysaver.h>
#include <UTFT.h>
#include <URTouch.h>

#include <SPI.h>
#include <Stepper.h>

#define ONE_WIRE_BUS 7

#define SD_CS 8

const int stepsPerRevolution=200; // change this to fit the number of steps per revolution for your motor
extern uint8_t hallfetica_normal[];
extern uint8_t SmallFont[];

UTFT tft(CTE50,38,39,40,41);
URTouch  ts( 6, 5, 4, 3, 2);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
RTC_DS1307 rtc;
Stepper myStepper(stepsPerRevolution,2,3,4,5); //H-Bridge 2,7,10,15 pins

void dateWrite(){
	tft.clrScr();
	DateTime ido = rtc.now();
  char cTime="Today is:";  cTime+=ido.year();  cTime+='/';  cTime+=ido.month();  cTime+='/';  cTime+=ido.day();
  tft.print(cTime,105,85);

	tft.print("Vissza",4,225);

  if(ts.dataAvailable()){
    ts.read();
    int x=ts.getX();
    int y=ts.getY();
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
  tft.clrScr();
	sensors.requestTemperatures(); // Send the command to get temperatures
	tft.print("Outside: ",85,85);
	float temp=sensors.getTempCByIndex(0);
	if(temp-(int)temp<0.5) tft.print((int)temp,157,85);
	else tft.print((int)temp+1,157,85);
  tft.setFont(hallfetica_normal);
  tft.print("`C",181,85);

  tft.setFont(SmallFont);
	tft.print("Inside: ",85,105); //Average of inside temperature
	temp = ((sensors.getTempCByIndex(1)+sensors.getTempCByIndex(2))/2);
	if(temp-(int)temp<0.5) tft.print((int)temp,149,105);
	else tft.print((int)temp+1,149,105);
	tft.setFont(hallfetica_normal);
  tft.print("`C",181,85);
  tft.setFont(SmallFont);

	int sensorReading=analogRead(A0); // LDR data pin connected to A0
	tft.print("Light level: ",85,125); tft.print(sensorReading,189,125); tft.print(" lx",213,125);

	tft.print("Vissza",4,225);
	
	if(ts.dataAvailable()){
    ts.read();
    int x=ts.getX();
    int y=ts.getY();
		if(x>160 && x<190){ //Area needs to be calculated
			if(y>160 && y<180){
				Serial.println("Back hit!");
				loop();
			}
		}
	}
	delay(10000);
	tempWrite();
}

void plusTurn(int turns, char motorNumb){
	int sensorReading=analogRead(motorNumb);			//10K Pot data pin connected to A1
	int motorSpeed=map(sensorReading,0,1023,0,100);		//Mapping it to range 0-100
	if(motorSpeed>0){
		myStepper.setSpeed(motorSpeed);
		myStepper.step(stepsPerRevolution*turns);		//Step 1/100 of a revolution
		}
	delay(500);
}

void minusTurn(int turns, char motorNumb){
	int sensorReading=analogRead(motorNumb); 
	int motorSpeed=map(sensorReading,0,1023,0,100); 
	if(motorSpeed>0){
		myStepper.setSpeed(motorSpeed);
		myStepper.step(-stepsPerRevolution*turns);
	}
	delay(500);
}

void setBlinders(){
	DateTime ido = rtc.now();
	bool manualMode = true;
	int sensorReading=analogRead(A0);		// LDR data pin connected to A0
	Serial.print("Light level: ");
	Serial.println(sensorReading);

  tft.clrScr();
	tft.print("Light level: ",20,80); tft.print(sensorReading,124,80); tft.print(" lx",148,80);

	tft.print("Manual mode: ON",20,100);
	tft.print("+",90,155);
	tft.print("-",210,155);

	if(ts.dataAvailable()){
    ts.read();
    int x=ts.getX();
    int y=ts.getY();

		if(x>145 && x<185){					//+-5px
			if(y>95 && y<121){
				Serial.println("Manual mode hit");
				tft.clrScr();
				tft.print("Light level: ",20,80); tft.print(sensorReading,124,80); tft.print(" lx",148,80);
				tft.print("Manual mode: ",20,100);
				manualMode=false;
			}
		}
	}
	
	if(!manualMode){ // If manual mode is false
		tft.print("OFF",124,100);				//This SHOULD automatically lower the blinders!!
		if(ido.hour()>=18)				//If it's 6PM it will lower the blinders
			//if(sensorReading<=100)	//For now this is not really necessarry...
				plusTurn(2000,"A1");	//Needs to be turned multipletimes, 200=1 full turn, A1 is analog pin No.1
		
		if(ido.hour()>=8)
			//if(sensorReading<=200)	
				minusTurn(2000,"A1");	//This SHOULD automatically lift the blinders!!
		//yield();//What does this do??
	}
	
	tft.print("Vissza",4,225);
	
	if(ts.dataAvailable()){
    ts.read();
    int x=ts.getX();
    int y=ts.getY();
		
		if(manualMode){
			if(x>80 && x<115){
				if(y>135 && y<175){
					Serial.println("Plus sign hit");
					plusTurn(1,"A1");
				}
			}
			if(x>195 && x<235){
				if(y>135 && y<175){
					Serial.println("Minus sign hit");
					minusTurn(1,"A1");
				}
			}
		}
		if(x>0 && x<85){
			if(y>205 && y<240){
				Serial.println("Back button hit");
				loop();
			}
		}
	}
	delay(10000);
	setBlinders();
}

void setHeaters(){
	sensors.requestTemperatures();
	tft.clrScr();
	tft.print("Inside temperature: ",50,80);
	float temp = ((sensors.getTempCByIndex(1)+sensors.getTempCByIndex(2))/2);
	if(temp-(int)temp<0.5) tft.print((int)temp,135,100);
	else tft.print((int)temp+1,135,100);
	tft.setFont(hallfetica_normal);
  tft.print("`C",181,85);
  tft.setFont(SmallFont);

	tft.print("+",90,155);
	tft.print("-",210,155);

	tft.print("Vissza",4,225);
	
	if(ts.dataAvailable()){
    ts.read();
    int x=ts.getX();
    int y=ts.getY();
    
		if(x>75 && x<115){
			if(y>140 && y<185){
				Serial.println("Plus sign hit");
				plusTurn(1,"A2");
			}
		}
		if(x>195 && x<235){
			if(y>140 && y<185){
				Serial.println("Minus sign hit");
				minusTurn(1,"A2");
			}
		}
		if(x>0 && x<85){
			if(y>205 && y<240){
				Serial.println("Back button hit");
				loop();
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
	
  tft.InitLCD();
  tft.clrScr();

  ts.InitTouch();
  ts.setPrecision(PREC_MEDIUM);

 Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
  }
  Serial.println("OK!");
}

void loop() {	
	DateTime ido = rtc.now();
  tft.print("Date",45,45);

  tft.print("Temperature",180,45);

  tft.print("Blinders",45,130);

  tft.print("Heating",180,130);

  char cTime=ido.hour()+":"+ido.minute();
  tft.print(cTime,180,210);

	if(ts.dataAvailable()){
    ts.read();
    int x=ts.getX();
    int y=ts.getY();

		if(x>30 && x<100){
			if(y>30 && y<70){
				Serial.println("Date hit!");
				dateWrite();
			}
		}
		if(x>165 && x<315){
			if(y>30 && y<70){
				Serial.println("Temperature hit!");
				tempWrite();
			}
		}
		if(x>30 && x<170){
			if(y>115 && y<155){
				Serial.println("Blinders hit!");
				setBlinders();
			}
		}
		if(x>165 && x<265){
			if(y>115 && y<155){
				Serial.println("Heating hit!");
				setHeaters();
			}
		}
	}
	delay(2000);
}
