#ifndef __GL6509__
#define __GL6509__

#include <Arduino.h>
#include <SoftwareSerial.h>

#define GL6509_BUFFER_MAX_SIZE 120
#define GL6509_RESP_TIMEOUT 5000

class gl6509
{
  public:
    gl6509(HardwareSerial &hardwareserial, long baud=9600);
    gl6509(SoftwareSerial &softwareserial, long baud=9600);
    bool begin();
    String getFirmwareVersion();
    String getManufactureID();
    String getModuleRevision();
    String getModelIdentification();
    String getMacSerialNumber();
    
  protected:
    Stream* _serial;
    
  private:
    typedef enum gl6509_command_type{
     GL6509_NO_VARIABLE,
     GL6509_READ,
     GL6509_TEST,
     GL6509_EXEC
    };
    
    uint8_t rxbuffer[GL6509_BUFFER_MAX_SIZE];
    uint8_t bufferHead = 0;
    void sendCommand(String cmd, gl6509_command_type type=GL6509_NO_VARIABLE);
    void sendCommand(String cmd, gl6509_command_type type, String params);
    int getResponseFromCmd(int timeout=GL6509_RESP_TIMEOUT);
    void clearRxBuffer();
    int readByteToRxBuffer();
    char* searchRxBuffer(const char *keyword);

    //command
    bool test();
};

#endif
