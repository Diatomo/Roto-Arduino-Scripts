
#include <Arduino.h>
#include <outputExtend.h>
#include "cRelay.h"


CRelay::CRelay(){

		//pins oeRelay
		uint8_t const oeData = 7;
		uint8_t const oeClk = 6;
		uint8_t const oeLtch = 5;
		uint8_t const nOE = 1;
		
		
		action = false;
		currDevice = 0;
		resetTimer = 0;
		audio = 5;
		optoCopter = 6;

		oe = new outputExtend(oeData, oeClk, oeLtch, nOE);
		
		uint8_t address = 1;
		uint8_t const nDisplays = 3;
		uint8_t serialPin = 8;
		//dig = Digits(serialPin);
		for (uint8_t i = 0; i < nDevices; i++){
			deviceDisplays[i] = dig.addGroup(address, nDisplays);
			address += nDisplays + 1; 
		}
}

void CRelay::update(uint8_t action){
		Serial.println("relay ACTION!!");
		uint8_t command = (action & 0x10) >> 4; //either a 0 or a 1;
		uint8_t button = action & 0x0F; //represents an output pin
		test(command, button);
		if (button != optoCopter || button != audio){
			currDevice = button;
		}
		deviceDisplays[currDevice]->segDisp(999, false);
		resetTimer = millis();
		oe->extendedWrite(button, command);
}

void CRelay::loop(){
	uint32_t currTimer = millis();
	Serial.println("loop");
	if (resetTimer != 0){
		if ((currTimer - resetTimer) > resetTime){
			Serial.println(" RESET ");
			oe->extendedWrite(currDevice, LOW);
			deviceDisplays[currDevice]->segDisp(111, false);
			resetTimer = 0;
		}
	}
}

void CRelay::test(uint8_t command, uint8_t button){
	Serial.print("command :: ");
	Serial.println(command);
	Serial.print("Button :: ");
	Serial.println(button);

}
