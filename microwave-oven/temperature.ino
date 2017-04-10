int temperature() {
  // put your main code here, to run repeatedly:
  float temp;
  temp = (float)analogRead(0)*5/1024.0;
  temp = temp - 0.5;
  temp = temp / 0.01;
  
  return (int)temp;
}


