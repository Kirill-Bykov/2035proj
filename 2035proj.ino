#include <AnalogKey.h>
AnalogKey<A0, 6> keys;
#include <GyverButton.h>
#include <DHT.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C  lcd(0x27, 2, 1, 0, 4, 5, 6, 7);
#define DHTPIN 12 // Номер пина для датчикая

DHT dht(DHTPIN, DHT22);//DHT22

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
    void set_values();
    void set_value();
    void set_menu();
    void set_addr_value(int menu_num_l, int line_num_l, float value_l);
    int  get_value();

  private:
    float values[2][6];
    String char_menu[2][6];
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
  public:
    float space_temp;
    float humid;
    float animal_temp;
    float animal_O2;
    float space_O2;
    float space_CO2;
   
    float *norm_space_temp = &menu.values[1][0];
    float *norm_humid = &menu.values[1][1];
    float *norm_animal_O2 = &menu.values[1][2];
    float *norm_space_O2 = &menu.values[1][3];
    float *norm_animal_temp = &menu.values[1][4];
    float *norm_space_CO2 = &menu.values[1][5];
    
    int space_temp_heter = 3;
    int humid_heter = 2;
    int animal_O2_heter = 3;
    int space_O2_heter = 3;
    int animal_temp_heter = 0.3;
    int space_CO2_heter = 3;
  
    int max_space_temp = *norm_space_temp + space_temp_heter;
    int min_space_temp = *norm_space_temp - space_temp_heter;
    int max_humid = *norm_humid + humid_heter;
    int min_humid = *norm_humid - humid_heter;
    int max_animal_O2 = *norm_animal_O2 + animal_O2_heter;
    int min_animal_O2 = *norm_animal_O2 - animal_O2_heter;
    int max_space_O2 = *norm_space_O2 + space_O2_heter;
    int min_space_O2 = *norm_space_O2 - space_O2_heter;
    int max_animal_temp = *norm_animal_temp + animal_temp_heter;
    int min_animal_temp = *norm_animal_temp - animal_temp_heter;
    int max_space_CO2 = *norm_space_CO2 + space_CO2_heter;
    int min_space_CO2 = *norm_space_CO2 - space_CO2_heter;
};

// инициализация структуры

Data data;

//номера пинов
int oxygenerator_pin = 1;
int cooling_fan_pin = 2;
int humidifier_pin = 3;
int carpet_pin = 4;
int UV_lamp_pin = 5;
int IR_lamp_pin = 6;
int ionizer_pin = 7;

//переменные времени
long int lastmillis;


GButton R_btn, L_btn, U_btn, D_btn, S_btn, P_btn;




//сигнатурный класс для коврика подогрева
class Device{
  public:
    Device(int pin){
      time_ON = 0;
      time_OFF = 0;
      start_millis = 0;
      stop_millis = 0;
      ON_OFF = false;
      device_pin = pin;
    }
    void on(){
      digitalWrite(device_pin,LOW);
      ON_OFF = true;
      start_millis = millis();
    }
    void off(){
      digitalWrite(device_pin,HIGH);
      ON_OFF = false;
      stop_millis = millis();
    }
    void timer(){
      if (ON_OFF){
        time_ON = millis() - start_millis;
        time_OFF = 0;
      }
      if (not(ON_OFF)){
        time_OFF = millis() - stop_millis;
        time_ON = 0;
      }
    }
  private:
    bool ON_OFF;
    long int time_ON;
    long int time_OFF;
    long int start_millis;
    long int stop_millis;
    int device_pin;
};

//вывод на экран
void LCD_menu::updateLCD() {
  lcd.setCursor(0, 0);
  lcd.print(char_menu[ menu_num][ l1_num]);
  lcd.setCursor(0, 1);
  lcd.print( char_menu[ menu_num][ l2_num]);
  lcd.setBacklight(LOW);      // Backlight ON
}

//задание значения в указанный адрес 
void LCD_menu::set_adr_value(int a1, int a2, float val){
  values[a1][a2] = val;
}

//Функция считывания температуры и влажности, проверка связи с датчиком.
void getTemp_Humid() {
  data.space_temp = dht.readTemperature();
  data.humid = dht.readHumidity();
  Serial.print(data.space_temp);
  if (isnan(data.humid) || isnan(data.space_temp)) {
    return;
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
  for(int i = 0; i<2;i++){
    for(int j = 0; j<6;j++){
      values[i][j] = 0;
    }
  }
  new_value = 0;
  l1_num = 0;
  l2_num = 1;
  menu_num = 0;
  s_flag = false;
}

//Отправка значений для индикации на экран
void LCD_menu::set_menu() {
  char_menu[0][0] = "SpaceTemp " + String(values[0][0]);
  char_menu[0][1] = "Humid " + String(values[0][1]);
  char_menu[0][2] = "AnimO2" + String(values[0][2]);
  char_menu[0][3] = "SpaceO2" + String(values[0][3]);
  char_menu[0][4] = "AnimTemp " + String(values[0][4]);
  char_menu[0][5] = "Space_CO2 " + String(values[0][5]);
  
  char_menu[1][0] = "nSpaceTemp " + String(values[1][0]);
  char_menu[1][1] = "nHumid " + String(values[1][1]);
  char_menu[1][2] = "nAnimO2 " + String(values[1][2]);
  char_menu[1][3] = "nSpaceO2 " + String(values[1][3]);
  char_menu[1][4] = "nAnimTemp " + String(values[1][4]);
  char_menu[1][5] = "nSpace " + String(values[1][5]);
}

void LCD_menu::set_values(){
  values[0][0] = data.space_temp;
  values[0][1] = data.humid;
  values[0][2] = data.animal_O2;
  values[0][3] = data.space_O2;
  values[0][4] = data.animal_temp;
  values[0][5] = data.space_CO2;
}
//Функция получающая значение из класса
int LCD_menu::get_value() {
  if (not((( addr[0]) >= 0) && (( addr[0]) <= 1) && (( addr[1]) >= 0) && (( addr[1]) <= 5)))  return 0;
  else return  values[ addr[0]][ addr[1]];
}

//Функция счета времени
void LCD_menu::work_time() {
  (seconds) = millis() / 1000 ;
  (minutes) = ( seconds) / 60 ;
  (hours) = ( minutes) / 60 ;
  (seconds) = ( seconds) - ( minutes) * 60 ;
  ( minutes) = ( minutes) - ( hours) * 60 ;
}

//функция действие на нажатие кнопки Right
void LCD_menu::r_act() {
  if ( s_flag) {
    set_value();
    s_flag = false;
  }

  else if (not(s_flag)) {
    if (menu_num == 0) menu_num = 1;
    else menu_num = 0;
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");

  }
}

//функция действие на нажатие кнопки Up
void LCD_menu::u_act() {
  Serial.print("U clicked");
  if (s_flag) {
    new_value += 1;
  }
  else {
    if ((l1_num > 0 && l1_num <= 4) && (l2_num > 1 && l2_num <= 5)) {
      l1_num -= 1;
      l2_num -= 1;
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
  if (s_flag) {
    new_value--;
  }
  else {
    if ((l1_num >= 0 && l1_num < 4) && (l2_num >= 1 && l2_num < 5)) {
      l1_num += 1;
      l2_num += 1;
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
  if (s_flag) {
    new_value = 0;
    s_flag = false;
    addr[0] = 0;
    addr[1] = 0;
  }

  else if (not(s_flag)) {
    if (menu_num == 0) menu_num = 1;
    else menu_num = 0;
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}

//функция действие на нажатие кнопки Sel
void LCD_menu::s_act() {
  Serial.print("S clicked");
  if (not(s_flag)) {
    s_flag = true;
    addr[0] = menu_num;
    addr[1] = l1_num;
    new_value = get_value();
  }
  else if (s_flag) {
    addr[0] = menu_num;
    addr[1] = l2_num;
    new_value = get_value();

  }
}

//функция действие на нажатие кнопки Rst ну она вообще то все перезапускает сама по дефолту так что :c
void LCD_menu::p_act() {
  return;
}

void setup() {

  pinMode(oxygenerator_pin, OUTPUT);
  pinMode(cooling_fan_pin, OUTPUT);
  pinMode(humidifier_pin, OUTPUT);
  pinMode(carpet_pin, OUTPUT);
  pinMode(UV_lamp_pin, OUTPUT);
  pinMode(IR_lamp_pin, OUTPUT);
  pinMode(ionizer_pin, OUTPUT);

  digitalWrite(oxygenerator_pin, HIGH);
  digitalWrite(cooling_fan_pin, HIGH);
  digitalWrite(humidifier_pin, HIGH);
  digitalWrite(carpet_pin, HIGH);
  digitalWrite(UV_lamp_pin, HIGH);
  digitalWrite(IR_lamp_pin, HIGH);
  digitalWrite(ionizer_pin, HIGH);

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
  lcd.home();

  Device oxygenerator(oxygenerator_pin);
  Device cooling_fan(cooling_fan_pin);
  Device humidifier(humidifier_pin);
  Device carpet(carpet_pin);
  Device UV_lamp(UV_lamp_pin);
  Device IR_lamp(IR_lamp_pin);
  Device ionizer(ionizer_pin);

  lastmillis = millis();

  Serial.begin(9600);

  dht.begin();
}

void loop() {
  if (millis() - lastmillis > 60000) {
    lcd.setBacklight(HIGH);      // автоматическое выключение подсветки при отсутсвии действий в течении минуты
  }
  R_btn.tick(keys.status(0));
  U_btn.tick(keys.status(1));
  D_btn.tick(keys.status(2));
  L_btn.tick(keys.status(3));
  S_btn.tick(keys.status(4));
  P_btn.tick(keys.status(5));
  
  menu.set_values();
  menu.set_menu();
  getTemp_Humid();
  key_pressed();
  menu.updateLCD();
}
