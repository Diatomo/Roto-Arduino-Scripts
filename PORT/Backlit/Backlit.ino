

/*
   Roto

 Project: Cargo See && Touch (Backlit)
 Exhibit: 
 Author(s): Charles Stevenson
 Date: 9/28/2018

 Original environment:
 Arduino 1.8.2

 Revision notes:

 */


#include <buttonBoard.h>

#define versionString "Roto - [PORT] [Carge See && Touch (Backlit)]"


//Button Board
const uint8_t bbDI = 12; //button board data in
const uint8_t bbDO = 11; //button board data out
const uint8_t bbCLK = 10; //button board clock
const uint8_t bbILT = 9; //IN LATCH
const uint8_t bbOLT = 8; //OUT LATCH
const uint8_t nBB = 1; //number of button boards
const uint8_t nBUTTONS = 3; // number of buttons
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, nBB);//init bb

const uint8_t panelOffset = 3;
bool buttState[nBUTTONS] = [false,false,false];


void setupBB(){
	bb.setLamp(HIGH);
	delay(2000);
	bb.setLamp(LOW);
	for (uint8_t button = 0; button < nBUTTONS; button++){
		bb.setLamp(button, HIGH);
	}
}

void setup(){
    
	uint32_t const BAUD = 9600;
	//Open Serial && write version
    Serial.begin(BAUD);
    Serial.println(versionString);
    Serial.print(F("File: "));
    Serial.println(__FILE__);

	//setupBoards
	setupBB();
}


/*
*
*	FxN :: keyUp
*		handles keyup events
*
*/
void keyUp(uint8_t button, uint8_t panel){
	if(buttState[button] == true){
		bb.setLamp(button, !buttState[button]);
		bb.setLamp(panel, buttState[button]);
		buttState[button] = false;
	}
}

/*
*
*	FxN :: keyDown
*		handles keydown events
*
*/
void keyDown(uint8_t button, uint8_t panel){
	if (buttState[button] == false){
		bb.setLamp(button, !buttState[button]);
		bb.setLamp(panel, buttState[button]);
		buttState[button] = true;
	}
}

/*
*
*	FxN :: buttonHandler
*		handles any key events
*
*/
void buttonHandler(){
	for (uint8_t button = 0; button < nBUTTONS; button++){
		if (bb.getButton(button)){
			keyDown(button, button+panelOffset);
		}
		else{
			keyUp(button, button+panelOffset);
		}
	}
}

/*
*	FxN :: loop
*/
void loop(){
	buttonHandler();
}
