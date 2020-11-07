#include <AnalogKey.h>
#include <GyverButton.h>

#include "DHT.h"
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified backpack
#define DHTPIN 12 // Тот самый номер пина, о котором упоминалось выше
//`````DHT dht(DHTPIN, DHT22); //Инициация датчика
DHT dht(DHTPIN, DHT11);

// минимальные и максимальные уставки
float minimal_temperature = 15; 
float maximal_temperature = 25;
float minimal_humid = 35;
float maximal_humid = 60;

//Переменные температуры и влажности 
float temp;
float humid;

//колхозные переменные для отслеживания последних действий 
boolean last_temp_max = false;
boolean last_temp_min = false;
boolean last_hum_max = false;
boolean last_hum_min = false;

//номера пинов
int temp_up_pin = 5;
int temp_down_pin = 2;
int humid_up_pin = 3; 
int humid_down_pin = 4;

//переменные для экрана
int readkey;
long int lastmillis;
long int hours;
long int minutes;
long int seconds;
int l1;
int l2;
int menu_num;
char menu_lines_chars[2][6][16];
boolean R_pressed, L_pressed, U_pressed, D_pressed, S_pressed,P_pressed;

//попытка добавить библиотеку GyverButton


void setup() {
  pinMode(temp_up_pin,OUTPUT);
  pinMode(temp_down_pin,OUTPUT);
  pinMode(humid_up_pin,OUTPUT);
  pinMode(humid_down_pin,OUTPUT);
  digitalWrite(temp_up_pin,HIGH);
  digitalWrite(temp_down_pin,HIGH);
  digitalWrite(humid_up_pin,HIGH);
  digitalWrite(humid_down_pin,HIGH);
  Serial.begin(9600);
  dht.begin();
  lastmillis = millis();
  lcd.begin(16, 2);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(0);
  lcd.clear();
  lcd.home() ;
  keypad = Keypad();
  l1 = 0;
  l2 = 1;
}


void loop() {
   if (millis() - lastmillis > 15000) {
   lcd.setBacklight(HIGH);      // автоматическое выключение подсветки при отсутсвии действий в течении 15 секунд
 }
  
  seconds = millis() / 1000 ;
  minutes = seconds / 60 ;
  hours = minutes / 60 ;
  seconds = seconds - (minutes * 60) ;
  minutes = minutes - hours *60 ;
  keypad.key_pressed();
  
  if (R_pressed) {
    R_act()
  }
  else if(U_pressed) {
  }
  else if(D_pressed) {
  }
  else if(L_pressed) {
  }
  else if(S_pressed) {
    
  }
  else if(P_pressed) {
  } 
  getTemp_Humid();
  check_temp();
  check_humid();
}

//вывод на экран
void updateLCDline2(){
    lcd.setCursor(0, 0);
    lcd.print(menu_line_char[menu_num][l1]);
    lcd.setCursor(0, 1);
    lcd.print(menu_line_char[menu_num][l2]);
    lcd.setBacklight(LOW);      // Backlight ON
}



//Функция считывания температуры и влажности, проверка связи с датчиком.
void getTemp_Humid(){
  temp = dht.readTemperature();
  humid = dht.readHumidity();
  if (isnan(h) || isnan(t)) {
    return;
  }
}

//Функция проверки температуры на соответствие уставкам. Выдает сигналы на включение реле.
void check_temp(){
  if (t>= maximal_temperature && not(last_temp_max)){
    digitalWrite(temp_down_pin,LOW);
    digitalWrite(temp_up_pin,HIGH);
    last_temp_max = true;
    last_temp_min = false;
  }
  if (t<= minimal_temperature && not(last_temp_min)){
    digitalWrite(temp_up_pin,LOW);
    digitalWrite(temp_down_pin,HIGH);
    last_temp_min = true;
    last_temp_max = false;
  }
}

//Функция проверки влажности на соответствие уставкам. Выдает сигналы на включение реле.
void check_humid(){
  if (h>=maximal_humid && not(last_hum_max)){
    digitalWrite(humid_down_pin,LOW);
    digitalWrite(humid_up_pin,HIGH);
    last_hum_max = true;
    last_hum_min = false;
  }
  if (h<= minimal_humid && not(last_hum_min)){
    digitalWrite(humid_up_pin,LOW);
    digitalWrite(humid_down_pin,HIGH);
    last_hum_min = true;
    last_hum_max = false;
  }
}

//класс клавиатуры
class Keypad(){
  //Функция чтения 
  public:
    Keypad();x`
    keypressed;
    key_disable();
    
}

//
char Keypad::key_pressed(){
  readkey = analogRead(0);
    
  if (readkey>0 && readkey<800){
    updateLCDline2();
    lastmillis = millis();
  }
    
  if (readkey<50) {
    R_pressed = true;
    delay 5;
    return 'R';
  }
  else if(readkey<176) {
    U_pressed = true;
    delay 5;
    return 'U';
  }
  else if(readkey<332) {
    D_pressed = true;
    delay 5;
    return 'D';
 }
  else if(readkey<525) {
    L_pressed = true;
    delay 5;
    return 'L';
 }
  else if(readkey<750) {
    S_pressed = true;
    delay 5;
    return 'S';
  }
  else if(readkey<950) {
    P_pressed = true;
    S_pressed = false;
    delay 5;
    return 'P';
  } 
}
void Keypad::key_disable(){
  R_pressed = false;
  L_pressed = false;
  U_pressed = false;
  D_pressed = false;
  P_pressed = false;
}
//void act_R(S_pressed){
//  
//  }
