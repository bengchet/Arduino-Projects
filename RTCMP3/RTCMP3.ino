/*
 * TimeRTC.pde
 * example code illustrating Time library with Real Time Clock.
 * 
 */
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
#include <LiquidCrystal.h>
#include <CytronEZMP3.h>

#define BTN A1
uint8_t prevBtnStatus = 1;
boolean play = 0;

CytronEZMP3 mp3;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

String weekDay[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
tmElements_t tm;

void setup()  {

  pinMode(BTN, INPUT_PULLUP);
  
  Serial.begin(9600);
  while (!Serial) ; // wait until Arduino Serial Monitor opens
  lcd.begin(8, 2);
  
  //run this for first time
  //setupTime();
  //while(1);

  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  lcd.clear();
  if(timeStatus()!= timeSet) {
     lcd.print("RTC fail");
     while(1);
  }
  else{
     Serial.println("RTC has set the system time");
  }

  //initialise mp3
  if(!mp3.begin(2, 3))
  {
    lcd.clear();
    lcd.print("MP3 fail");
    //while(1);
  }
  mp3.setVolume(28);   
}

void loop()
{
  if (timeStatus() == timeSet) {
    lcd.clear();
    digitalClockDisplay();
  } else {
  }
  /*if(hour() == 12 && minute() == 00 && second() == 0){
    mp3.play();
  }
  if(hour() == 12 && minute() == 00 && second() == 30){
    mp3.stop();
  }*/

  uint8_t currentBtnStatus = digitalRead(BTN);
  if(currentBtnStatus != prevBtnStatus){
    
    if(currentBtnStatus == 0){
      play ^= 1;
      if(play)
        mp3.play();
      else
        mp3.stop();
    }
    else{
      //nothing to do
    }

    prevBtnStatus = currentBtnStatus; 
  }
    
  delay(10);
}

void setupTime(){
  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    RTC.write(tm);
  }
}

void digitalClockDisplay(){
  // digital clock display of the time
  lcd.print(weekDay[weekday()-1]);
  lcd.print(" ");
  printDateDigits(day());
  printDateDigits(month());
  
  lcd.setCursor(0,1);
  printDateDigits(hour());
  printDigits(minute());
  printDigits(second());
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  lcd.print(":");
  if(digits < 10)
    lcd.print('0');
  lcd.print(digits);
}

void printDateDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  if(digits < 10)
    lcd.print('0');
  lcd.print(digits);
}

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

