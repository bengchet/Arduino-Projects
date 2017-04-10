int a = 0;

void blink1()
{
  for(int i=0;i<6;i++)
  {
      digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  delay(200);
  }
}

void blink2()
{
  for(int i=0;i<6;i++)
  {
      digitalWrite(13, HIGH);
  delay(2000);
  digitalWrite(13, LOW);
  delay(1000);
  }
}

void do3()
{
  Serial.println("Gotcha!!");
}

void (*menu_funcs[3])() = {blink1,blink2,do3};

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  (*menu_funcs[1])();
  run_func(do3);
}

void run_func(void (&func)())// this part is useful when developing library
{
  func();
}


