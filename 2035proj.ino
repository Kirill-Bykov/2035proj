#include <AnalogKey.h>
AnalogKey<A0, 6> keys;
#include <GyverButton.h>
#include <DHT.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C  lcd(0x27, 2, 1, 0, 4, 5, 6, 7);
#define DHTPIN 12 // Номер пина для датчикая

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
    void p_act();
    void set_value();
    void set_addr_value(int menu_num_l, int line_num_l, float value_l);
    int  get_value();

  private:
    int values[2][6];
    String char_menu[2][6] = {{
        {"Temperature: " + String(values[0][1])},
        {"Humidity: " + String(values[0][2])},
        {"5555555555555555"},
        {"5555555555555555"},
        {"5555555555555555"},
        {"6666666666666666"}
      }, {
        {"Norm.t :" + String(values[1][1])},
        {"Norm.h :" + String(values[1][2])},
        {"EEEEEEEEEEEEEEEE"},
        {"EEEEEEEEEEEEEEEE"},
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
    friend struct Data;

};

//Инициализация главных классов
LCD_menu menu;

//Переменные температуры и влажности
struct Data {
  float temp;
  float humid;

  int *norm_temp = menu.values[1][1];
  int *norm_humid = menu.values[1][2];


  int temp_heter = 3;
  int humid_heter = 2;

  int max_temp = &norm_temp + temp_heter;
  int min_temp = &norm_temp - temp_heter;
  int max_humid = &norm_humid + humid_heter;
  int min_humid = &norm_humid - humid_heter;
};

// инициализация структуры

Data data;

//Вот это надо точно убрать :)
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

  keys.attach(0, 0);
  keys.attach(1, 130);
  keys.attach(2, 310);
  keys.attach(3, 477);
  keys.attach(4, 720);
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
  if (millis() - lastmillis > 1500000) {
    lcd.setBacklight(HIGH);      // автоматическое выключение подсветки при отсутсвии действий в течении 15 секунд
  }
  R_btn.tick(keys.status(0));
  U_btn.tick(keys.status(1));
  D_btn.tick(keys.status(2));
  L_btn.tick(keys.status(3));
  S_btn.tick(keys.status(4));
  P_btn.tick(keys.status(5));
  getTemp_Humid();
  menu.set_addr_value(0, 1, 100);
  menu.set_addr_value(0, 2, 100);
  key_pressed();
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
  data.temp = dht.readTemperature();
  data.humid = dht.readHumidity();
  if (isnan(data.humid) || isnan(data.temp)) {
    return;
  }
}

//Функция проверки температуры на соответствие уставкам. Выдает сигналы на включение реле.
void check_temp() {
  if (data.temp >= data.max_temp && not(last_temp_max)) {
    digitalWrite(temp_down_pin, LOW);
    digitalWrite(temp_up_pin, HIGH);
    last_temp_max = true;
    last_temp_min = false;
  }
  if (data.temp <= data.min_temp && not(last_temp_min)) {
    digitalWrite(temp_up_pin, LOW);
    digitalWrite(temp_down_pin, HIGH);
    last_temp_min = true;
    last_temp_max = false;
  }
}

//Функция проверки влажности на соответствие уставкам. Выдает сигналы на включение реле.
void check_humid() {
  if (data.humid >= data.max_humid && not(last_hum_max)) {
    digitalWrite(humid_down_pin, LOW);
    digitalWrite(humid_up_pin, HIGH);
    last_hum_max = true;
    last_hum_min = false;
  }
  if (data.humid <= data.min_humid && not(last_hum_min)) {
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
//конструктор класса
LCD_menu::LCD_menu() {
  this -> new_value = 0;
  this -> l1_num = 0;
  this -> l2_num = 1;
  this -> menu_num = 0;
  this -> s_flag = false;
}

void LCD_menu::set_addr_value(int menu_num_l = 0, int line_num_l = 0, float value_l = 0) {
  this->values[menu_num_l][line_num_l] = value_l;
}
//Функция получающая значение из класса
int LCD_menu::get_value() {
  if (not(((this -> addr[0]) >= 0) && ((this -> addr[0]) <= 1) && ((this -> addr[1]) >= 0) && ((this -> addr[1]) <= 5)))  return 0;
  else return this -> values[this -> addr[0]][this -> addr[1]];
}

//Функция записывающая значение в класс
void LCD_menu::set_value() {
  if (not(((this -> addr[0]) >= 0) && ((this -> addr[0]) <= 1) && ((this -> addr[1]) >= 0) && ((this -> addr[1]) <= 5)))  return;
  else this -> values[addr[0]][addr[1]] = this -> new_value;
}

//Функция счета времени
void LCD_menu::work_time() {
  (this -> seconds) = millis() / 1000 ;
  (this -> minutes) = (this -> seconds) / 60 ;
  (this -> hours) = (this -> minutes) / 60 ;
  (this -> seconds) = (this -> seconds) - (this -> minutes) * 60 ;
  (this -> minutes) = (this -> minutes) - (this -> hours) * 60 ;
}

//функция действие на нажатие кнопки Right
void LCD_menu::r_act() {
  if (this -> s_flag) {
    this->set_value();
    this->s_flag = false;
  }

  else if (not(this -> s_flag)) {
    if (this->menu_num == 0) this->menu_num = 1;
    else this->menu_num = 0;
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");

  }
}

//функция действие на нажатие кнопки Up
void LCD_menu::u_act() {
  Serial.print("U clicked");
  if (this -> s_flag) {
    this -> new_value += 1;
  }

  else {
    if ((this -> l1_num > 0 && this -> l1_num <= 4) && (this -> l2_num > 1 && this -> l2_num <= 5)) {
      this -> l1_num -= 1;
      this -> l2_num -= 1;
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
    }
  }
}

//функция действие на нажатие кнопки Down
void LCD_menu::d_act() {
  Serial.print("D clicked");
  if (this -> s_flag) {
    this -> new_value--;
  }
  else {
    if ((this -> l1_num >= 0 && this -> l1_num < 4) && (this -> l2_num >= 1 && this -> l2_num < 5)) {
      this -> l1_num += 1;
      this -> l2_num += 1;
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
    }
  }
}

//функция действие на нажатие кнопки Left
void LCD_menu::l_act() {
  Serial.print("L clicked");
  if (this -> s_flag) {
    this -> new_value = 0;
    this -> s_flag = false;
    this -> addr[0] = 0;
    this -> addr[1] = 0;
  }

  else if (not(this -> s_flag)) {
    if (this->menu_num == 0) this->menu_num = 1;
    else this->menu_num = 0;
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}

//функция действие на нажатие кнопки Sel
void LCD_menu::s_act() {
  Serial.print("S clicked");
  if (not(this -> s_flag)) {
    this->new_value = this->get_value();
    this->addr[0] = this->menu_num;
    this->addr[1] = this->l1_num;
    this->s_flag = true;
  }
  else if (this -> s_flag) {
    this->new_value = this->get_value();
    this->addr[0] = this->menu_num;
    this->addr[1] = this->l2_num;

  }
}


//функция действие на нажатие кнопки Rst ну она вообще то все перезапускает сама по дефолту так что :c
void LCD_menu::p_act() {
  return;
}
