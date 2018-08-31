/*
 * Roto
 *
 * Project: Akron Pyschology
 *
 *
 * Exhibit: Moron Test
 *
 *
 * Author(s): Brady Schoeffler
 *    Revised by: Charles Stevenson
 *
 *
 *
 * Original environment:
 * Arduino 1.0.5 - Roto Libraries xxxx.xx.xx
 *
 * Revision notes:
 *  02/12/2018 :: Overhaul of code into an FSM. 
 *
 *
 *
 *
 */

#define versionString "Roto - [Akron Pyschology] [Moron Test]"

// Libraries:
#include <digits.h>
#include <alarmClock.h>
#include <buttonBoard.h>

//7seg
bool sentinel = true;
const int relay = 4;

// Pin Assignments:
const byte bbDI = 8;
const byte bbDO = 9;
const byte bbCLK = 10;
const byte bbILT = 11;
const byte bbOLT = 12;
const byte bbCount = 1;
const byte segData1 = 14;
const byte segClk1 = 15;
const byte segLat1 = 16;
uint8_t const NUM_SEG = 3;

// Constants: 
uint8_t const NUM_STATES = 3;
uint8_t const NUM_CLOCKS = 3;
uint8_t const lenDisplay = 3;
uint8_t const DISPLAY_MAX = 180; //Change type if exceeds 255;
uint8_t const dp = 0;

//timers
uint32_t const ATTRACT_INTERVAL = 100;
uint32_t const PLAY_INTERVAL = 1000;
uint32_t const DEBOUNCE_INTERVAL = 100;

// Global Variables:
uint32_t numDisplay = 0;

//states
bool attractActive = true;
bool gameActive = false;
bool const buttonActive = true; //always true
bool* states[NUM_STATES] = {&attractActive, &gameActive, &buttonActive};

//init objects:
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, bbCount);
digits dig1 = digits(segData1, segClk1, segLat1, NUM_SEG); //create object of class sevenSegment
digitGroup seg1 = digitGroup(&dig1,0,NUM_SEG);

//prototypes
void sexify();
void play();
void buttonCheck();

//alarmClocks
alarmClock attractClk = alarmClock(sexify);
alarmClock playClk = alarmClock(play);
alarmClock buttonCheckClk = alarmClock(buttonCheck);

alarmClock clocks[NUM_CLOCKS] = {attractClk, playClk, buttonCheckClk};
uint32_t const timers[NUM_CLOCKS] = {ATTRACT_INTERVAL, PLAY_INTERVAL, DEBOUNCE_INTERVAL};



/* =========    INITIAL SETUP    ================*/


/*
 * 
 *  FxN sevenSegMagic
 *    New intergalactic protocol that 
 *    sets up space-age revision 7segs!!
 *    
 */

void sevenSegSetup(){
  digitalWrite(relay, LOW);
  delay(1000);
  digitalWrite(relay, HIGH);
  delay(1000);
  for (byte i = segData1; i < segLat1; i++){
    digitalWrite(i,LOW);
  }
  delay(100);//obligatory delay...
  for (byte i = segData1; i < segLat1; i++){
    digitalWrite(i,HIGH);
  }
}

/*
*
* FxN :: setup
*   prints inital info; setup segment displays
*
*/
void setup(){
  pinMode(relay,OUTPUT);
  Serial.begin(9600);
  Serial.println(versionString);
  Serial.print("File: ");
  Serial.println(__FILE__);
  bb.setLamp(0,HIGH);
}


/* =========    HELPER FUNCTIONS    ================*/

/*
*
* FxN :: reset
*   resets gameState to initial state
*
*/
void reset(){
  bb.setLamp(0,HIGH);
  attractActive = true;
  gameActive = false;
  numDisplay = 0;
}

/*
*
* FxN :: playSound
*   @param : pin = pin to turn on to play a sound 
*   plays a sound 
*
*/
void playSound(byte pin){
      bb.setLamp(pin, HIGH);
      delay(500);
      bb.setLamp(pin, LOW);
}

/*
*
* FxN :: clearDisplay
*   clears 7Seg display
*
*/
void clearDisplay(){
    seg1.segDisp(blank);
    dig1.copySection(0, lenDisplay, lenDisplay);
    dig1.update();
}

/*
*
* FxN :: updateDisplay
*   updates the 7Seg display
*
*/
void updateDisplay(){
    seg1.segDisp(numDisplay,dp);
    dig1.copySection(0, lenDisplay, lenDisplay);
    dig1.update();
}

/*
*
* FxN :: sexify
*   attraction function
*
*/
void sexify() {
  seg1.chaseAnimation8();
}


/* =========    GAME ACTIVE    ================*/


/*
*
* FxN :: play
*   play logic when gameState is active
*
*/
void play(){
  if(loseState()){
    loseAnim();
  }
  else{
    updateDisplay();
  }
}

/*
*
* FxN :: lostState
*   checks to see if the time expired;
*   if time expired, turn off active game State
*   else updateDisplay
*   
*/
bool loseState(){
    bool lose = false;
    numDisplay++;
    if(numDisplay > DISPLAY_MAX){
      gameActive = false;
      lose = true;
    }
    else{
      updateDisplay();
    }
    return lose;
}

/*
*
* FxN :: lostAnim
*   executed losing animation 
*
*/
void loseAnim(){
      numDisplay = DISPLAY_MAX;
      playSound(4); //4 denotes pin the sound_file is on.
      reset();
}

/*
*
* FxN :: winState
*   playsWin animation and activates a winState
*
*/
void winState(){
  blinkScore();
  reset();
}

/*
*
* FxN :: blinkScore
*   blinks a winning score
*
*/
void blinkScore() {
  uint8_t const NUM_BLINKS = 5;
  for(uint8_t i = 0; i < NUM_BLINKS; i++) {
    clearDisplay();
    delay(500);
    updateDisplay();
    delay(500); 
  }
  delay(3000);
}

/* =========    GAME STATE CONTROLLERS    ================*/
/*
*
* FxN :: alarmClockRoutine
*   basically finite state machine controller using callbacks && states.
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
*
* FxN :: buttonCheck
*   checks if a button is pressed and the response depending on gameState
*
*/
void buttonCheck(){
  if (bb.getButton(0)){
    if (gameActive){
      winState();
	  delay(500);
    }
    else{
      attractActive = false;
	  bb.setLamp(0,LOW);
      gameActive = true;
	  delay(1000);
    }

  }
}

/*
*
* FxN :: loop
*
*
*/
void loop(){
  if (sentinel){
    sevenSegSetup();
    sentinel = false;
  }
  alarmClockRoutine();
}





