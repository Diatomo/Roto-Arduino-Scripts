
/*
   Roto
 
 Project: Discovery World Energy
 Exhibit: Power_Milwaulkee <Buttons && Stu-Knob section>
 Author(s): Charles Stevenson
 Date: 4/9/2018
 
 Original environment:
 Arduino 1.8.2
 
 Revision notes:

		EXHIBIT IS STILL IN PRODUCTION
				
				Therefore a lot of the requirements have not been set yet.
				I've not idea how to implement some of the tools and thus
				a lot of this program is filler.

				However;

				There are some implementation patterns that are generic enough
				to encompass any input or state changes placed into the requirement
				of the exhibit. For example, buttonroutine scans for button input
				and then changes the state of the program. The alarmclock routine
				then scans these state changes and deactivates or acivates callback
				functions.

		REQUIREMENTS POSTED SO FAR.
				The stu knob turns and outputs only a 0 or a 1.
						- nothing about the direction
						- nothing about the sensitvity has been defined
						- nothing about how the 0 or 1 is exactly produced. e.g. (direction / speed);
						- no buttons implementations have been defined
 
 */

 
#include <alarmClock.h>
#include <buttonBoard.h>

#define versionString "Roto - [Discovery World Energy] [Power Milwaulkee]"


/* ====================================================*/
/* =========    SCRIPT CONFIGURATION   ================*/
/* ====================================================*/

//GENERAL CONFIG
uint32_t BAUD_RATE = 9600;
uint32_t chanA = 2;
uint32_t chanB = 3;

//KNOB CONFIG
uint32_t const SENSITIVITY = 10000;
uint32_t sensCounter = SENSITIVITY/2; //may not need this.

//BUTTON BOARD CONFIG <Check Schematic for Details>
const uint8_t bbDI = 8; //button board data in
const uint8_t bbDO = 9; //button board data out
const uint8_t bbCLK = 10; //button board clock
const uint8_t bbILT = 11; //IN LATCH
const uint8_t bbOLT = 12; //OUT LATCH
const uint8_t nBB = 1; //number of button boards
const uint8_t nBUTTONS = 4; // number of buttons

//states
const uint8_t NUM_STATES = 1;
bool activeState = false;
bool* states[NUM_STATES] = {&activeState};

//prototypes
void resetExhibit();

//INSTANTIATE OBJECTS
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, nBB);//init bb
alarmClock resetClk = alarmClock(resetExhibit); //init clock

//ALARM CLOCK CONFIG
const uint8_t NUM_CLOCKS = 1; //array size constant
uint32_t resetTimer = 60000;//define timers
alarmClock clocks[NUM_CLOCKS] = {resetClk};//alarmclocks and timers
const uint32_t timers[NUM_CLOCKS] = {resetTimer};


/*
*
*	FxN :: testStartButtons
*		blinks button leds on and off to check if they are working.
*
*/
void testStartButtons(){
		bool toggle = false;
		const uint8_t nTOGGLES = 5;
		for(uint8_t i = 0; i < nTOGGLES; i++){
				bb.setLamp(toggle);
				delay(500);
				toggle = !toggle;
		}
		bb.setLamp(LOW);
}


/*
*	FxN :: setup
*		Prints config, setup pins, mount interrupt, test button leds
*
*/
void setup(){
    
	//Open Serial && write version
    Serial.begin(BAUD_RATE);
    Serial.println(versionString);
    Serial.print(F("File: "));
    Serial.println(__FILE__);

    //pins
    pinMode(chanA, INPUT);
    pinMode(chanB, INPUT);

    //interrupt
    attachInterrupt(digitalPinToInterrupt(2),stuKnob,CHANGE); 

    //buttonTest
    testStartButtons();
}


/* ===============================================*/
/* =========    HELPER FUNCTIONS  ================*/
/* ===============================================*/

/*
*
*
* FxN :: changeHertz
*   Interrupt Function, that counts up and down the encoder values.
*
*/
void stuKnob(){
    if (digitalRead(chanA) != digitalRead(chanB)){
        if (sensCounter < SENSITIVITY){
            sensCounter++;
        }
    }
    else{
        if (sensCounter > 0){
            sensCounter--;
        }
    }
}

/*
*	FxN :: resetExhibit
*		Resets Exhibit to Initial State
*/
void resetExhibit(){
		bb.setLamp(LOW);
}

/*
*    FxN :: buttonRoutine
*		PLEASE COMMENT REQUIREMENTS HERE.
*		IMPLEMENTATION IS CURRENTLY FILLER
*		UNTIL REQUIREMENTS/BEHAVIOR IS POSTED.
*/
void buttonRoutine(){
		static int8_t previousButton = -1;
		for (uint8_t i = 0; i < nBUTTONS; i++){
				if (bb.getButton(i) && (i != previousButton)){//if unique button is pressed
						previousButton = i;
						bb.setLamp(LOW);
						bb.setLamp(i, HIGH);//turn the light on for the button pressed
						activeState = false;
				}
		}
}


/*
*
* FxN :: alarmClockRoutines
*   processes states and polling
*   
*/
void alarmClockRoutine(){
    for (byte i = 0; i < NUM_CLOCKS; i++){
        if (clocks[i].isSet()){//if clock is set
            if (*(states[i])){//if corresponding state is true
                clocks[i].poll();//poll
            }
            else{
                clocks[i].unSetAlarm();//otherwise; unset alarm
            }
        }
        else if (!clocks[i].isSet()){//if clock is no set
            if (*(states[i])){//if state is true
                clocks[i].setAlarm(timers[i]);//turn on alarm
            }
        }
    }
}



/* ===============================================*/
/* =========    PLAY FUNCTIONS    ================*/
/* ===============================================*/

/*
*		FxN :: play
*			Holder of Script API
*			That is, the high level logic of what the exhibit does.
*
*/
void play(){
		analyzeParams();
}


/*
*    FxN :: analyzeParams
*        Generic function to analyze results produced by the interrupt by the encoder
*
*/
void analyzeParams(){
        if (sensCounter >= SENSITIVITY || sensCounter <= 0){
                Serial.println(sensCounter > 0);
                sensCounter = SENSITIVITY/2;//reset sensCounter
				activeState = true;
        }
}

/*
*
*	FxN :: loop
*		Hold input / state checking routines that alter the flow of the program
*
*/
void loop(){
		buttonRoutine(); //searches for button presses and changes program state.
		alarmClockRoutine(); //changes flow of programi based on state changes
		play(); //plays some sort of sequence of events
}
