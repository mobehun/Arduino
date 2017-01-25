#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 9

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal lcd(5, 7, 10, 11, 12, 13);

void setup() {
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  sensors.begin();
  // Print a message to the LCD.
  int deviceCount=sensors.getDeviceCount(); /*Testing on startup*/
  lcd.print(deviceCount);
}

void loop() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  /*
  lcd.setCursor(0,0); // Inside temperature No.1
  lcd.print("TMP1: "); lcd.print(sensors.getTempCByIndex(1));
  lcd.setCursor(12,0); lcd.print("C");
  
  lcd.setCursor(0,1); // Inside temperature No.2
  lcd.print("TMP2: "); lcd.print(sensors.getTempCByIndex(2));
  lcd.setCursor(12,1); lcd.print("C");
  */
  lcd.setCursor(0,0); // Outside temperature
  float temp=sensors.getTempCByIndex(0);
  lcd.print("Kulso: ");
  if(temp-(int)temp<0.5) lcd.print((int)temp);
  else lcd.print((int)temp+1);
  lcd.setCursor(10,0); lcd.print("C");
    
  lcd.setCursor(0,1); //Average of inside temperature
  float tempA = ((sensors.getTempCByIndex(1)+sensors.getTempCByIndex(2))/2);
  lcd.print("Belso: ");
  if(tempA-(int)tempA<0.5) lcd.print((int)tempA);
  else lcd.print((int)tempA+1);
  lcd.setCursor(10,1);
  lcd.print("C");
  lcd.setCursor(0,3);
  /*int deviceCount = sensors.getDeviceCount();
  lcd.print(deviceCount);*/
  
  delay(1000);
}
