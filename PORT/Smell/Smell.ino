/*
   Roto

 Project: Cargo See && Touch (Smell)
 Exhibit: Smell
 Author(s): Charles Stevenson
 Date: 9/28/2018

 Original environment:
 Arduino 1.8.2

 Revision notes:

 */


#include <alarmClock.h>
#include <buttonBoard.h>

#define versionString "Roto - [PORT] [Carge See && Touch (Smell)]"


//Button Board
const uint8_t bbDI = 8; //button board data in
const uint8_t bbDO = 9; //button board data out
const uint8_t bbCLK = 10; //button board clock
const uint8_t bbILT = 11; //IN LATCH
const uint8_t bbOLT = 12; //OUT LATCH
const uint8_t nBB = 1; //number of button boards
const uint8_t nBUTTONS = 3; // number of buttons
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, nBB);//init bb

const uint8_t panelOffset = 3;

//prototypes
void resetFanOne();
void resetFanTwo();
void resetFanThree();

//Constants
uint8_t const NUM_STATES = 3;
uint8_t const NUM_CLOCKS = 3;

//init clocks
alarmClock fanOneClk = alarmClock(resetFanOne);
alarmClock fanTwoClk = alarmClock(resetFanTwo);
alarmClock fanThreeClk = alarmClock(resetFanThree);

//states
bool fanOneState = false;
bool fanTwoState = true;
bool fanThreeState = false;
bool* states[NUM_STATES] = {&fanOneState, &fanTwoState, &fanThreeState};

bool lockOne = false;
bool lockTwo = false;
bool lockThree = false;
bool* locks[NUM_STATES] = {&lockOne, &lockTwo, &lockThree};

uint32_t const fanTimr = 2000;
//alarmclocks and timers
alarmClock clocks[NUM_CLOCKS] = {fanOneClk, fanTwoClk, fanThreeClk};
uint32_t const timers[NUM_CLOCKS] = {fanTimr, fanTimr, fanTimr};


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
* FxN :: alarmClockRoutines
*   processes states and polling
*   
*   DO NOT TOUCH THIS FUNCTION
*
*
*/
void alarmClockRoutine(){
    for (byte i = 0; i < NUM_CLOCKS; i++){
        if (clocks[i].isSet()){//if clock is set
            if (*(states[i])){//if corresponding state is true
                clocks[i].poll();//poll
            }
            else{
                clocks[i].unSetAlarm();//otherwise;unset alarm
            }
        }
        else if (!clocks[i].isSet()){//if clock is no set
            if (*(states[i])){//if state is true
                clocks[i].setAlarm(timers[i]);//turn on alarm
            }
        }
    }
}

void resetFanOne(){
	bb.setLamp(3, LOW);
	fanOneState = false;
}

void resetFanTwo(){
	bb.setLamp(4, LOW);
	fanTwoState = false;
}

void resetFanThree(){
	bb.setLamp(5,LOW);
	fanThreeState = false;
}

/*
*
*	FxN :: keyDown
*		handles keydown events
*
*/
void keyDown(uint8_t button, uint8_t panel){
	//bb.setLamp(button, !(*states[button]));
	//bb.setLamp(panel, *states[button]);
	
	bb.setLamp(button, !(*states[button]));
	bb.setLamp(panel, *states[button]);
}


void refresh(uint8_t button){
	if (*states[button] == false){
		*locks[button] = false;
		bb.setLamp(button,HIGH);
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
		if (bb.getButton(button) && !(*locks[button])){
			if (*states[button] == false){
				*locks[button] = true;
				*states[button] = true;
				keyDown(button, button+panelOffset);
			}
		}
		else if (!bb.getButton(button)){
			refresh(button);
		}
	}
}

/*
*	FxN :: loop
*/
void loop(){
	buttonHandler();
	alarmClockRoutine();
}
