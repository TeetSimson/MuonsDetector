void setup() {           	 
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(14), pin14Pulse, RISING);
}

double val;

void loop()  {
}


void pin14Pulse() {
  val = analogRead(14);
  Serial.print("analog 0 is: ");
  Serial.println(val*(9.0/1023.0)); //change 9 to what voltage we will use
}
