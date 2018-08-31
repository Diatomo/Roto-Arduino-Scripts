
/*
   Roto

 Project: High Museum of Art
 Exhibit: Daydream Builder
 Author(s): Charles Stevenson
 Date: 8/13/2018

 Original environment:
 Arduino 1.8.2

 Revision notes:
 */


#include <alarmClock.h>
#include <buttonBoard.h>
#include <outputExtend.h>
#include <MIDI.h>

boolean buttonDown = false;

//BUTTON BOARD Config
const uint8_t bbDI = 12; //button board data in
const uint8_t bbDO = 11; //button board data out
const uint8_t bbCLK = 10; //button board clock
const uint8_t bbILT = 9; //IN LATCH
const uint8_t bbOLT = 8; //OUT LATCH
const uint8_t nBB = 2; //number of button boards
const uint8_t nBUTTONS = 12; // number of buttons
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, nBB);//init bb

//Output Extend Config
const uint8_t oeData = 4;
const uint8_t oeClock = 5;
const uint8_t oeLatch = 6;
const uint8_t nOE = 1;
outputExtend oe = outputExtend(oeData, oeClock, oeLatch, nOE);

//alarmClock Config
alarmClock lockout =  alarmClock();
uint32_t lockoutTmr = 1000;

bool states[nBUTTONS] = {false,false,false,false,false,false,false,false,false,false,false,false};

//MIDI
// Created and binds the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();
uint8_t const nSounds = 8;


int8_t activeButt = -1;

void testButtons(){
	bb.setLamp(HIGH);
	delay(1000);
	bb.setLamp(LOW);
}

/*
*
*	FxN :: setup();
*
*/
void setup(){

  uint32_t BAUD_RATE = 9600;
  while (!Serial);
  Serial.begin(BAUD_RATE);
  testButtons();
  MIDI.begin();
}



//Button Handler
/*=========================================================*/
bool keyDown(bool currButt, uint8_t button){
	bool temp = false;
	if (currButt && !buttonDown){
		temp = true;
		buttonDown = true;//global var (probably don't need this but I know it works)   
		activeButt = button;
	}
	return temp;
}

bool keyUp(bool currButt, uint8_t button){
	bool temp = false;
	if(activeButt != -1 && !bb.getButton(activeButt) && buttonDown){
		temp = true;
		buttonDown = false;
		activeButt == -1;
	}
	return temp;
}
/*
*
*	FxN :: buttonHandler
*		Handles Button Presses
*/
bool buttonHandler(uint8_t button){
	bool currButt = bb.getButton(button);
	if (keyDown(currButt, button)){ //if key down set lamp etc..
		states[button] = !states[button];
		bb.setLamp(button, states[button]);
		playMedia(button);
	}
	keyUp(currButt, button);
}
/*=========================================================*/


void playVideo(uint8_t video){
	oe.extendedWrite(video, HIGH);
	delay(100);
	oe.extendedWrite(video,LOW);
}

void playSound(uint8_t audio){
	uint8_t velocity = 0;
	if (states[audio]){
	uint8_t velocity = 127;
	}
	uint8_t channel = 1;
	MIDI.sendNoteOn(audio,velocity,channel);
}

bool playMedia(uint8_t button){
	lockout.setAlarm(lockoutTmr);
	if (button > nSounds){//play video
		playVideo(button - nSounds);
	}
	else{//play sound
		playSound(button);
	}
}


/*
*
* FxN :: alarmClockRoutines
*   processes states and polling
*   
*   DO NOT TOUCH THIS FUNCTION
*
*
*/
void alarmClockRoutine(){
	if (lockout.isSet()) { lockout.poll(); }
}

/*
*
*	FxN :: loop
*		just calls the buttonHandler;
*/
void loop(){
	alarmClockRoutine();
	for (uint8_t button = 0; button < nBUTTONS; button++){
		buttonHandler(button);
	}
}
