#include <AnalogKey.h>
AnalogKey<A0, 6> keys;
#include <GyverButton.h>
#include "DHT.h"
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C  lcd(0x27, 2, 1, 0, 4, 5, 6, 7);
#define DHTPIN 12 // Номер пина для датчика

DHT dht(DHTPIN, DHT11);//DHT22
//Класс управляющий выводом на экран
class LCD_menu {
  public:
    LCD_menu();
    void updateLCD();
    void set_line_char():
    void r_act();
    void u_act();
    void d_act();
    void l_act();
    void s_act();

    private:
      char menu[2][6][16] = {{
        {"1111111111111111"},
        {"2222222222222222"},
        {"3333333333333333"},
        {"4444444444444444"},
        {"5555555555555555"},
        {"6666666666666666"}
      },

      { {"AAAAAAAAAAAAAAAA"},
        {"BBBBBBBBBBBBBBBB"},
        {"CCCCCCCCCCCCCCCC"},
        {"DDDDDDDDDDDDDDDD"},
        {"EEEEEEEEEEEEEEEE"},
        {"FFFFFFFFFFFFFFFF"}
      }
    };
      int l1_num;
      int l2_num;
      int menu_num;
      long int lastmillis;
      long int hours;
      long int minutes;
      long int seconds;


};
//Класс управляющий приборами
class Manager {
  public:
    Manager();
    void set_value();
    int  get_value();
  private:
    int menu_addr, line_addr;
    float values[2][6]; // Массив с уставками и всем таким 
};

//Инициализация главных классов
LCD_menu menu;
Manager mng;

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


GButton R_btn, L_btn, U_btn, D_btn, S_btn, R_btn;

//попытка добавить библиотеку GyverButton

void setup() {

  pinMode(temp_up_pin, OUTPUT);
  pinMode(temp_down_pin, OUTPUT);
  pinMode(humid_up_pin, OUTPUT);
  pinMode(humid_down_pin, OUTPUT);

  digitalWrite(temp_up_pin, HIGH);
  digitalWrite(temp_down_pin, HIGH);
  digitalWrite(humid_up_pin, HIGH);
  digitalWrite(humid_down_pin, HIGH);

  keys.attach(0, 10);
  keys.attach(1, 150);
  keys.attach(2, 330);
  keys.attach(3, 515);
  keys.attach(4, 740);
  keys.attach(5, 930);

  lcd.begin(16, 2);
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(0);
  lcd.clear();
  lcd.home() ;

  lastmillis = millis();

  Serial.begin(9600);

  dht.begin();

  l1_num = 0;
  l2_num = 1;

}

void loop() {
  if (millis() - lastmillis > 15000) {
    lcd.setBacklight(HIGH);      // автоматическое выключение подсветки при отсутсвии действий в течении 15 секунд
  }
  R_btn.tick(keys.status(0));
  U_btn.tick(keys.status(1));
  D_btn.tick(keys.status(2));
  L_btn.tick(keys.status(3));
  S_btn.tick(keys.status(4));
  P_btn.tick(keys.status(5));

  work_time();
  key_pressed();
  getTemp_Humid();
  check_temp();
  check_humid();
}

//вывод на экран
void LCD_menu::updateLCD() {
  lcd.setCursor(0, 0);
  lcd.print(menu[menu_num][l1_num]);
  lcd.setCursor(0, 1);
  lcd.print(menu[menu_num][l2_num]);
  lcd.setBacklight(LOW);      // Backlight ON
}

//Функция считывания температуры и влажности, проверка связи с датчиком.
void getTemp_Humid() {
  temp = dht.readTemperature();
  humid = dht.readHumidity();
  if (isnan(humid) || isnan(temp)) {
    return;
  }
}

//Функция проверки температуры на соответствие уставкам. Выдает сигналы на включение реле.
void check_temp() {
  if (temp >= max_temp && not(last_temp_max)) {
    digitalWrite(temp_down_pin, LOW);
    digitalWrite(temp_up_pin, HIGH);
    last_temp_max = true;
    last_temp_min = false;
  }
  if (temp <= min_temp && not(last_temp_min)) {
    digitalWrite(temp_up_pin, LOW);
    digitalWrite(temp_down_pin, HIGH);
    last_temp_min = true;
    last_temp_max = false;
  }
}

//Функция проверки влажности на соответствие уставкам. Выдает сигналы на включение реле.
void check_humid() {
  if (humid >= max_humid && not(last_hum_max)) {
    digitalWrite(humid_down_pin, LOW);
    digitalWrite(humid_up_pin, HIGH);
    last_hum_max = true;
    last_hum_min = false;
  }
  if (humid <= min_humid && not(last_hum_min)) {
    digitalWrite(humid_up_pin, LOW);
    digitalWrite(humid_down_pin, HIGH);
    last_hum_min = true;
    last_hum_max = false;
  }
}

void key_pressed() {

  if (R_btn.isClick()) R_act();

  if (U_btn.isClick()) U_act();

  if (D_btn.isClick()) D_act();

  if (L_btn.isClick()) L_act();

  if (S_btn.isClick()) S_act();

  if (P_btn.isClick()) P_act();
}

void LCD_menu::work_time() {
  seconds = millis() / 1000 ;
  minutes = seconds / 60 ;
  hours = minutes / 60 ;
  seconds = seconds - (minutes * 60) ;
  minutes = minutes - hours * 60 ;
}

//функция действие на нажатие кнопки Right
void LCD_menu::R_act() {
  if (S_flag) {
    mng.set_value(this.value,this.l_val);
    }
  else if (not(S_flag)) {
    if (menu_num != 1) menu_num++;
    else menu_num--;
  }
}

//функция действие на нажатие кнопки Up
void LCD_menu::U_act() {
  if (S_flag) {
    if not(isnan(l_val)) {
      this.menu[l_val]++;
  }
}

//функция действие на нажатие кнопки Down
void LCD_menu::D_act() {
  
}

//функция действие на нажатие кнопки Left
void LCD_menu::L_act() 

//функция действие на нажатие кнопки Sel
void LCD_menu::S_act(){
  if (not(S_flag)){
    this.value = mng.get_value({this.menu_num,this.l1);
  }
  else if (S_flag){ 
    this.value = mng.get_value({this.menu_num,this.l2);
  }
}

//функция действие на нажатие кнопки Rst ну она вообще то все перезапускает сама по дефолту так что :c
void LCD_menu::P_act(){
  
}

void LCD_menu::set_line_char(){
  
}
void Manage::Manage;

int Manage::get_value(int[2] addr){
  if (not(addr[0]>=0 && addr[0] <=1 && addr[1] >= 0 && addr[1] <=5)){
    return 0
    }
  else return this.values[addr[0]][addr[1]];
  }
}
void Manage::set_value{
  
}
