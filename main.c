#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ONE_WIRE_BUS 9
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal lcd(5, 7, 10, 11, 12, 13);

const int button = 2;
bool bState;

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  lcd.begin(20, 4); // set up the LCD's number of columns and rows:
  sensors.begin(); // Print a message to the LCD.
  int deviceCount=sensors.getDeviceCount(); /*Testing on startup*/
  lcd.print(deviceCount);
  display.clearDisplay();

  pinMode(button,INPUT);
}

void loop() {
  sensors.requestTemperatures(); // Send the command to get temperatures

  lcd.setCursor(0,0); // Outside temperature
  float temp=sensors.getTempCByIndex(0);
  lcd.print("Kulso: ");
  if(temp-(int)temp<0.5) lcd.print((int)temp);
  else lcd.print((int)temp+1);
  if(temp<-9){
    lcd.setCursor(11,0); lcd.print("C");
  }
  else {
    lcd.setCursor(10,0); lcd.print("C");
  }

    
  lcd.setCursor(0,1); //Average of inside temperature
  float tempA = ((sensors.getTempCByIndex(1)+sensors.getTempCByIndex(2))/2);
  lcd.print("Belso: ");
  if(tempA-(int)tempA<0.5) lcd.print((int)tempA);
  else lcd.print((int)tempA+1);
  lcd.setCursor(10,1);
  lcd.print("C");
  lcd.setCursor(0,3);
  
  int deviceCount = sensors.getDeviceCount();
  display.setCursor(0,30);
  display.print("Devices: ");
  display.print(deviceCount);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  
  if(digitalRead(button) == HIGH){
    bState=true;
  }
  else bState=false;
  
  display.display();
  delay(500);
  display.clearDisplay();
  
  //delay(1000);
}
