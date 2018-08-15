/*
   Roto
 
 Project:FRIST
 Exhibit: Sound Vibration
 Author(s): Charles Stevenson
 Date: 1/24/18
 
 Original environment:
 Arduino 1.8.2
 
 Revision notes:
 
 */


#include <alarmClock.h>
#include <digits.h>
#include <outputExtend.h>
#include <EEPROM.h>

#define versionString "Roto - [FRIST] [Sound Vibration]"

boolean testActive = false;

//sizes
uint8_t const NUM_HERTZ = 7;
uint8_t const NUM_OE = 1;
uint8_t const NUM_SEGS = 4;
uint8_t const NUM_CLOCKS = 3;
uint8_t const NUM_STATES = NUM_CLOCKS;
uint8_t const OE_OFFSET = 1;

//knob and cleanup
uint32_t const SENSITIVITY = 10000; //SENSITIVITY FOR KNOB TURNING
uint32_t maxCleanupCounts = 15; //FOR CLEANUP
//counters
int sensCounter = SENSITIVITY/2;


//settings 
uint32_t const BAUD_RATE = 9600;
uint8_t currHertz = 1;//start index
uint8_t prevHertz = 1;//previous sound played
uint32_t blowCounter = 0;//increment counter to trigger a cleanup
uint32_t const displayHertz[NUM_HERTZ] = {400,632,849,1130,1396,1706,1830};//list of hertz to be played


/*
*
*  States
*   Control the flow of the program via activating and deactivating alarmclocks
*     chaserState === triggers sexify function, flipped when initial state, reset, or cleaning up i.e. when blower is on.
*       activeState === triggers a reset timer, flipped when knob is touched outside of blower state
*       blowerState === triggers blower OFF, flipped when blower is turned ON.
*/
//states
bool activeState = false;
bool chaserState = true;
bool blowerState = false;
bool* states[NUM_STATES] = {&activeState, &chaserState, &blowerState};

bool hzChange = false;

//sound only plays when inactive for a certain period of time;
bool activeKnob = false;//boolean to check to see if knob is in action;
uint32_t activeKnobTimer = 0;
uint32_t knobTimerInterval = 2000;



//pin configurations...see schematic for details
uint32_t const oeData = 13;
uint32_t const oeClock = 12;
uint32_t const oeLatch = 11;
uint32_t const segData = 17;
uint32_t const segClock = 18;
uint32_t const segLatch = 19;
uint32_t const chanA = 2;
uint32_t const chanB = 3;
uint32_t const BUTTON_ONE = 8; 
uint32_t const BUTTON_TWO = 9;
int eeAddyOne = 0;
int eeAddyTwo = sizeof(uint32_t);

//function prototypes
void analyzeParams(byte, byte, uint32_t);
void resetExhibit();
void sexify();
void turnOffBlower();

//init clocks
alarmClock resetClk = alarmClock(resetExhibit);
alarmClock attractClk = alarmClock(sexify);
alarmClock termBlowerClk = alarmClock(turnOffBlower);

//define timers
uint32_t resetTimer = 60000;
uint32_t chaseTimer = 300;
uint32_t blowTimer = 25000;

//OBJECTS
//alarmclocks and timers
alarmClock clocks[NUM_CLOCKS] = {resetClk, attractClk, termBlowerClk};
uint32_t timers[NUM_CLOCKS] = {resetTimer, chaseTimer, blowTimer};
//boards
outputExtend oe = outputExtend(oeData, oeClock, oeLatch, NUM_OE); 
digits dig = digits(segData, segClock, segLatch, NUM_SEGS);
digitGroup segs = digitGroup(&dig, 0, NUM_SEGS); 


/* =========    INITIAL SETUP    ================*/
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

void setupOE(){
    //oe Extended.
    for (byte i = 0; i < NUM_HERTZ + OE_OFFSET; i++){
        oe.extendedWrite(i,LOW);
        delay(100);
    }
}

/*
*
* FxN :: setup
*
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
    attachInterrupt(digitalPinToInterrupt(2),changeHertz,CHANGE); 

	//buttons
	pinMode(BUTTON_ONE, INPUT);
	pinMode(BUTTON_TWO, INPUT);

    //setupBoards
    setupSegs();
    setupOE();

	//EEPROM.put(eeAddyOne, maxCleanupCounts);//first time compiling.
	EEPROM.get(eeAddyOne, maxCleanupCounts);
	//EEPROM.put(eeAddyTwo, blowTimer);
	EEPROM.get(eeAddyTwo, blowTimer);
}

/* =========    HELPER FUNCTIONS    ================*/

/*
*
*  FxN :: sexify
*   Attraction Function
*
*/
void sexify(){
    segs.chaseAnimation8();//sexy chase animation
}

void halt(){
    activeState = false;
    chaserState = false;
    blowerState = false;
}

/*
*
* FxN :: resetExhibit
* resets certain parameters to an initial state
*
*/
void resetExhibit(){
    currHertz = 1;//reset hertz index to starting value
    prevHertz = 1;//reset previous hertz index to starting value
    sensCounter = SENSITIVITY/2;//set inbetween SENSITIVTY bounds so a constant theta turn will inc or dec currHertz
    activeState = false;//set to attract state
    chaserState = !activeState;//set to attract state
    blowerState = false;
    hzChange = false;
}

/*
*
*  FxN :: playSound
*   Routine that plays sound.
*
*/
void playSound(){
    if (blowerState != true){
      oe.extendedWrite(currHertz + OE_OFFSET, HIGH);//reset
      delay(1000);//wait to register
      oe.extendedWrite(currHertz + OE_OFFSET, LOW);//play sound
      delay(100);
      blowCounter++;
  }
}


/*
*
* FxN :: turnOnBlower
*   Turns on the blower
*
*/
void turnOnBlower(){
    oe.extendedWrite(0,LOW);
    delay(100);
    oe.extendedWrite(0,HIGH);
    delay(100);

}

/*
*
* FxN :: turnOffBlower()
*   turns blower off.
*
*/
void turnOffBlower(){
    oe.extendedWrite(0,HIGH);
    delay(100);
    oe.extendedWrite(0,LOW);
    delay(100);
    resetExhibit();
} 


/* =========    KNOB STATS    ================*/

/*
*
*
* FxN :: changeHertz
*   Interrupt Function, that counts up and down the encoder values.
*
*/
void changeHertz(){
    if (digitalRead(chanA) != digitalRead(chanB)){
        if (sensCounter < SENSITIVITY && (currHertz < (NUM_HERTZ-1))){
            sensCounter++;
        }
    }
    else{
        if (sensCounter > 0 && (currHertz > 0)){
            sensCounter--;
        }
    }
}

/*
*
* FxN :: analyzeHertz
*   Analyzes is a change needs to be made to the overall hertz
*
*   There are two different changes to keep track of. One is local {change} and the other is global {hzChange}
*   The reason I need this is because the metric I use to trigger a sound is if the previous Sound played
*   is different than the sound I'm about to play. Therefore, the local change triggers the active state
*   meaning I'm displaying sound options via 7segs. But if someone touches the knob and returns to the previous sound
*   based on the metric I'm using it won't play. Therefore a global change says you really did change the sound and
*   when the knob is left inactive it will play regardless if it was the previous sound or not.The control structures
*   are probably stepping on eachothers feet a little bit, if it were implemented in a larger codebase it would be worth
*   optimizing
*
*
*/
boolean analyzeHertz(){
    boolean change = false;
    if (sensCounter >= SENSITIVITY){
        if (currHertz < (NUM_HERTZ - 1)){
            currHertz++;
            sensCounter = SENSITIVITY/2;
            hzChange = true;
            change = true;
        }
    }
    else if (sensCounter <= 0){
        if (currHertz > 0){
            currHertz--;
            sensCounter = SENSITIVITY/2;
            hzChange = true;
            change = true;
        }
    }
    return change;
}

/*
*
* FxN :: checkCleanUp
*   checks if sand needs to be reblown and switched to corresponding state
*
*/
void checkCleanUp(){
    if (blowCounter >= maxCleanupCounts){
        activeState = false;
        chaserState = true;
        blowerState = true;
        blowCounter = 0;
        turnOnBlower();
    }
}

/*
*
* FxN :: knobInactive
*   Specific logic checking if the knob has been touched
* 
*
*/
bool knobInactive(){
    bool trigger = false;
    if (activeKnob == false){
        if (activeKnobTimer == 0){
            activeKnobTimer = millis();
        }
        else{
            trigger = knobActiveDuration();
        }
    }
    else{
        activeKnobTimer = 0;
    }
    return trigger;
}

/*
*
* FxN :: knobActiveDuration
*   If its been for the allotted time send a trigger to play a sound
*
*/
bool knobActiveDuration(){
    bool isKnobNotActive = false;
    if (millis() - activeKnobTimer > knobTimerInterval){
        isKnobNotActive = true;
        activeKnobTimer = 0;
    }
    return isKnobNotActive;
}


/* =========    STATE LOGIC FLOW CONTROLLER    ================*/
/*
*
* FxN :: play 
*   displays hertz then plays hertz
*
*/
void play(){
    
    if (analyzeHertz()){//if a change is detected, fire up display but don't play a sound.
        activeKnob = true;
        if (!blowerState){
            activeState = false;//prevents reset when knob is touched
            chaserState = false;//don't chase need to display hertz
            segs.segDisp(displayHertz[currHertz]);//display
        }
    }
    else{
        if (!blowerState){
            activeKnob = false;//if knob is untouched
            activeState = true;//prime for a reset. (60s)
        }
    }
    if (prevHertz != currHertz || hzChange){
        if(knobInactive()){//play sound when knob is left alone for x seconds (must be less than reset timer)
            prevHertz = currHertz;//update prevHertz
            hzChange = false;//toggle off global change
            playSound();
        }
        checkCleanUp();
    }
}


/*
*   FxN :: testSounds
*       This is just a test function
*       control whether or not this is active at the top
*       of this script
*
*/
void testSounds(){
    segs.segDisp(displayHertz[currHertz]);
    playSound();
    checkCleanUp();
    currHertz = (currHertz + 1) % NUM_HERTZ;
    delay(1000);
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

/*
    FxN :: buttonRoutine
        @Requirements
            SMALL BUTTON (RESET SWITCH LIKE) but panel Mount;
            1st button (Number of Sounds) {5,25};
            2nd button (Blower Timer) {10,60} //in seconds.
        @BEHAVIOR
            HOLD BUTTON 5 seconds before programming mode;
            Then hold button while spinning stu knob to pick parameter
            Letting go will confirm. 
            save values to eeprom.
*/
void buttonRoutine(){
    static uint32_t timer;
    uint32_t const TIME_INTERVAL = 5000; // 5 seconds
    if (!digitalRead(BUTTON_ONE)){
        if (!timer) { timer = millis(); }
        if (millis() - timer > TIME_INTERVAL){
            //halt the program
            halt();
            //set bounds
            uint32_t minSounds = 5;
            uint32_t maxSounds = 25;
            maxCleanupCounts = analyzeParams(minSounds, maxSounds, maxCleanupCounts);
            EEPROM.put(eeAddyOne, maxCleanupCounts); //save to eeprom 
            resetExhibit();
        }
    }
    else if (!digitalRead(BUTTON_TWO)){
        if (!timer) { timer = millis(); }
        if (millis() - timer > TIME_INTERVAL){
            //halt program
            halt();
            //set bounds
            uint32_t minSeconds = 10;
            uint32_t maxSeconds = 60;
            blowTimer = analyzeParams(minSeconds, maxSeconds, blowTimer/1000) * 1000;
			timers[2] = blowTimer;
            EEPROM.put(eeAddyTwo, blowTimer); //save to eeprom 
            resetExhibit();
        }
    }
    else{
        //reset timer
        if (timer){
            timer = 0;
      }
    }
}

/*
    FxN :: analyzeParams
        Generic function for tuning parameters in the buttonRoutine module
*/
uint32_t analyzeParams(uint32_t min, uint32_t max, uint32_t tempVal){
	currHertz = 1; //reset it in case the setting changes happen when on max hertz value. (check interrupt cond)
    while (!digitalRead(BUTTON_ONE) || !digitalRead(BUTTON_TWO)){
        //tune parameters
        if (sensCounter >= SENSITIVITY){
            if (tempVal < max){
                tempVal++;
                sensCounter = SENSITIVITY/2;
            }
        }
        else if (sensCounter <= 0){
            if (tempVal > min){
                tempVal--;
                sensCounter = SENSITIVITY/2;
            }
        }
        //update display
        segs.segDisp(tempVal);
    }
	return tempVal;
}

/*
*
* FxN :: loop
*   loops through a state check routin and a play routine.
*
*/
void loop(){
    alarmClockRoutine();
    buttonRoutine();
    if (testActive){
        testSounds();
    }
    else{
        play();
    }
}
