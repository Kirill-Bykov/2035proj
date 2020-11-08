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
    void work_time();
    void r_act();
    void u_act();
    void d_act();
    void l_act();
    void s_act();
    int  get_value();

  private:
    float values[2][6];
    char char_menu[2][6][16] = {{
        {"Температура: " + char(values[0][0])},
        {"Влажность: " + char(values[0][1])},
        {"3333333333333333"},
        {"4444444444444444"},
        {"5555555555555555"},
        {"6666666666666666"}
      },

      { {"Экран наладки   "},
        {"Норм.t :" + char(values[1][1])},
        {"Норм.h :" + char(values[1][2])},
        {"DDDDDDDDDDDDDDDD"},
        {"EEEEEEEEEEEEEEEE"},
        {"FFFFFFFFFFFFFFFF"}
      }
    };
    int addr[2];
    int new_value;
    int l1_num = 0;
    int l2_num = 1;
    int menu_num;
    long int hours;
    long int minutes;
    long int seconds;
    boolean s_flag;

};

//Инициализация главных классов
LCD_menu menu;

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

//переменные времени
long int lastmillis;


GButton R_btn, L_btn, U_btn, D_btn, S_btn, P_btn;

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


}

void loop() {
  //  if (millis() - lastmillis > 15000) {
  //    lcd.setBacklight(HIGH);      // автоматическое выключение подсветки при отсутсвии действий в течении 15 секунд
  //  }

  R_btn.tick(keys.status(0));
  U_btn.tick(keys.status(1));
  D_btn.tick(keys.status(2));
  L_btn.tick(keys.status(3));
  S_btn.tick(keys.status(4));
  P_btn.tick(keys.status(5));
  menu.work_time();
  key_pressed();
  getTemp_Humid();
  check_temp();
  check_humid();
  menu.updateLCD();
}

//вывод на экран
void LCD_menu::updateLCD() {
  lcd.setCursor(0, 0);
  lcd.print(this ->char_menu[this -> menu_num][this -> l1_num]);
  lcd.setCursor(0, 1);
  lcd.print(this -> char_menu[this -> menu_num][this -> l2_num]);
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

//функция проверки нажатости кнопки и действия в соответствии них
void key_pressed() {

  if (R_btn.isClick()) menu.r_act();

  if (U_btn.isClick()) menu.u_act();

  if (D_btn.isClick()) menu.d_act();

  if (L_btn.isClick()) menu.l_act();

  if (S_btn.isClick()) menu.s_act();

  if (P_btn.isClick()) menu.p_act();
}
void LCD_menu::LCD_menu(LiquidCrystal_I2C){}
void LCD_menu::work_time {
  this -> seconds = millis() / 1000 ;
  this -> minutes = this -> seconds / 60 ;
  this -> hours = this -> minutes / 60 ;
  this -> seconds = this -> seconds - (this -> minutes * 60) ;
  this -> minutes = this -> minutes - this -> hours * 60 ;
}

//функция действие на нажатие кнопки Right
void LCD_menu::r_act() {
  if (this -> s_flag) {
    menu.set_value(this -> new_value, {this -> menu_num, this -> );
                  }
    else if (not(this -> s_flag)) {
      if (menu_num != 1) menu_num++;
      else menu_num--;
    }
  }
}

//функция действие на нажатие кнопки Up
void LCD_menu::u_act() {
  if (this -> s_flag) {
    this -> new_value++;
  }

  else {
    if (this -> l1_num >= 0 && this -> l1_num <= 4) {
      this -> l1_num -= 1;
    }
    if (this -> l2_num >= 1 && this -> l2_num <= 5) {
      this -> l2_num -= 1;
    }
  }
}

//функция действие на нажатие кнопки Down
void LCD_menu::d_act() {
  if (this -> s_flag) {
    this -> new_value--;
  }
  else {
    if (this -> l1_num >= 0 && this -> l1_num <= 4) {
      this -> l1_num += 1;
    }
    if (this -> l2_num >= 1 && this -> l2_num <= 5) {
      this -> l2_num += 1;
    }
  }
}

//функция действие на нажатие кнопки Left
void LCD_menu::l_act() {

}

//функция действие на нажатие кнопки Sel
void LCD_menu::s_act() {
  if (not(this -> s_flag)) {
    this -> new_value = menu.get_value(this -> menu_num, this -> this -> l1_num);
    this -> s_flag = true;
  }
  else if (this -> s_flag) {
    this -> new_value = menu.get_value(this -> menu_num, this -> this -> l2_num);
    
  }
}


//функция действие на нажатие кнопки Rst ну она вообще то все перезапускает сама по дефолту так что :c
void LCD_menu::p_act() {
  return;
}

//Функция получающая значение из класса
int LCD_menu::get_value(int[2] addr) {
  if (not(addr[0] >= 0 && addr[0] <= 1 && addr[1] >= 0 && addr[1] <= 5))  return 0;
  else return this -> values[addr[0]][addr[1]];
}

//Функция записывающая значение в класс
void LCD_menu::set_value(int value, int[2] addr) {
  if (not(addr[0] >= 0 && addr[0] <= 1 && addr[1] >= 0 && addr[1] <= 5))  return;
  else this -> values[addr[0], addr[1]] = value;
}
