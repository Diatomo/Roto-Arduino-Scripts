void setup()
{
  Serial.begin(9600);

  pinMode(14,INPUT);
  pinMode(15,INPUT);

}

void loop(){
	Serial.print(analogRead(18));
	Serial.print(" | ");
	Serial.println(analogRead(15));
	delay(1000);
}
