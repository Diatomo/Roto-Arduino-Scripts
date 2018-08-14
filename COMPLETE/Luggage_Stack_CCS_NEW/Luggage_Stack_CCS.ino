/*
 * Roto
 *
 * Project:
 * CR Smith
 *
 *
 * Exhibit:
 * Luggage Stack
 *
 *
 * Author(s):
 * Charles Stevenson
 *
 *
 *
 * Original environment:
 * Arduino 1.0.5 - Roto Libraries xxxx.xx.xx
 *
 * Revision notes:
 *
 Maa
 *
 *
 *
 */


#define versionString "Roto - [CR Smith] [Luggage Stack]"

// Debug Mode (comment to disable)
//#define debugMode

// Libraries:
#include "outputExtend.h"

// Pin Assignments:
const byte dataPin = 13;
const byte clockPin = 12;
const byte latchPin = 11;

//Switches
bool topLeft = true;
bool topRight = true;

const char OUTPUTA = 'b';
const char OUTPUTB = 't';
const char OUTPUTC = 'B';
const char OUTPUTD = 'T';
const char OUTPUTE = 'o';


//light panels
int Lpanel = 0;
int Rpanel = 2;

// Constants:
const byte numberOfBoards = 1;

// Global Variables:

// Global Classes:
outputExtend oe = outputExtend(dataPin, clockPin, latchPin, numberOfBoards);

void setup(){
	Serial.begin(9600);
	Serial.println(versionString);
	Serial.print("File: ");
	Serial.println(__FILE__);

	for (int i = 0; i < 4; i++){
		oe.extendedWrite(i,HIGH);
	}
	delay(2000);
	for (int i = 0; i < 4; i++){
		oe.extendedWrite(i,LOW);
	}
}


void flipLights(bool topOn,int A){
	oe.extendedWrite(A, topOn);
	oe.extendedWrite(A+1, !topOn);
}

void turnOffLights(){
	oe.extendedWrite(Lpanel, LOW);
	oe.extendedWrite(Lpanel+1, LOW);
	oe.extendedWrite(Rpanel, LOW);
	oe.extendedWrite(Rpanel+1, LOW);
}

void reset(){
	topLeft = true;
	topRight = true;
}

void loop(){
	char command = Serial.read();
	if (command == OUTPUTA  || command == OUTPUTB){
		topLeft = !topLeft;
		flipLights(topLeft, Lpanel);
	}
	else if (command == OUTPUTC || command == OUTPUTD){
		topRight = !topRight;
		flipLights(topRight, Rpanel);
	}
	else if (command == OUTPUTE){
		turnOffLights();
		reset();
	}
}
