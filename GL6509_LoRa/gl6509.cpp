#include "gl6509.h"

// constructor
gl6509::gl6509(HardwareSerial &hardwareserial, long baud): bufferHead(0)
{
   hardwareserial.begin(baud);
   _serial = &hardwareserial;	
}

gl6509::gl6509(SoftwareSerial &softwareserial, long baud): bufferHead(0)
{
   softwareserial.begin(baud);
   _serial = &softwareserial;
}

// Public Function

// init
bool gl6509::begin()
{
  // do nothing yet
  return test();
}

// get gl6509 firmware version
bool gl6509::test()
{
  sendCommand("", GL6509_NO_VARIABLE); //send AT only
  if(getResponseFromCmd() > 0 && strstr(rxbuffer, "OK")){
    return true;
  }
  else
    return false;
}

String gl6509::getFirmwareVersion()
{
  sendCommand("+SGMR", GL6509_READ);
  int value = getResponseFromCmd();
  if(value > 0){
    // get value from buffer
    String str = (const char*) rxbuffer;
    int idx1 = str.indexOf("+SGMR:\"");
    if(idx1 != -1){
      int idx2 = str.indexOf("\"", idx1 + 7);
      return str.substring(idx1+7, idx2);
    }
    else
      return "VERSION UNKNOWN";
  }
  else 
    return "INVALID RESPONSE";
}

String gl6509:: getManufactureID()
{
  sendCommand("+SGMI", GL6509_READ);
  int value = getResponseFromCmd();
  if(value > 0){
    // get value from buffer
    String str = (const char*) rxbuffer;
    int idx1 = str.indexOf("+SGMI:\"");
    if(idx1 != -1){
      int idx2 = str.indexOf("\"", idx1 + 7);
      return str.substring(idx1+7, idx2);
    }
    else
      return "VERSION UNKNOWN";
  }
  else 
    return "INVALID RESPONSE";
}

String gl6509:: getModuleRevision()
{
  sendCommand("+SLMR", GL6509_READ);
  int value = getResponseFromCmd();
  if(value > 0){
    // get value from buffer
    String str = (const char*) rxbuffer;
    int idx1 = str.indexOf("+SLMR:\"");
    if(idx1 != -1){
      int idx2 = str.indexOf("\"", idx1 + 7);
      return str.substring(idx1+7, idx2);
    }
    else
      return "VERSION UNKNOWN";
  }
  else 
    return "INVALID RESPONSE";
}

String gl6509:: getModelIdentification()
{
  sendCommand("+SGMM", GL6509_READ);
  int value = getResponseFromCmd();
  if(value > 0){
    // get value from buffer
    String str = (const char*) rxbuffer;
    int idx1 = str.indexOf("+SGMM:\"");
    if(idx1 != -1){
      int idx2 = str.indexOf("\"", idx1 + 7);
      return str.substring(idx1+7, idx2);
    }
    else
      return "VERSION UNKNOWN";
  }
  else 
    return "INVALID RESPONSE";
}

String gl6509:: getMacSerialNumber()
{
  sendCommand("+SGMD", GL6509_READ);
  int value = getResponseFromCmd();
  if(value > 0){
    // get value from buffer
    String str = (const char*) rxbuffer;
    int idx1 = str.indexOf("+SGMD:\"");
    if(idx1 != -1){
      int idx2 = str.indexOf("\"", idx1 + 7);
      return str.substring(idx1+7, idx2);
    }
    else
      return "VERSION UNKNOWN";
  }
  else 
    return "INVALID RESPONSE";
}

// Private Function

void gl6509::sendCommand(String cmd, gl6509_command_type type, String params)
{
  _serial->print("AT");
  _serial->print(cmd);
  if(type == GL6509_READ)
    _serial->print("?");
  else if(type == GL6509_EXEC){
    _serial->print("=");
    _serial->print(params);
  }
  else if(type == GL6509_TEST){
    _serial->print("=?");
  }
  _serial->print("\n");

}

void gl6509::sendCommand(String cmd, gl6509_command_type type)
{
  return sendCommand(cmd, type, "");
}

int gl6509::getResponseFromCmd(int timeout)
{
  int _timeout = timeout;
  uint8_t received = 0;

  //clear rxbuffer
  clearRxBuffer();
  
  while(timeout--){

    if(_serial->available()){
      
      received += readByteToRxBuffer();
      
      if(searchRxBuffer("OK")){
        return received;
      }
  
      else if(searchRxBuffer("ERROR")){
        return 0;
      }

      _timeout = timeout;continue;
    }
    
    delay(1);
  }

  Serial.println((const char*)rxbuffer);
  return -1;
  
}

void gl6509::clearRxBuffer()
{
  memset(rxbuffer, '\0', sizeof(rxbuffer));
  bufferHead = 0;
}

int gl6509::readByteToRxBuffer()
{
  
  char c = _serial->read();

  //Do not store null character in buffer
  //if(c == 0) return 0;
  
  rxbuffer[bufferHead] = c;

  bufferHead = (bufferHead + 1) % GL6509_BUFFER_MAX_SIZE;
  
  return 1;
}

char* gl6509::searchRxBuffer(const char *keyword)
{
  return strstr((const char *)rxbuffer, keyword);
}
