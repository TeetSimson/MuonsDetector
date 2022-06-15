void setup() {           	 
  Serial.begin(115200);
}

int val;

void loop(){
  val = analogRead(14);
  Serial.print("analog 0 is: ");
  Serial.println((((3300 / 1024) * val)-500)/10);
  delay(250);
}
