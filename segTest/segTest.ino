

#include <SoftwareSerial.h>

SoftwareSerial mySerial(9,10);
unsigned long baud = 2400;

void setup(){
    Serial.begin(baud);
		mySerial.begin(baud);
		//mySerial.println("Hello,world?");
}


void loop(){
		Serial.write(mySerial.read());
		delay(50);
    //delay(1000);oa
}
