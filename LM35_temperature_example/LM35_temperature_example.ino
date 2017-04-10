const int Up = 10;
const int Down = 11;
const int Set = 12;

void setup() {
  // put your setup code here, to run once:
Serial.begin(57600);
pinMode(A0, INPUT);
pinMode(Up, INPUT);
pinMode(Down, INPUT);
pinMode(Set, INPUT);
pinMode(13, OUTPUT);
}

void loop()
{
  Serial.println(temperature());
  delay(500);
}

int temperature() {
  // put your main code here, to run repeatedly:
  float temp;
  temp = (float)analogRead(0)*5/1024.0;
  temp = temp - 0.5;
  temp = temp / 0.01;
  //Serial.println(temp);

//  if(temp < 60)
//  {
//    digitalWrite(13, HIGH);
//    delay(200);
//  }
//  else
//  {
//    digitalWrite(13, LOW);
//    delay(200);
//    } 
return temp;
  }


