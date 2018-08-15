



#include <DmxSimple.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(18,17);

void setup(){
	//Serial.begin(9600);
	//mySerial.begin(9600);
	DmxSimple.usePin(17);
	DmxSimple.maxChannel(64);	
	//mySerial.print("did the receive receive this?");
}


void loop(){
	//Serial.println("is this printing");
	//mySerial.read(mySerial.print("is this RECEIVING!!"));
	for ( uint8_t i = 0; i < 30; i++){
		DmxSimple.write(i,255);
	}
	delay(500);
}
