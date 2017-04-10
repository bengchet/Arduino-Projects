#include <LiquidCrystal.h>
#include <EEPROM.h>
#define USER_DATA 3

#define BTN_UP 10
#define BTN_DOWN 11
#define BTN_SET 12
#define LED_STAT 13
#define PIR A1
#define HEATER A5
#define FAN 3
#define FAN_DIR 2

//define address
//user_id*USER_DATA category
//user_id*USER_DATA + 1 temperature limit
//user_id*USER_DATA + 2 time limit

//address 255 is to store number of users

const char* main_menu[2]{
  "Choose setup", 
  "Add New Setup",
};

const char* settings_menu[3]{
  "Food Category", 
  "Temp Limit (*C)",
  "Time Limit (min)",
};

const char* category[5]{
  "Meat",
  "Soup",
  "Fish",
  "Noodles",
  "Vegetables"
};

int settings_params[3][2]{
  {0, 5},
  {30, 51},
  {0, 61} 
};

void (*action[3])(void){
  mainMenu,
  choose_settings,
  settings,
};

int menu_index = 0;
int action_index = 0;
int temp_limit;
unsigned long time_limit;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {

  Serial.begin(115200);
  //settings
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SET, INPUT_PULLUP);
  pinMode(LED_STAT, OUTPUT);
  pinMode(PIR, INPUT);
  pinMode(FAN, OUTPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(FAN_DIR, OUTPUT);

  digitalWrite(HEATER, 1);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.clear();
  lcd.print("Welcome to");
  lcd.setCursor(0,1);
  lcd.print("WinterWarmers");
  delay(2000);
  lcd.clear();
}

void loop() {

  (*action[action_index])();
}


void mainMenu(){

  lcd.clear();
  lcd.print(String(menu_index+1)+"."+main_menu[menu_index]);

  while(true){
    if(!digitalRead(BTN_UP)){
      if(--menu_index < 0) menu_index = sizeof(main_menu)/sizeof(const char*) - 1;
      lcd.clear();
      lcd.print(String(menu_index+1)+"."+main_menu[menu_index]);
      delay(200);
      while(!digitalRead(BTN_UP));
    }
    else if(!digitalRead(BTN_DOWN)){
      if(++menu_index > sizeof(main_menu)/sizeof(const char*)-1 ) menu_index = 0;
      lcd.clear();
      lcd.print(String(menu_index+1)+"."+main_menu[menu_index]);
      delay(200);
      while(!digitalRead(BTN_DOWN));
    }
    else if(!digitalRead(BTN_SET)){
      action_index = menu_index + 1;
      menu_index = 0;
      lcd.clear();
      delay(200);
      while(!digitalRead(BTN_SET));
      break;
    }
    else{
      //todo: unknown
    }
  }
    
}

void settings(){

  byte dat[3]={0};

  for(menu_index = 0; menu_index < sizeof(settings_menu)/sizeof(const char*); menu_index++){

    lcd.clear();
    //Setting up Category, Temperature Limit and Time Limit
    lcd.print(settings_menu[menu_index]);
    lcd.setCursor(0,1);
    byte current_val = (settings_params[menu_index][0]+settings_params[menu_index][1])/2;
    lcd.print(menu_index == 0? category[current_val]:String(current_val));

    while(true){
      if(!digitalRead(BTN_UP)){
        lcd.clear();
        lcd.print(settings_menu[menu_index]);
        lcd.setCursor(0,1);
        if(--current_val < 0){
          current_val = settings_params[menu_index][1]-1;
        }
        lcd.print(menu_index == 0? category[current_val]:String(current_val));
        delay(200);
      }
      else if(!digitalRead(BTN_DOWN)){
        lcd.clear();
        lcd.print(settings_menu[menu_index]);
        lcd.setCursor(0,1);
        if(++current_val > settings_params[menu_index][1]-1){
          current_val = 0;
        }
        lcd.print(menu_index == 0? category[current_val]:String(current_val));
        delay(200);
      }
      else if(!digitalRead(BTN_SET)){
        dat[menu_index] = current_val;
        Serial.println(dat[menu_index]);
        while(!digitalRead(BTN_SET));
        delay(200);
        break;
      }
    }
   
  }

  //
  byte users = EEPROM.read(255);
  Serial.print("Before: ");Serial.println(users);
  EEPROM.write(255, users + 1);
  Serial.print("After: ");
  byte newUsers = EEPROM.read(255);Serial.println(newUsers);
  
  for(int i=0; i< 3; i++){
    EEPROM.write((newUsers-1)*USER_DATA + i, dat[i]);
    Serial.println(EEPROM.read((newUsers-1)*USER_DATA + i));
  }
  action_index = 0;
  menu_index = 0;
  lcd.clear();
  lcd.print("Settings saved");
  delay(2000);

}

void choose_settings(){

  int user_id = 0;
  lcd.clear();
  byte value = EEPROM.read(255);
  if(value == 255 || value == 0){
    lcd.print("No User");
    delay(2000);
    action_index = 0;
    return;
  }

  //get first data
  byte cat = EEPROM.read(user_id*USER_DATA);
  byte temp = EEPROM.read(user_id*USER_DATA + 1);
  byte time = EEPROM.read(user_id*USER_DATA + 2);
  lcd.clear();
  lcd.print(category[cat]);
  lcd.setCursor(0,1);
  lcd.print(String(temp));lcd.print("*C ");
  lcd.print(String(time));lcd.print("min");

  
  while(true){
    if(!digitalRead(BTN_UP)){
      if(--user_id < 0) user_id = value - 1;
      lcd.clear();
      byte cat = EEPROM.read(user_id*USER_DATA);
      byte temp = EEPROM.read(user_id*USER_DATA + 1);
      byte time = EEPROM.read(user_id*USER_DATA + 2);
      lcd.clear();
      lcd.print(category[cat]);
      lcd.setCursor(0,1);
      lcd.print(String(temp));lcd.print("*C ");
      lcd.print(String(time));lcd.print("min");
      delay(200);
    }
    else if(!digitalRead(BTN_DOWN)){
      if(++user_id > value-1) user_id = 0;
      lcd.clear();
      byte cat = EEPROM.read(user_id*USER_DATA);
      byte temp = EEPROM.read(user_id*USER_DATA + 1);
      byte time = EEPROM.read(user_id*USER_DATA + 2);
      lcd.clear();
      lcd.print(category[cat]);
      lcd.setCursor(0,1);
      lcd.print(String(temp));lcd.print("*C ");
      lcd.print(String(time));lcd.print("min");
      delay(200);
    }
    else if(!digitalRead(BTN_SET)){
      process_start(temp ,time);
      action_index = 0;
      menu_index = 0;
      delay(2000);
      break;
    }
  }
  
  
}

void process_start(byte temp, byte time){

  unsigned long time_in = millis();
  temp_limit = temp;
  time_limit = time;

  digitalWrite(LED_STAT, 1);

  analogWrite(FAN, 3);
  digitalWrite(FAN_DIR, 1);

  digitalWrite(HEATER, 0);
  
  lcd.clear();
  delay(500);
  
  while(millis()-time_in < 1000 * time_limit){
    int temp = temperature();
    lcd.home();
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print("*C");
  
//    if(temperature() > temp_limit + 5){
//      analogWrite(FAN, 0);
//      digitalWrite(HEATER, 1);
//    }
//    else if(temperature() < temp_limit - 5){
//      analogWrite(FAN, 5);
//      digitalWrite(HEATER, 0);
//    }

    //checkPIR, stop fan and heater if motion detected
    //if(digitalRead(PIR) == LOW) break;
    Serial.println(temp);
    
  }
  //turn off
  analogWrite(FAN, 0);
  digitalWrite(HEATER, 1);
  digitalWrite(LED_STAT ,0);
  
}

