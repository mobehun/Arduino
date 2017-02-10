#include <EEPROMex.h>
#include <EEPROMVar.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

volatile static double pulse_distance=0.00040255;
volatile static long double injector_value=0.003166666666667;

volatile double traveled_distance,display_traveled_dist,injector_open_time,used_fuel,tank_capacity=45,instant_fuel_cons,disp_instant_fuel_cons;
volatile float all_fuel_cons_values,avg_fuel_cons;
volatile unsigned long counter, avg_speed_div, dist_pulse_count,temp_dist_pulse_count,all_speed_values,injector_pulse_time,injTime1,injTime2,save_number,temp_injector_pulse_time;
volatile unsigned int speed,display_speed,avg_speed,max_speed=0,fccounter,save_counter,i;
volatile boolean to_be_saved;

void setup() {
  noInterrupts();
  lcd.begin();
  lcd.clear();
  
  TCCR1A=0;
  TCCR1B |= (1<<CS10) | (1<<CS11);
  TIMSK1 |= (1<<TOIE1);
  TCNT1=3036;
  
  pinMode(2,INPUT);
  pinMode(1,INPUT);
  pinMode(3,INPUT);
  digitalWrite(1,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  
  attachInterrupt(digitalPinToInterrupt(2),distance_count,FALLING);
  attachInterrupt(digitalPinToInterrupt(1),injector_count,CHANGE);
  
  EEPROM.setMemPool(0,1024);
  loadData();
  
  interrupts();
}

void loop() {
  
  if(digitalRead(3)==LOW){
    traveled_distance=0;
    used_fuel=0;
    avg_fuel_cons=0;
    avg_speed_div=0;
  }
  
  if(millis()%100==0){
    lcd.clear();
    
    lcd.setCursor(0,0);
    lcd.print("V:");
    lcd.setCursor(2,0);
    lcd.print(display_speed);
    lcd.setCursor(6,0);
    lcd.print("km/h");
    
    lcd.setCursor(0,1);
    lcd.print("Dst:");
    lcd.setCursor(4,1);
    lcd.print(display_traveled_dist,1);
    lcd.setCursor(9,1);
    lcd.print("km");
    
    lcd.setCursor(18,1);
    lcd.print(save_number);
    
    lcd.setCursor(0,2);
    lcd.print("AFC:");
    lcd.setCursor(4,2);
    lcd.print(avg_fuel_cons,1);
    lcd.setCursor(9,2);
    lcd.print("l/100km");
    
    lcd.setCursor(11,0);
    lcd.print("UF");
    lcd.setCursor(14,0);
    lcd.print(used_fuel,2);
    lcd.setCursor(19,0);
    lcd.print("L");
    
    lcd.setCursor(0,3);
    lcd.print("IFC");
    lcd.setCursor(4,3);
    lcd.print(disp_instant_fuel_cons,1);
    lcd.setCursor(9,3);
    if(speed>3) lcd.print("l/100km");
    else lcd.print("l/h");
  }
}

ISR(TIMER1_OVF_vect){
  counter++;
  
  if(temp_injector_pulse_time<800 && temp_dist_pulse_count==0 && to_be_saved==1) saveData();
  if(save_counter>45 && speed==0 && to_be_saved==1) saveData();
  temp_injector_pulse_time=0;
  temp_dist_pulse_count=0;
  
  if(counter>3){
    currentSpeed();
    fuelCons();
    if(instant_fuel_cons>0.3) to_be_saved=1;
    
    if(speed>3){
      avg_speed_div++;
      avgSpeed();
      maxSpeed();
    }
    save_counter++;
    dist_pulse_count=0;
    injector_pulse_time=0;
    counter=0;
    
    disp_instant_fuel_cons=instant_fuel_cons;
    display_traveled_dist=traveled_distance;
    display_speed=speed;
  }
  TCNT1=3036;
}

void fuelCons(){
  injector_open_time=((double)injector_pulse_time/1000000);
  
  if(speed>3) instant_fuel_cons=(100*(((injector_open_time*injector_value)*3600)*4))/speed;
  else instant_fuel_cons=((injector_open_time*injector_value)*3600)*4;
  if(speed==0 && instant_fuel_cons>5 || instant_fuel_cons>80){
    instant_fuel_cons=0;
    injector_open_time=0;
  }
  used_fuel+=((injector_open_time*injector_value)*4);
  if(tank_capacity>0) tank_capacity=45-used_fuel;
}

void distance_count(){
  dist_pulse_count++;
  traveled_distance+=pulse_distance;
  temp_dist_pulse_count++;
}

void injector_count(){
  if(digitalRead(1)==LOW) injTime1=micros();
  else {
    injTime2=micros();
    injector_pulse_time=injector_pulse_time+(injTime2-injTime1);
    temp_injector_pulse_time=temp_injector_pulse_time+(injTime2-injTime1);
    digitalWrite(1,HIGH);
  }
}

void currentSpeed(){
  if(speed==0 && dist_pulse_count>20){
    traveled_distance-=pulse_distance*dist_pulse_count;
    dist_pulse_count=0;
  }
  speed=pulse_distance*dist_pulse_count*3600;
}

void maxSpeed(){
  if(speed>max_speed){
    max_speed=speed;
    saveData();
  }
}

void avgSpeed(){
  all_speed_values+=speed;
  avg_speed=all_speed_values/avg_speed_div;
}

void saveData(){
  save_number++;
  if (save_number>=90000 && save_number<180000){i=50; EEPROM.updateInt(1000,5);}
  else if(save_number>=180000 && save_number<270000){i=100; EEPROM.updateInt(1002,5);}
  else if(save_number>=270000 && save_number<360000){i=150; EEPROM.updateInt(1004,5);}
  else if(save_number>=360000 && save_number<450000){i=200; EEPROM.updateInt(1006,5);}
  else if(save_number>=450000 ){i=250; EEPROM.updateInt(1008,5);}
  
  EEPROM.writeDouble(i+0,traveled_distance);
  EEPROM.writeInt(i+4,avg_speed);
  EEPROM.writeInt(i+6,avg_speed);
  
  EEPROM.writeFloat(i+8,all_fuel_cons_values);
  EEPROM.writeLong(i+12,avg_speed_div);
  
  EEPROM.writeDouble(i+16,used_fuel);
  EEPROM.writeLong(i+20,save_number);
  
  save_counter=0;
  to_be_saved=0;
}

void loadData(){
  if (EEPROM.readInt(1008)>1) i=250;
  else if (EEPROM.readInt(1006)>1) i=200;
  else if (EEPROM.readInt(1004)>1) i=150;
  else if (EEPROM.readInt(1002)>1) i=100;
  else if (EEPROM.readInt(10000)>1) i=50;
  
  traveled_distance=EEPROM.readDouble(i+0);
  if(isnan(EEPROM.readDouble(i+0))) traveled_distance=0;
  
  max_speed=EEPROM.readInt(i+4);
  avg_speed=EEPROM.readInt(i+6);
  
  all_fuel_cons_values=EEPROM.readFloat(i+8);
  if(isnan(EEPROM.readFloat(i+8))) all_fuel_cons_values=0;
  
  avg_speed_div=EEPROM.readLong(i+12);
  if(isnan(EEPROM.readLong(i+12))) avg_speed_div=0;
  
  used_fuel=EEPROM.readDouble(i+16);
  if(isnan(EEPROM.readDouble(i+16))) used_fuel=0;
  
  save_number=EEPROM.readLong(i+20);
  if(isnan(EEPROM.readLong(i+20))) save_number=0;
}
