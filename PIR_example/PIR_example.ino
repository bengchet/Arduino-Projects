/*
2. * PIR sensor tester
3. */
 
int ledPin = 6;                // choose the pin for the LED
int inputPin = A1;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
int motor = 4;
int coil = 5;
 
void setup() {
pinMode(ledPin, OUTPUT);      // declare LED as output
pinMode(inputPin, INPUT);     // declare sensor as input
 
Serial.begin(57600);
}
 
void loop(){
  val = digitalRead(inputPin);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    digitalWrite(ledPin, HIGH);  // turn LED ON
    digitalWrite(motor, LOW);
    digitalWrite(coil, LOW);
//    if (pirState == LOW) {
//      // we have just turned on
//      Serial.println("Motion detected!");
//      // We only want to print on the output change, not state
//      pirState = HIGH;
   // }
  } else {
    digitalWrite(ledPin, LOW); // turn LED OFF
    digitalWrite(motor, HIGH);
    digitalWrite(coil, HIGH);
//    if (pirState == HIGH){
//      // we have just turned of
//      Serial.println("Motion ended!");
//      // We only want to print on the output change, not state
//      pirState = LOW;
   // }
  }
}

//int pir = 0;
//void setup() {
//  // put your setup code here, to run once:
//Serial.begin(57600);
//pinMode(A1, INPUT);
//pinMode(6, OUTPUT);
//}
//
//void loop() {
//  // put your main code here, to run repeatedly:
//  pir = LOW;
//  pir = digitalRead(A1);
//  Serial.println(pir);
//  if(pir == HIGH)
// {
//  digitalWrite(6, HIGH);
//  delay(100);
//  }
//  else
//  {
////  digitalWrite(6, HIGH);
////  delay(1000);
//  digitalWrite(6, LOW);
//  delay(100);
////  pir = HIGH;
//  }
//}
