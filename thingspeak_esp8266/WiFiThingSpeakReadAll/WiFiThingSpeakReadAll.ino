#include <CytronWiFiShield.h>
#include <CytronWiFiClient.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

const char *ssid = "Cytron-Asus";
const char *pass = "f5f4f3f2f1";
unsigned long weatherStationChannelNumber = 12397;

void setup() {
  
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
 
  if(!wifi.begin(Serial2))
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
 
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  float windDirection = readField(weatherStationChannelNumber, 1);
  float windSpeed = readField(weatherStationChannelNumber, 2);
  float humidity = readField(weatherStationChannelNumber, 3);
  float temperature = readField(weatherStationChannelNumber, 4);
  float rainfall = readField(weatherStationChannelNumber, 5);
  float pressure = readField(weatherStationChannelNumber, 6);

  Serial.println(F("======================================")); 
  Serial.println(F("Current weather conditions in Natick: ")); 
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
  if(rainfall > 0)
    Serial.print(F(", and it's raining!"));
  Serial.println();
  delay(10000); // Note that the weather station only updates once a minute
  */

  float data[6];
  int size = sizeof(data)/sizeof(float);
  readAll(weatherStationChannelNumber, data ,size);

  float windDirection = data[0];
  float windSpeed = data[1];
  float humidity = data[2];
  float temperature = data[3];
  float rainfall = data[4];
  float pressure = data[5];
  
  Serial.println(F("======================================")); 
  Serial.println(F("Current weather conditions in Natick: ")); 
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
  if(rainfall > 0)
    Serial.print(F(", and it's raining!"));
  Serial.println();
  delay(10000); // Note that the weather station only updates once a minute
}

void readAll(unsigned long channelID, float* dat, int datSize){
  StaticJsonBuffer<280> jsonBuffer;
  String recv="";
  
  const char destServer[] = "api.thingspeak.com";
  ESP8266Client client;
  if (!client.secure_connect(destServer, 443))
  {
    Serial.println(F("Failed to connect to server."));
    client.stop();
    return;
  }
 
  String url = "GET /channels/";
  url += (String)channelID;
  url += "/feeds/last.json HTTP/1.1\r\nHost: api.thingspeak.com\r\nConnection: close\r\n\r\n";
  if(!client.print(url))
  {
    Serial.println(F("Sending failed"));
    client.stop();
    return;
  }

  // set timeout approximately 5s for server reply
  int i=5000;
  while (client.available()<=0&&i--)
  {
    delay(1);
    if(i==1) {
      Serial.println(F("Timeout"));
      return;
      }
  }
  //skip throught content
  client.find(",\"field1\"");
  recv ="{\"field1\"";
  //get the json
  recv += client.readStringUntil('}');
  recv +="}";
  while (client.available()>0)
    client.read();
  
  client.stop();

  //Serial.println(recv);
  
  JsonObject& root = jsonBuffer.parseObject(recv);

  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  for(int i=0;i<datSize;i++){
     String cat = "field" + (String)(i+1);
     dat[i] = root[cat];
     //Serial.println(dat[i]);
  }
}

float readField(unsigned long channelID, int fieldID)
{
  StaticJsonBuffer<200> jsonBuffer;
  String recv="";
  
  const char destServer[] = "api.thingspeak.com";
  ESP8266Client client;
  if (!client.connect(destServer, 80))
  {
    Serial.println(F("Failed to connect to server."));
    client.stop();
    return -1;
  }
  String cat = "field" + (String)fieldID;
  String url = "GET /channels/";
  url += (String)channelID;
  url += "/fields/";
  url += (String)fieldID;
  url += "/last.json HTTP/1.1\r\nHost: api.thingspeak.com\r\nConnection: close\r\n\r\n";
  if(!client.print(url))
  {
    Serial.println(F("Sending failed"));
    client.stop();
    return -1;
  }

  // set timeout approximately 5s for server reply
  int i=5000;
  while (client.available()<=0&&i--)
  {
    delay(1);
    if(i==1) {
      Serial.println(F("Timeout"));
      return -1;
      }
  }
  //skip throught content
  client.find("{");
  recv ="{";
  //get the json
  recv += client.readStringUntil('}');
  recv +="}";
  while (client.available()>0)
    client.read();
  
  client.stop();

  //Serial.println(recv);
  
  JsonObject& root = jsonBuffer.parseObject(recv);

  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return -1;
  }

  float value = root[cat];
  return value;
}

