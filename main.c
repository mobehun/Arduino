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

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal lcd(5, 7, 10, 11, 12, 13);

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  lcd.begin(20, 4); // set up the LCD's number of columns and rows:
  sensors.begin(); // Print a message to the LCD.
  int deviceCount=sensors.getDeviceCount(); /*Testing on startup*/
  lcd.print(deviceCount);
  display.clearDisplay();
  
  /*display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Kulso: ");
  display.display();
  delay(1000);
  display.clearDisplay();*/
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
  /*lcd.setCursor(0,0); // Outside temperature
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
  lcd.setCursor(0,3);*/
  
  /*int deviceCount = sensors.getDeviceCount();
  lcd.print(deviceCount);*/

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  float temp=sensors.getTempCByIndex(0);
  display.print("Kulso: ");
  if(temp-(int)temp<0.5) display.print((int)temp);
  else display.print((int)temp+1);
  display.setCursor(10,0); display.print("C");

  display.setCursor(0,1); //Average of inside temperature
  float tempA = ((sensors.getTempCByIndex(1)+sensors.getTempCByIndex(2))/2);
  display.print("Belso: ");
  if(tempA-(int)tempA<0.5) display.print((int)tempA);
  else display.print((int)tempA+1);
  display.setCursor(10,1); display.print("C");
  
  display.display();
  delay(1000);
  display.clearDisplay();
  
  //delay(1000);
}
