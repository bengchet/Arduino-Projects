#include <SoftwareSerial.h>
#define average 20

SoftwareSerial mySerial(2,3); //pin 3 transmit
int thres []= // col - a,b,c,d; row - bend most, bend less, straight
{
  150,210,200,200,
  210,250,250,250,
  260,330,320,330 
};

int reading[4] = {0,0,0,0};
int output[4] = {0,0,0,0};
int p_output[4] = {0,0,0,0};
//int a, pa = 0;
//int b, pb= 0;
//int c, pc = 0;
//int d, pd = 0;

void setup() {
  // put your setup code here, to run once:
  mySerial.begin(9600);
  pinMode(12, INPUT_PULLUP);
  pinMode(A0, INPUT);
  pinMode(A2, INPUT);
  pinMode(A4, INPUT);
  pinMode(A6, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  while(digitalRead(12));
  while(!digitalRead(12));
  mySerial.println("Start");
  digitalWrite(13, HIGH);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  //int i;
  int _delay = 500;
  for(int i = 0;i<average;i++)
  {
    reading[0] += analogRead(A0);
    reading[1] += analogRead(A2);
    reading[2] += analogRead(A4);
    reading[3] += analogRead(A6);
  }
    
  reading[0] /= average;
  reading[1] /= average;
  reading[2] /= average;
  reading[3] /= average;
  
  for(int j = 0;j<4;j++)
  {
    int i;bool minimum = false;
    output[j] = 2;
    for(i = 0;i<3&&!minimum;i++)
    {
      if(reading[j]<thres[i*4+j])
      {
        output[j] = i;
        minimum = true;
      }
    }
  }
  
//  for(i = 0;i<3;i++)
//  {
//    if(a<thres[i*4+0])
//      break; 
//  }
//  if(i>2) i=2;
//  a = i;
//  
//  for(i = 0;i<3;i++)
//  {
//    if(b<thres[i*4+1])
//      break;
//  }
//  if(i>2) i=2;
//  b = i;
//
//  for(i = 0;i<3;i++)
//  {
//    if(c<thres[i*4+2])    
//      break;
//  }
//  if(i>2) i=2;
//  c = i;
//
//  for(i = 0;i<3;i++)
//  {
//    if(d<thres[i*4+3])
//      break;
//  }
//  if(i>2) i=2;
//  d = i;

  for(int j = 0;j<4;j++)
  {
    if(output[j]!=p_output[j])
    {
      bluetoothPress(output[j]*4+j);
      _delay = 800;
      p_output[j]= output[j];
      delay(10);
    }
  }
  
//  if(a!=pa)
//  {
//    bluetoothPress(a*4+0);delay(10);
//    _delay = 800;
//    pa = a;
//  }
//  
//  if(b!=pb)
//  {
//    bluetoothPress(b*4+1);delay(10);
//    _delay = 800;
//    pb = b;
//  }
//  
//  if(c!=pc)
//  {
//    bluetoothPress(c*4+2);delay(10);
//    _delay = 800;
//    pc = c;
//  }
//
//  if(d!=pd)
//  {
//    bluetoothPress(d*4+3);delay(10);
//    _delay = 800;
//    pd = d;
//  }
  
//  mySerial.print(a);mySerial.print(",");
//  mySerial.print(b);mySerial.print(",");
//  mySerial.print(c);mySerial.print(",");
//  mySerial.println(d);
//
  delay(_delay);
}

void bluetoothPress(int num)
{
  Serial.write(0xff);Serial.write(0xff);
  switch(num)
  {
    case 0:Serial.print("xup");break;
    case 4:Serial.print("xdp");break;
    case 8:Serial.print("xlp");break;

    case 1:Serial.print("xrp");break;
    case 5:Serial.print("cup");break;
    case 9:Serial.print("cdp");break;

    case 2:Serial.print("clp");break;
    case 6:Serial.print("crp");break;
    case 10:Serial.print("b1p");break;

    case 3:Serial.print("b2p");break;
    case 7:Serial.print("b3p");break;
    case 11:Serial.print("b4p");break;
  }
}

void bluetoothRelease(int num)
{
  Serial.write(0xff);Serial.write(0xff);
  switch(num)
  {
    case 0:Serial.print("xur");break;
    case 4:Serial.print("xdr");break;
    case 8:Serial.print("xlr");break;

    case 1:Serial.print("xrr");break;
    case 5:Serial.print("cur");break;
    case 9:Serial.print("cdr");break;

    case 2:Serial.print("clr");break;
    case 6:Serial.print("crr");break;
    case 10:Serial.print("b1r");break;

    case 3:Serial.print("b2r");break;
    case 7:Serial.print("b3r");break;
    case 11:Serial.print("b4r");break;
  }
}


