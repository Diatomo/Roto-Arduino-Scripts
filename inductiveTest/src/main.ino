

void setup(){
	Serial.begin(9600);
	pinMode(4, INPUT);
}

void loop(){
	Serial.println(digitalRead(4));

}
