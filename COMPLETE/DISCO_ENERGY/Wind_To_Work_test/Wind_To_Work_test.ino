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
int32_t highScore = 0;
int32_t currentScore = 0;
int32_t prevScore = 0;
//sizes
uint8_t const NUM_DIGITS = 6;
uint8_t const NUM_CLOCKS = 3;
uint8_t const NUM_STATES = 3;
//=================================


//Stats
int minimum = 0;
int maximum = 0;
int tempCounter = 1;
int counter = 1;
int maxCounter = 0;

int minGrowth = 0.5;
int maxGrowth = 3;

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
uint32_t activeTimer = 200; // 1 seconds ADJUST THIS VALUE FOR SCORE UPDATE
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

int maxAmountofCounts = 20000/activeTimer ;//hardcoded exact math should be (dtBlowTimer / dtPollTimer)

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
		if (highScore < maximum && analyzeHighScore()){
				highScore = maximum;
				segsTop.segDisp(highScore);
		}
}

bool analyzeHighScore(){
		float tempGrowthRate = float((maximum - minimum) / counter);
		Serial.print("tempGrowthRate :: ");
		Serial.println(tempGrowthRate);
		Serial.print("counter :: ");
		Serial.println(counter);
		return (minGrowth < tempGrowthRate && tempGrowthRate < maxGrowth && counter < maxAmountofCounts);
}

void closeOut(){
		while(score != 0){
				score--;
				delay(10);
				segsBot.segDisp(score);
		}
		updateHighScore();
		minimum = 0;
		maximum = 0;
		tempCounter = 1;
		counter = 1;
}


void adjustScore(){
		int32_t incrementBoundary = 4;
		int32_t factor = 5;
		bool change = false;
		if (currentScore != 0){
				tempCounter++;
				for (int32_t i = incrementBoundary; i > 0; i--){
						if (!change){
								if (score < currentScore - factor * i){
										score = score + (i+1);
										change = true;
										if (currentScore > maximum){
												maximum = score;
												counter = tempCounter;
										}
								}
								else if (score > currentScore + factor * i){
										score = score - (i+1);
										change = true;
								}
						}
				}
		}
		else{
				closeOut(); //if turbine stops then reset the game.
		}
		segsBot.segDisp(score);

}

/*
*	FxN :: displayScore
*		changes to an active state and displaysScore
*
*/
void displayScore(){
		currentScore = analyzeGear();
		if (currentScore > 0){
				resetState = false;
				if (minimum != 0){
						minimum = score+1;
				}
		}
		if (currentScore == 0){
				resetState = true;
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
		//updateHighScore();//if so; update high score
		gearCounter = 0; //reset gearCounter
}

/*
*	FxN :: loop
*		controls flow logic
*/
void loop(){
		alarmClockRoutine();
}
