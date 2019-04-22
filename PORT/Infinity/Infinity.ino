
/*
   Roto
 
 Project:PORT DISCOVERY
 Exhibit: CARGO See && Touch (Infinity Mirror)
 Author(s): Charles Stevenson
 Date: 9/24/18
 
 Original environment:
 Arduino 1.8.2
 
 Revision notes:
 
 */


#include <outputExtend.h>

#define versionString "Roto - [PORT DISCOVERY] [CARGO See && Touch (Infinity Mirror)]"


//ledPins


//Cap Sensor Pin
uint8_t const capSensor = 4;
bool capState = false;

//board setup
uint8_t const nOE = 1;
uint8_t const oeData = 13;
uint8_t const oeClock = 12;
uint8_t const oeLatch = 11;
outputExtend oe = outputExtend(oeData, oeClock, oeLatch, nOE); 

void setupOE(){
	oe.extendedWrite(HIGH);
	delay(1000);
	oe.extendedWrite(LOW);
}

void setupTouch(){
	pinMode(capSensor, INPUT);
}


void setup(){
	uint32_t const BAUD = 9600;
	Serial.begin(BAUD);
	
    //Open Serial && write version
    Serial.begin(BAUD_RATE);
    Serial.println(versionString);
    Serial.print(F("File: "));
    Serial.println(__FILE__);

    //setupBoards
	setupOE();
	setupTouch();
}

void keyUp(){
	if(capState == true){
		oe.extendedWrite(LOW);
		capState = false;
	}
}

void keyDown(){
	if (capState == false){
		oe.extendedWrite(HIGH);
		capState = true;
	}
}

void buttonHandler(){
	if (digitalRead(capSensor)){
		keyDown();
	}
	else{
		keyUp();
	}
}


void loop(){
	buttonHandler();
}
