
/*
   Roto
 
 Project: Ohio Statehouse
 Exhibit: Constitution
 Author(s): Charles Stevenson
 Date: 9/28/18
 
 Original environment:
 Arduino 1.8.2
 
 Revision notes:
 
 */


/* ====================================================*/
/* =========    SCRIPT CONFIGURATION   ================*/
/* ====================================================*/

#include <pwmBoard.h>
#include "outputExtend.h"
#include <buttonBoard.h>

#define versionString "Roto - [Ohio Statehouse] [Ohio Constitution]"

#define ON LOW
#define OFF HIGH
#define A 0
#define B 1


const uint8_t PROGMEM gamma8[] = {
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
	1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
	2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
	5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
	10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
	17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
	25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
	37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
	51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
	69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
	90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
	115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
	144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
	177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
	215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};

//Cap Sensor Pin
uint8_t const capSensorA = 0;
uint8_t const capSensorB = 1;
uint8_t const clockA = 0;
uint8_t const clockB = 1;

uint8_t const counter = 8;
uint8_t counterToggle = LOW;
bool capState = false;

/*PWM BOARDS*/
uint8_t pwmAddy = 15;
pwmBoard pwm = pwmBoard(pwmAddy);

/*OUTPUT EXTEND*/
uint8_t const oeData = 6;
uint8_t const oeClock = 5;
uint8_t const oeLatch = 4;
uint8_t const nOE = 1;
outputExtend oe = outputExtend(oeData, oeClock, oeLatch, nOE);

/*BUTTON BOARD*/
uint8_t const bbDI = 9;
uint8_t const bbDO = 10;
uint8_t const bbCLK = 11;
uint8_t const bbILT = 12;
uint8_t const bbOLT = 13;
uint8_t const nBB = 1; //number of button boards
uint8_t const nBUTTONS = 2; // number of buttons
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, nBB);//init bb

float ratioMetric = 500.0;


struct display{
	uint8_t ID;
	uint8_t capSensor;
	bool buttonPressed = false;
	uint8_t clock;
	uint8_t pot;
	int levelCap;
	float currLevel = 0;
	uint32_t timer = 0;
	uint32_t buttonTimer = 0;

	bool isSet(){
		return (timer > 0);
	};

	void unSet(){
		timer = 0;
	};

	void setAlarm(){
		timer = millis();
	};

	bool poll(){
		return (millis() - timer > 1);
	};
};

typedef struct display Display;

Display caseA;
Display caseB;

void setupDisplays(){
	
	/*CASEA*/
	//setup button
	caseA.ID = 0;
	caseA.capSensor = 0;

	//setup clock
	caseA.clock = 0;
	oe.extendedWrite(caseA.clock, LOW);

	//setup potentiometer
	caseA.pot = 14;
	caseA.levelCap = constrain(map(analogRead(caseA.pot), 0, 1023, 0, 255), 0, 255);
		
	/*CASEB*/
	//setup button
	caseB.ID = 1;
	caseB.capSensor = 1;

	//setup clock

	caseB.clock = 1;
	oe.extendedWrite(caseB.clock, LOW);

	//setup potentiometer
	caseB.pot = 15;
	caseB.levelCap = constrain(map(analogRead(caseB.pot), 0, 1023, 0, 255), 0, 255);
}

void setupBB(){
	bb.setLamp(false);
}

void setup(){

	//setup Serial
	uint32_t BAUD_RATE = 9600;
	while (!Serial);
    Serial.begin(BAUD_RATE);
    Serial.println(versionString);
    Serial.print(F("File: "));
    Serial.println(__FILE__);

	setupDisplays();
	setupBB();

	pinMode(caseA.pot,INPUT);
	pinMode(caseB.pot,INPUT);
	
	//setup pwm
	pwm.start(); pwm.setLevel(0); pwm.send();
}


/*
*======================================================================
*					LIGHT && BUTTON FUNCTIONS
*======================================================================
*/

/*
*	FxN :: fadeOn
*		fades the lights on
*/
void fadeOn(Display* cabinet){
	
	if (cabinet->currLevel < cabinet->levelCap){
		cabinet->currLevel += (cabinet->levelCap / 255.0);
		if (cabinet->currLevel > cabinet->levelCap){
			cabinet->currLevel = cabinet->levelCap;
		}
	}
	else{
		cabinet->currLevel--;
		if (cabinet->currLevel < 0){
			cabinet->currLevel = 0;
		}
	}

	//CASE A light index
	if (cabinet->ID == A){
		for (uint8_t i = 0; i < 4; i++){
		pwm.setLevel(i, pgm_read_byte(&gamma8[int(cabinet->currLevel)]));
		}
	}

	//CASE B light index
	if (cabinet->ID == B){
		for (uint8_t i = 4; i < 8; i++){
			pwm.setLevel(i, pgm_read_byte(&gamma8[int(cabinet->currLevel)]));
		}
	}
	pwm.send();
}

/*
*	FxN :: fadeOff
*		fades the lights off
*/
void fadeOff(Display* cabinet){
	if (cabinet->currLevel > 0){
		cabinet->currLevel -= (cabinet->levelCap / 255.0);
		if (cabinet->currLevel < 0 || cabinet->currLevel == 0){
			cabinet->currLevel = 0;
			oe.extendedWrite(cabinet->clock, LOW);
		}
	}

	//CASE A light index
	if (cabinet->ID == A){
		for (uint8_t i = 0; i < 4; i++){
			pwm.setLevel(i, pgm_read_byte(&gamma8[int(cabinet->currLevel)]));
		}
	}
	
	//CASE B light index
	if (cabinet->ID == B){
		for (uint8_t i = 4; i < 8; i++){
			pwm.setLevel(i, pgm_read_byte(&gamma8[int(cabinet->currLevel)]));
		}
	}
	delay(1);
	pwm.send();
}


/*
*	FxN :: readCap
*		Reads button presses on the displays on the case.
*/
void readCap(Display* cabinet){
	if (bb.getButton(cabinet->capSensor) && !cabinet->buttonPressed){
		cabinet->buttonTimer = millis();
		cabinet->buttonPressed = true;
		oe.extendedWrite(cabinet->clock, HIGH);
	}
	//1000 = 1 second
	else if (millis() - cabinet->buttonTimer > 3000 && cabinet->buttonTimer != 0){
		cabinet->buttonPressed = false;
		cabinet->buttonTimer = 0;

	}
}

/*
*======================================================================
*							MAIN
*======================================================================
*/


/*
* FxN :: alarmClockRoutines
*   processes states and polling
*/
void alarmClockRoutine(Display* cabinet){
	if (cabinet->isSet()){
		if (cabinet->buttonPressed){//if corresponding state is true
			if(cabinet->poll()){
				fadeOn(cabinet);
			}
		}
		else{
			cabinet->unSet();//otherwise;unset alarm
		}
	}
	else if (!cabinet->isSet()){//if clock is no set
		if (cabinet->buttonPressed){//if state is true
			cabinet->setAlarm();//turn on alarm
		}
		else if(!cabinet->buttonPressed){
			cabinet->unSet();
			fadeOff(cabinet);
		}
	}
}

void changeLevel(Display* cabinet){
	//1023 = potentiometer MAX (5V)
	//255 = byte pwm MAX
	uint8_t newRead = constrain(map(analogRead(cabinet->pot), 0, 1023, 0, 255), 0, 255);
	if (newRead != cabinet->levelCap){
		cabinet->levelCap = newRead;
	}
}

/*
*
*	FxN :: loop
*		1)	Reads capacitive touch button as fast as possible
*			until it is pressed. 
*		2)	Then Throttles the update for each second interval.
*
*/
void loop(){

	//read caps
	readCap(&caseA);
	readCap(&caseB);

	//alarm clock routine
	alarmClockRoutine(&caseA);
	alarmClockRoutine(&caseB);

	//changeLevel if needed
	changeLevel(&caseA);
	changeLevel(&caseB);
}
