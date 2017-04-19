#include <SoftwareSerial.h>
#include "gl6509.h"

#define btn 8
#define debugSerial Serial

SoftwareSerial mySerial(2, 3); // RX, TX
//#define mySerial Serial5
gl6509 lora(mySerial);
char prevBtn = 0;
int32_t timeout = 0;

void setup() {

  pinMode(btn, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Open serial communications and wait for port to open:
  debugSerial.begin(9600);
  while (!debugSerial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //init gl6509
  if (!lora.begin()) {
    debugSerial.println(F("GL6509 init failed"));
    while (1);
  }

  debugSerial.println(F("\n#################################"));
  debugSerial.println(F("## GL6509 LPWAN Module Testing ##"));
  debugSerial.println(F("#################################\n"));
  
  debugSerial.println(F("Listing Module Specification"));
  debugSerial.println(F("----------------------------"));
  
  // Version
  debugSerial.print(F("Version: "));
  debugSerial.println(lora.getFirmwareVersion());

  // Revision of LoRa Module
  debugSerial.print(F("Module revision: "));
  debugSerial.println(lora.getModuleRevision());

  // Manufacture ID
  debugSerial.print(F("Manufacture ID: "));
  debugSerial.println(lora.getManufactureID());

  debugSerial.print(F("Model Identification: "));
  debugSerial.println(lora.getModelIdentification());

  debugSerial.print(F("MAC and Serial number of LMU: "));
  debugSerial.println(lora.getMacSerialNumber());

  timeout = millis();
}

void loop() { // run over and over

  // read the button status
  char currentBtn = digitalRead(btn);

  if (currentBtn != prevBtn) {

    if (currentBtn == HIGH) { //button pressed
      //light up LED
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else {
      //dim the LED
      digitalWrite(LED_BUILTIN, LOW);
    }

    // update the prevBtn status
    prevBtn = currentBtn;

  }

}



