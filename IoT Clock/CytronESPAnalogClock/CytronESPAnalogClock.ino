/*
 * Time_NTP.pde
 * Example showing time sync to NTP time source
 *
 * This sketch uses the ESP8266WiFi library
 */
#include <Adafruit_NeoPixel.h>
#include <TimeLib.h> 
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// define pins
#define NEOPIN 15

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEOPIN, NEO_GRB + NEO_KHZ800);

const char *ssid[3] = {"Cytron-Asus", "Cytron-Asus1", "Cytron-R&D"};  //  your network SSID (name)
const char pass[] = "f5f4f3f2f1";       // your network password

// NTP Servers:
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov


const int timeZone = 8;     // Central European Time

#define BRIGHTNESS 255 // set max brightness

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

time_t prevDisplay = 0; // when the digital clock was displayed

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
  strip.setPixelColor(i, c);
  strip.show();
  delay(wait);
  }
}

void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void digitalClockDisplay(){
  // digital clock display of the time
  int _hour = hour();
  int _minute = minute();
  int _second = second();
  
  Serial.print(_hour);
  printDigits(_minute);
  printDigits(_second);
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year()); 
  Serial.println(); 

  if(_hour>11){
    _hour -=12;
  }
  _hour = (_hour*60 + _minute) / 12;  //each red dot represent 24 minutes.

  // arc mode
  for(uint8_t i=0; i<strip.numPixels(); i++) {
    //clear all pixels
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  //light up hour needle
  strip.setPixelColor(_hour, strip.Color(255, 0, 0));
  //strip.setPixelColor((_hour-1<0)?59:_hour-1, strip.Color(255, 0, 0));
  strip.setPixelColor((_hour+1>59)?1:_hour+1, strip.Color(255, 0, 0));

  //light up minute needle
  strip.setPixelColor(_minute, strip.Color(0, 255, 0));
  //trip.setPixelColor((_minute+1>59)?1:_minute+1, strip.Color(0, 255, 0));
  
  //light up second needle
  strip.setPixelColor(_second, strip.Color(0, 0, 255));

  //display
  strip.show();
 
  // wait
  delay(100);
  
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

void setup() 
{
  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only
  delay(250);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
 
  strip.setBrightness(BRIGHTNESS); // set brightness
 
  // startup sequence
  delay(500);
  colorWipe(strip.Color(255, 0, 0), 20); // Red
  colorWipe(strip.Color(0, 255, 0), 20); // Green
  colorWipe(strip.Color(0, 0, 255), 20); // Blue
  delay(500);
  
  Serial.println("TimeNTP Example");
  Serial.print("Connecting to ");

  uint8_t num = 0;
  WiFi.begin(ssid[num%3], pass);
  Serial.println(ssid[num]);

  int retry = 60;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(--retry == 0){
      num = (num + 1)%3;
      WiFi.begin(ssid[num], pass);
      Serial.print("Connecting to ");
      Serial.println(ssid[num]);
      retry = 60;
    }
  }

  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
 
}

void loop()
{  
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();  
    }
  }
}
