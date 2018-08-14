/*==================================================================
								Roto
 
						Discovery World Energy
						 Putting Wind To Work
 
						  Charles Stevenson

STILL IN PRODUCTION
Notes::
		Putting Wind To Work
				+Push a Button (not included in code)
				+turns blower on
				+air spins custom made propellor
				+how fast the propellor spins determined by fan blade config; returns rpm
				+bottom score is current score (current rpm)
				+top score is high score (highest rpm)
				+resets after 60s if not active
 ==================================================================*/

#include <alarmClock.h>
#include <digits.h>

#define versionString "Roto - [Discovery World Energy] [Putting Wind To Work]"

//SETTINGS
//=================================
//serial
uint32_t const BAUD_RATE = 9600;
//score trackers
uint32_t gearCounter = 0;
float rev = 30; //number of gears
//score containers
uint32_t highScore = 0;
uint32_t currentScore = 0;
uint32_t prevScore = 0;
//sizes
uint8_t const NUM_DIGITS = 6;
uint8_t const NUM_CLOCKS = 3;
uint8_t const NUM_STATES = 3;
//=================================


//BOARD PINS
//=================================
//7segTop
uint32_t const segData = 11;
uint32_t const segClock = 12;
uint32_t const segLatch = 13;
uint32_t const lengthA = 3;
uint32_t const lengthB = 3;
uint32_t const offsetA = 0;
uint32_t const offsetB = lengthA;
//GearTooth
uint32_t chanA = 2;
//=================================
byte sustainCounter = 0;
int score = 0;


//FLOW CONFIGURATION
//=================================
//states
bool activeState = false; //triggers play function :: 1s
bool attractState = true; //triggers sexify function :: 0.3s
bool resetState = false;  //triggers resetExhibit function :: 60s
bool* states[NUM_STATES] = {&activeState, &attractState, &resetState};
//prototypes
void play();
void sexify();
void resetExhibit();
//timers
uint32_t activeTimer = 100; // 1 seconds ADJUST THIS VALUE FOR SCORE UPDATE
uint32_t attractTimer = 300; // 0.3 seconds
uint32_t resetTimer = 60000; // 60 seconds
uint32_t timers[NUM_CLOCKS] = {activeTimer, attractTimer, resetTimer};
//clocks
alarmClock activeClk = alarmClock(play);
alarmClock attractClk = alarmClock(sexify);
alarmClock resetClk = alarmClock(resetExhibit);
alarmClock clocks[NUM_CLOCKS] = {activeClk, attractClk, resetClk};
//=================================

//instantiate objects
digits dig = digits(segData, segClock, segLatch, NUM_DIGITS);
digitGroup segsBot = digitGroup(&dig, offsetA, lengthA); 
digitGroup segsTop = digitGroup(&dig, offsetB, lengthB); 


/*=========================================================*/
/* ---- 		CALIBRATION AND SETUP 					---*/
/*=========================================================*/

/*
*
* FxN :: setupSegs
*   protocol to reset pins on 7segs
*
*/
void setupSegs(){
    for (byte i = segData; i <= segLatch; i++){
        digitalWrite(i,LOW);      
    }
    for (byte i = segData; i < segLatch; i++){
        digitalWrite(i,HIGH);
    }
}

/*
   Fxn Setup
 */
void setup()
{
  //Open Serial && write version
  Serial.begin(BAUD_RATE);
  Serial.println(versionString);
  Serial.print(F("File: "));
  Serial.println(__FILE__);

  //pins
  pinMode(chanA, INPUT);

  //interrupt
  attachInterrupt(digitalPinToInterrupt(chanA),intrGearTooth,CHANGE); 

  //7segs
  setupSegs();

}

/*=========================================================*/
/* ---- 		 Helper Functions   					---*/
/*=========================================================*/

/*
*	FxN :: resetExhibit
*		resets to init state
*/
void resetExhibit(){
		//reset states
		attractState = true;
		activeState = !attractState;
		//reset gear counter
		gearCounter = 0;
}

/*
*	FxN :: stateChanges
*		handles state Changes
*		active and reset state
*/
void stateChanges(){
		if (!activeState){
				//activates play function
				activeState = true;
				attractState = false;
		}
}

/*
* FxN :: intrGearTooth
*   Interrupt Function, that counts gear teeth
*/
void intrGearTooth(){
		gearCounter++;
		stateChanges();
}

/*
*	FxN :: analyzeGear
*		returns GearCounter in rpm
*/
uint32_t analyzeGear(){
		return (float(gearCounter / rev) * float((float(1000.0/activeTimer) * 60.0))); //1000 = ms; 60 = seconds in minute
}

/*
*	FxN :: updateHighScore
*/
void updateHighScore(){
		if (highScore < currentScore && notHacking()){
				highScore = currentScore;
				segsTop.segDisp(highScore);
		}
}


bool rampUp(){
		uint32_t rampThreshold = 50;
		return ((currentScore - prevScore) > rampThreshold);
}

bool sustain(){
		uint8_t sustainTrigger = 5;
		uint8_t sustainThreshold = 20;
		bool notCheating = false;
		if (currentScore - prevScore <= sustainThreshold && currentScore - prevScore >= 0 ||
		    prevScore - currentScore <= sustainThreshold && prevScore - currentScore >= 0){
				sustainCounter++;
				if (sustainCounter >= sustainThreshold){
						notCheating = true;
				}
		}
		else{
				sustainCounter = 0;
		}
		return notCheating;
}

bool notHacking(){
		return (!rampUp() && sustain());
}


void adjustScore(){
		if (score < currentScore){
				score++;
		}
		else if (score > currentScore){
				score--;
		}
		Serial.println(currentScore);
		Serial.println(score);
		segsBot.segDisp(score);

}

/*
*	FxN :: displayScore
*		changes to an active state and displaysScore
*
*/
void displayScore(){
		currentScore = analyzeGear();
		if (!currentScore){
				//turns reset to off
				resetState = false;
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

/*
*  FxN :: sexify
*  	Attraction Function :: <3
*/
void sexify(){
    segsBot.chaseAnimation8();
	segsTop.segDisp(highScore);
}


/*=========================================================*/
/* ---- 		 Play Functions   					    ---*/
/*=========================================================*/

/*
*	FxN :: play
*		high level calls of flow of program
*/
void play(){
		displayScore();//displays score
		adjustScore();
		updateHighScore();//if so; update high score
		gearCounter = 0; //reset gearCounter
		resetState = true; //activate reset state
}

/*
*	FxN :: loop
*		controls flow logic
*/
void loop(){
		alarmClockRoutine();
}
