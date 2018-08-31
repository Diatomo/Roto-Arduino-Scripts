
/*
   Roto
 
 Project:FRIST
 Exhibit: Corn of Plenty
 Author(s): Charles Stevenson
 Date: 1/31/18
 
 Original environment:
 Arduino 1.8.2
 
 Revision notes:

  This is a work in progress...

  
 */



#include "alarmClock.h"
#include "outputExtend.h"
#include "inputExtend.h"

#define versionString "Roto - [NMAI] [Corn of Plenty]"

//TODO unlock 5 seconds.

//light scheme
//red always on
//yellow = input
//green = unlocked (red turns off);

//sizes
uint8_t const NUM_ROWS = 4;
uint8_t const NUM_COLUMNS = 3;
uint8_t const NUM_LEDS = 3;
uint8_t const NUM_CLOCKS = 1;
uint8_t const NUM_STATES = 1;
uint8_t const NUM_OE = 1; //output extend boards
uint8_t const NUM_IE = 1; //input extend boards
uint8_t const ANSWER_LENGTH = 6;

//settings
uint32_t const BAUD_RATE = 9600;

//states
bool activeState = false;
bool* states[NUM_STATES] = {&activeState};

//pin configurations
uint32_t const oeData = 16;
uint32_t const oeClock = 15;
uint32_t const oeLatch = 14;
uint32_t const ieData = 13;
uint32_t const ieClock = 12;
uint32_t const ieLatch = 11;

//keypad configuration
uint8_t const pinA = 0;
uint8_t const pinB = 1;
uint8_t const pinC = 2;
uint8_t const pinD = 3;
uint8_t const pinOne = 0;
uint8_t const pinTwo = 1;
uint8_t const pinThree = 2;

//leds
uint32_t const ledG = 6;
uint32_t const ledY = 5;
uint32_t const ledR = 4;

//relay
uint32_t const relay = 7;

//dataStructures
uint8_t const ROWS[NUM_ROWS] = {pinA, pinB, pinC, pinD};
uint8_t const COLUMNS[NUM_COLUMNS] = {pinOne, pinTwo, pinThree};

//representation of keypad
char const keypad[NUM_ROWS][NUM_COLUMNS] = {
												{ '1', '2', '3' },
												{ '4', '5', '6' },
												{ '7', '8', '9' },
												{ '#', '0', '*' }
										   };

//answer variables
char const ANSWER_KEY[ANSWER_LENGTH] = { '1', '6', '0', '0', '0', '0'};
uint8_t answerCounter = 0;

//TODO timeout 2 seconds. 

//prototypes
void resetExhibit();

//define timers
uint32_t resetTimer = 3000; //3 seconds
uint32_t inputTime = millis();
bool inputOn = false;
uint32_t inputInterval = 300;

//init clocks
alarmClock resetClk = alarmClock(resetExhibit);

//init board objects
outputExtend oe = outputExtend(oeData, oeClock, oeLatch, NUM_OE);
inputExtend ie = inputExtend(ieData, ieClock, ieLatch, NUM_IE);



void oeSetup(){
		for (byte i = 0; i < 8; i++){ //8 denotes number of outputs
				oe.extendedWrite(i, HIGH);
				delay(300);
		}
		for (byte i = 0; i < 8; i++){ //8 denotes number of outputs.
				oe.extendedWrite(i, LOW);
				delay(300);
		}
		oe.extendedWrite(ledR, HIGH); //red led
}

/*
*

*	
*	FxN :: setup	
*		obligatory setup function that sets up pin states
*
*/

void setup(){
		Serial.begin(BAUD_RATE);
		Serial.println(versionString);
		Serial.print(F("File: "));
		Serial.println(__FILE__);
		oeSetup();
}

/*
*
* FxN :: resetExhibit
*	resets certain parameters to an initial state
*
*/
void resetExhibit(){
		answerCounter = 0;
		oe.extendedWrite(ledR, HIGH);
		oe.extendedWrite(ledY, LOW);
		oe.extendedWrite(ledG, LOW);
		oe.extendedWrite(relay,LOW);
}

/*
*
*	
*	FxN :: multiplex
*		scans the keypad for data input.
*		if two reads are low then will return a character
*		from the keypad matrix otherwise it returns a null character
*
*/
char multiplex(){
		char input = 'n';
		bool keyPressed = false;
		for (byte i = 0; i < NUM_ROWS; i++){
				oe.extendedWrite(ROWS[i], HIGH);
				for (byte j = 0; j < NUM_COLUMNS; j++){
						keyPressed = !(ie.extendedRead(COLUMNS[j]));
						if (keyPressed){
								resetClk.setAlarm(resetTimer);//resets alarm
								input =  keypad[i][j];
								Serial.print("What key was pressed? ");
								Serial.println(input);
								while (ie.extendedRead(COLUMNS[j]));
						}
				}
				oe.extendedWrite(ROWS[i], LOW);
		}
		return input; //return a null character otherwise;


}

/*
*
*	
*	FxN :: unlock	
*		triggers the win animation
*				1) unlocking the relay
*				2) lighting on the led
*/
void unlock(){
		oe.extendedWrite(relay, HIGH);
		delay(100);
		oe.extendedWrite(ledR, LOW);
		delay(50);
		oe.extendedWrite(ledG, HIGH);
		oe.extendedWrite(ledY, LOW);
    delay(5000);
}

/*
*
*	
*	FxN :: checkUnlock
*		Checks to see if the answercounter has been incremented all the way to the end.
*
*/
void checkUnlock(){
		if (answerCounter >= (ANSWER_LENGTH)){
				unlock();
        resetExhibit();
		}
}


void triggerInput(){
		oe.extendedWrite(ledY, HIGH);
		delay(200);
		inputOn = true;
		inputTime = millis();
}

/*
*
*	
*	FxN :: checkAnswer	
*		checks if the answer is correct then proceeds to check win state
*
*/
void checkAnswer(char answer){
		if (answer != 'n'){
				Serial.println(answer);
		}
		if (answer == ANSWER_KEY[answerCounter]){
				answerCounter++;
				triggerInput();
				checkUnlock();
		}
		else if (answer != 'n' && answer != ANSWER_KEY[answerCounter]){
				if ((answer != '#') || (answer != '*')){
						triggerInput();
				}
				answerCounter = 0;
		}
}

/*
*
*	FxN :: alarmClockRoutines
*		processes states and polling
*
*
*/
void alarmClockRoutine(){
		if (resetClk.isSet()){
				resetClk.poll();//poll
		}
		if ((millis() - inputTime > inputInterval) && inputOn){
				oe.extendedWrite(ledY, LOW);
				inputOn = false;
		}
}

/*
*
*	
*	FxN :: loop	
*		scans for multiplex that returns a key
*		checks for correct answer
*		if so, check win state and increment counter
*		if not, reset counter and re-looooooop.
*
*/
void loop(){
		checkAnswer(multiplex());
		alarmClockRoutine();		
}
