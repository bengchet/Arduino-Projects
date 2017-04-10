#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FS.h>

WiFiUDP listener;
WiFiClient cl;
IPAddress ip(192, 168, 1, 242);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

bool loadConfig()
{
  // open file for reading.
  File configFile = SPIFFS.open("/cl_conf.txt", "r");
  if (!configFile)
  {
    Serial.println("Failed to open cl_conf.txt.");

    return false;
  }

  // Read content from config file.
  String content = configFile.readString();
  configFile.close();
  
  content.trim();

  // Check if ther is a second line available.
  uint8_t start_pos = 0;
  uint8_t end_pos = 0;
  
  while(1)
  {
    String ssid, pass = "";
    end_pos = content.indexOf("#", start_pos);
    if (end_pos >0)
    {
      ssid = content.substring(start_pos, end_pos);
    }
    
    start_pos = end_pos + 1;
    end_pos = content.indexOf("@", start_pos);
    if (end_pos >0)
    {
      pass = content.substring(start_pos, end_pos);
    }
    start_pos = end_pos + 1;

    Serial.println(ssid);Serial.println(pass);
    WiFi.begin(ssid.c_str(), pass.c_str());
    
    int i = 30;
    while (i--&&WiFi.status()!= WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      if(WiFi.status() == WL_CONNECTED) return true;
    }
  }
} // loadConfig

bool saveConfig(String ssid, String pass)
{
  // Open config file for writing.
  File configFile = SPIFFS.open("/cl_conf.txt", "a");
  if (!configFile)
  {
    Serial.println("Failed to open cl_conf.txt for writing");
    return false;
  }

  // Save SSID and PSK.
  configFile.print(ssid);configFile.print("#");
  configFile.print(pass);configFile.print("@");

  configFile.close();
  
  return true;
} // saveConfig

void updateSketch(int cmd, int size)
{
    if(!Update.begin(size, cmd)){
      //Serial.println("Update Begin Error");
      return;
    }
    cl.print("ready");
         
    uint32_t written = 0;

    while(!Update.isFinished()){
      written += Update.write(cl);
      if(written>0) cl.print("#");
    }
    //Serial.setDebugOutput(false);
    
    if(Update.end()&&written==size){
      cl.print("O");
      //Serial.printf("Update Success: %u\nRebooting...\n", millis() - startTime);
      ESP.restart();
    } else {
      cl.print("X");
      Update.printError(cl);
      Update.printError(Serial);
    }
}

void stk500()
{
    cl.print("ready");  
    digitalWrite(4, LOW); //reset arduino
    delay(10);
    digitalWrite(4, HIGH);
    delay(500);
    int i = 500;

    while(!Serial.available()&&--i)
    {
      Serial.write(0x30);
      Serial.write(0x20);
//      Serial.write(0x30);
//      Serial.write(0x20);
    }
    delay(10);
    while(Serial.available()) Serial.read();
    
    Serial.write(0x41);
    Serial.write(0x81);
    Serial.write(0x20);
    Serial.write(0x41);
    Serial.write(0x82);
    Serial.write(0x20);
    Serial.write(0x50);
    Serial.write(0x20);
    Serial.write(0x75);
    Serial.write(0x20);
    
    delay(10);
    while(Serial.available()) Serial.read();
    delay(500);

    int written = 0;
    int current_page = 0;
    bool skip = false;
    
    while(cl.available()>128)
    {
      int index = 128;
      int checksum = 0;
      Serial.write(0x55);
      Serial.write((current_page*64)%256);
      Serial.write((current_page*64)/256);
      Serial.write(0x20);
      Serial.write(0x64);
      Serial.write(0x00);
      Serial.write(0x80);
      Serial.write(0x46);
      while(index!=0)
      {
        //digitalWrite(2, LOW);
        Serial.write(cl.read());
        written+=1;index-=1;
        //digitalWrite(2, HIGH);
      }
      Serial.write(0x20);
      while(!Serial.available())
        delay(1);
      //delay(50);
      while(Serial.available())
      {
        Serial.read();checksum+=1;delay(10);
      }
      if(checksum!=4)
      {
        skip = true;cl.print("fail");break;
      }
      cl.print(current_page);//delay(200);
      current_page++;
    }

    if(cl.available()&&!skip)
    {
      Serial.write(0x55);
      Serial.write((current_page*64)%256);
      Serial.write((current_page*64)/256);
      Serial.write(0x20);
      Serial.write(0x64);
      Serial.write(cl.available()/256);
      Serial.write(cl.available()%256);
      Serial.write(0x20);
      while(cl.available())
      {
        //digitalWrite(2, LOW);
        Serial.write(cl.read());
        written+=1;
        //digitalWrite(2, HIGH);
      }
      Serial.write(0x20);
      cl.print(current_page);
    }
    
    delay(200);
    Serial.write(0x51);
    Serial.write(0x20);
    delay(20);
    if(Serial.available()==6) cl.print("end");
    else cl.print("fail");
      
    digitalWrite(4, LOW); //reset arduino
    delay(10);
    digitalWrite(4, HIGH);

    while(Serial.available()) 
      Serial.read();

    cl.print("size uploaded: ");cl.print(written);
    cl.print("end");
    cl.stop();
}

void blink()
{
  int i = 6;
  while(i--)
  {
    digitalWrite(2, LOW);
    delay(200);
    digitalWrite(2, HIGH);
    delay(200);
  }
}

void setup() {
  Serial.begin(115200);
  //Serial.setDebugOutput(false);
  Serial.println("");
  Serial.println("OTA test");
  pinMode(2, OUTPUT);digitalWrite(2, HIGH);
  pinMode(4, OUTPUT);digitalWrite(4, HIGH);
  WiFi.disconnect();

  // Initialize file system.
  if (!SPIFFS.begin())
  {
    Serial.println("Failed to mount file system");
    return;
  }

  //saveConfig(your ssid, your password);
  //while(1);
  
  loadConfig();
  WiFi.config(ip, gateway, subnet);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  listener.begin(8266);

  //Serial.print("Sketch size: ");
  //Serial.println(ESP.getSketchSize());
  //Serial.print("Free size: ");
  //Serial.println(ESP.getFreeSketchSpace());
}

void loop() {
  
  digitalWrite(2, LOW);
  if (listener.parsePacket()) {
    IPAddress remote = listener.remoteIP();
    int cmd  = listener.parseInt();
    int port = listener.parseInt();
    int sz   = listener.parseInt();
    //Serial.println("Got packet");
    //Serial.printf("%d %d %d\r\n", cmd, port, sz);
    
    if (!cl.connect(remote, port)) {
      //Serial.println("failed to connect");
      return;
    }
    
    WiFiUDP::stopAll();
  
    if(cmd==0) updateSketch(cmd, sz);
    else if(cmd==2) stk500();

    blink();
    delay(100);
    listener.begin(8266);
  }

  delay(100);
}

