/*
 * Roto
 *
 * Project: Akron Psychology
 *
 *
 * Exhibit: Reaction Time
 *
 *
 * Author(s): Brady Schoeffler
 *
 *
 *
 * Original environment:
 * Arduino 1.0.5 - Roto Libraries xxxx.xx.xx
 *
 * Revision notes:
 *
 *
 *
 *
 */

#define versionString "Roto - [Akron Psychology] [Reaction Time]"

// Debug Mode (comment to disable)
//#define debugMode

// Libraries:
#include <digits.h>
#include <alarmClock.h>
#include <buttonBoard.h>

// Pin Assignments:
const byte startButton = 0;
const byte gameButton = 1;
const byte bbDI = 8;
const byte bbDO = 9;
const byte bbCLK = 10;
const byte bbILT = 11;
const byte bbOLT = 12;
const byte bbCount = 1;
const byte segData1 = 14;
const byte segClk1 = 15;
const byte segLat1 = 16;

// Constants:
#define numberOfBoards 1
#define segNumber 3 //number of total digits
#define displayLength 3
#define dp 3
#define updateDisplayIntervalClock 1000
#define timeInterval 5000

// Global Variables:
boolean gameStart = false;
boolean earlyTrip = false;
byte gameSection = 0;
int count = 0;
unsigned long updateDisplayInterval;
unsigned long startCounter;
unsigned long deadTimer;
unsigned long maxTimer;
unsigned long debounceTimer;
int debounceInterval = 250;
int reactionTimeout = 3000;
int reactionTimes[5];
int averageTime;
byte averagePointer;
int randomDelay;

boolean cat = true;

// Global Classes:
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, bbCount);
digits dig1 = digits(segData1, segClk1, segLat1, segNumber); //create object of class sevenSegment
digitGroup seg1 = digitGroup(&dig1,0,displayLength);


void attractor();
repeatAlarm attractMe = repeatAlarm(attractor);


/*
 * 
 *  FxN sevenSegMagic
 *    New intergalactic protocol that 
 *    sets up space-age revision 7segs!!
 *    
 */

void sevenSegMagic(){
  digitalWrite(4,LOW);
  for (byte i = segData1; i < segLat1; i++){
    digitalWrite(i,LOW);
  }
  delay(1000);//obligatory delay...
  for (byte i = segData1; i < segLat1; i++){
    digitalWrite(i,HIGH);
  }
  delay(500);//obligatory delay...
  for (byte i = segData1; i < segLat1; i++){
    digitalWrite(i,LOW);
  }
  delay(500);//obligatory delay...
  digitalWrite(4,HIGH);
}

void setup()
{
  
  pinMode(4,OUTPUT);
  Serial.begin(9600);
  Serial.println(versionString);
  Serial.print("File: ");
  Serial.println(__FILE__);
  attractMe.setInterval(100);
}

bool earlyPress(){
	bool early = false;
	if(bb.getButton(gameButton) && millis() > debounceTimer) {
	  early = true;
	  debounceTimer = debounceInterval + millis();
	}
	return early;
}

void updateDisplay(){
	  seg1.segDisp(reactionTimes[averagePointer]/10, dp);
	  dig1.copySection(0, displayLength, displayLength);
	  dig1.update();
}

bool startGame(){
	bool gameInit = false;
	if (bb.getButton(startButton) && !gameStart) {
		debounceTimer = debounceInterval + millis();
		averagePointer = 0;
		seg1.segDisp(0, dp);
		dig1.copySection(0, displayLength, displayLength);
		dig1.update();
		bb.setLamp(startButton, LOW);
		bb.setLamp(gameButton, LOW);
		gameInit = true;
	}
	return gameInit;

}

void attract(){
	attractMe.poll();
	bb.setLamp(startButton, HIGH);
}

void play(){
	while(averagePointer < 5) {
		randomDelay = random(5000,7000); // Random delay between 5 and 7 seconds
		deadTimer = millis() + randomDelay;
		while(millis() < deadTimer) {
			earlyTrip = earlyPress();
			if(earlyTrip){
				break;
			}
		}

	if(!earlyTrip) {
		bb.setLamp(gameButton, HIGH);
		startCounter = millis();
		maxTimer = startCounter + reactionTimeout;

		// Wait for either button press or timeout
		while(!bb.getButton(gameButton) && millis() < maxTimer) {}

		bb.setLamp(gameButton, LOW);
		debounceTimer = debounceInterval + millis();

		//record reaction time
		reactionTimes[averagePointer] = millis() - startCounter;
	}

	else{
		reactionTimes[averagePointer] = reactionTimeout;
	}
	updateDisplay();
	if (earlyTrip){
		Serial.print("averagePointer ::");
		Serial.println(reactionTimes[averagePointer]);
		blinkScore(reactionTimes[averagePointer], dp, 300, 5, 0);
	}
	averagePointer++;
	if (averagePointer < 5 && earlyTrip){
		updateDisplay();
	}
	earlyTrip = false;
	}
}

void win(){
	averageTime = (reactionTimes[0] + reactionTimes[1] + reactionTimes[2] + reactionTimes[3] + reactionTimes[4]) / 5;
	gameStart = false;
	Serial.println(averageTime);
	blinkScore(averageTime/10, dp, 500, 3, 3000);
	bb.setLamp(startButton, true);
	averagePointer = 0;
	for (int i = 0; i < 5; i++){
		reactionTimes[i] = 0;
	}
}

void loop(){
	//7 seg, reset mechanism because of variation between chip to chip. 
	if (cat==true){
		cat=false;
		sevenSegMagic();
	}
	if (startGame()){
		play();
	}
	else{
		attract();
	}
	if (averagePointer >= 5){
		win();
	}
}

void blinkScore(int value, int decimal, int interval, int counter, int delayTime) {
		Serial.print("Value :: ");
		Serial.println(value);
		if (value >= 1000){
				value = value / 10;
		}
  for(int i = 0; i < counter; i++) {
    seg1.segDisp(blank);
    dig1.copySection(0, displayLength, displayLength);
    dig1.update();

    delay(interval);

    seg1.segDisp(value,decimal);
    dig1.copySection(0, displayLength, displayLength);
    dig1.update();

    delay(interval);
  }
  seg1.segDisp(value,decimal);
  dig1.copySection(0, displayLength, displayLength);
  dig1.update();

  delay(delayTime);
}

void attractor() {
  seg1.chaseAnimation8();
  dig1.update();
}

