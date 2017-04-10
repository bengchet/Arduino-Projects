#include <CytronWiFiShield.h>
#include <CytronWiFiClient.h>
#include <SoftwareSerial.h>
#include <ThingSpeak.h>

const char *ssid = "Cytron-Asus";
const char *pass = "f5f4f3f2f1";
ESP8266Client client;
unsigned long weatherStationChannelNumber = 12397;
String windDirection="";
String windSpeed="";
String humidity="";
String temperature="";
String rainfall="";
String pressure="";
                      
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  if(!wifi.begin(2, 3))
  {
    Serial.println(F("Error talking to shield"));
    while(1);
  }
  Serial.println(F("Start wifi connection"));
  if(!wifi.connectAP(ssid, pass))
  {
    Serial.println(F("Error connecting to WiFi"));
    while(1);
  } 
  Serial.print(F("Connected to "));Serial.println(wifi.SSID());
  Serial.println(F("IP address: "));
  Serial.println(wifi.localIP()); 
  ThingSpeak.begin(client);
  delay(1000);
}

void readWindDirection(){
  windDirection = parseString(ThingSpeak.readStringField(weatherStationChannelNumber,1));
}

void readWindSpeed(){
  windSpeed = parseString(ThingSpeak.readStringField(weatherStationChannelNumber,2));
}

void readHumidity(){
  humidity = parseString(ThingSpeak.readStringField(weatherStationChannelNumber,3));
}

void readTemperature(){
  temperature = parseString(ThingSpeak.readStringField(weatherStationChannelNumber,4));
}

void readRainfall(){
  rainfall = parseString(ThingSpeak.readStringField(weatherStationChannelNumber,5));
}

void readPressure(){
  pressure = parseString(ThingSpeak.readStringField(weatherStationChannelNumber,6));
}

void loop() {
  // put your main code here, to run repeatedly:
  windDirection="";
  windSpeed="";
  humidity="";
  temperature="";
  rainfall="";
  pressure="";
  Serial.println(F("======================================")); 
  Serial.println(F("Current weather conditions in Natick: ")); 
  
  while(windDirection.equals(""))
    readWindDirection();
  while(windSpeed.equals(""))
    readWindSpeed();
  while(humidity.equals(""))
    readHumidity();
  while(temperature.equals(""))
    readTemperature();
  while(rainfall.equals(""))
    readRainfall();
  while(pressure.equals(""))
    readPressure();
  
  Serial.print(temperature);
  Serial.print(F(" degrees F, ")); 
  Serial.print(humidity);
  Serial.println(F("% humidity")); 
  Serial.print(F("Wind at "));
  Serial.print(windSpeed);
  Serial.print(F(" MPH at ")); 
  Serial.print(windDirection);
  Serial.println(F(" degrees")); 
  Serial.print(F("Pressure is "));
  Serial.print(pressure);
  Serial.print(F(" inHg"));
  if(rainfall.toInt() > 0)
    Serial.print(F(", and it's raining!"));
  Serial.println();
  delay(10000); // Note that the weather station only updates once a minute
  
}

//workaround for Cytron WiFi Shield leftover content when client closes
String parseString(String str)
{
  int index = str.indexOf(",CLOSED");
  if(index==-1) return str;
  return str.substring(0,index-1);
}
