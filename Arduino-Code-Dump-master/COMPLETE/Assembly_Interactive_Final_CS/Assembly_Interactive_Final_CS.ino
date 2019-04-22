/*
 * Roto
 *
 * Project: Akron Psychology
 *
 *
 * Exhibit: Assembly Interactive
 *
 *
 * Author(s):
 * Caleb Shuey
 *
 *
 * Original environment:
 * Arduino 1.0.6 - Roto Libraries xxxx.xx.xx
 *
 * Revision notes:
 *
 *
 *
 *
 */

#define versionString "Roto - [Akron Psychology] [Assembly Interactive]"

// Debug Mode (comment to disable)
//#define debugMode
bool sentinel = true; //variable for 7seg
const int relay = 3;

// Libraries:
#include <digits.h>
#include <alarmClock.h>
#include <buttonBoard.h>

// Pin Assignments:

const int BTN[] = {0,1,2};
const int LED_Panel[] = {4,5,6};
const int audio[] = {4,5,6,7};
const int timeLength[] = {20,40,60};

// Constants:
#define numberOfBoards 1
#define segNumber 6 //number of total digits
#define displayLength 2
#define dp 0
#define updateDisplayIntervalClock 1000
#define timeInterval 2000

const byte bbDI = 8;
const byte bbDO = 9;
const byte bbCLK = 10;
const byte bbILT = 11;
const byte bbOLT = 12;
const byte bbCount = 1;

const byte segData1 = 14;
const byte segClk1 = 15;
const byte segLat1 = 16;

// Global Variables:
boolean gameStart = false;
byte gameSection = 1;
int state = 0;
int count = 0;
unsigned long updateDisplayInterval;
unsigned long restartTimer = 30000;

void attractor();
void restarted(); 
// Global Classes:
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, bbCount);

digits dig1 = digits(segData1, segClk1, segLat1, segNumber); //create object of class sevenSegment
digitGroup seg1 = digitGroup(&dig1,0,displayLength);
digitGroup seg2 = digitGroup(&dig1,2,displayLength);
digitGroup seg3 = digitGroup(&dig1,4,displayLength);
digitGroup attract = digitGroup(&dig1,0,segNumber);

repeatAlarm attractMe = repeatAlarm(attractor);
alarmClock restart = alarmClock(restarted);

void startup(){
  for(int i=0;i<7;i++){
   bb.setLamp(i, HIGH);
   delay(200);
   bb.setLamp(i, LOW); 
  }
}

void sevenSegSetup(){
  bb.setLamp(relay, LOW);
  delay(1000);
  bb.setLamp(relay, HIGH);
  delay(1000);
  for (byte i = segData1; i < segLat1; i++){
    digitalWrite(i,LOW);
  }
  delay(100);//obligatory delay...
  for (byte i = segData1; i < segLat1; i++){
    digitalWrite(i,HIGH);
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println(versionString);
  Serial.print("File: ");
  Serial.println(__FILE__);
  for(int i=0;i<4;i++){
   pinMode(audio[i], OUTPUT);
   digitalWrite(audio[i], LOW); 
  }
  startup();
  attractMe.setInterval(100);
}

void loop(){
  if (sentinel){
    sevenSegSetup();
    sentinel = false;
  }
  restart.poll();
  // Attract
  if (gameStart == false && state == 0) {
    attractMe.poll();
    bb.setLamp(BTN[state], HIGH);
  }
  
  // Starts each section
  if (bb.getButton(BTN[state]) && gameStart == false) {
    restart.unSetAlarm();
    gameStart = true;
    bb.setLamp(BTN[state], LOW);
    bb.setLamp(LED_Panel[state], HIGH);
    seg1.segDisp(blank);
    seg2.segDisp(blank);
    seg3.segDisp(blank);
    dig1.update();
  }
  
  // Sets Timers
  if(gameStart){
    switch(state){
    case 0:
      timerTick(timeLength[state]); 
      break;
    case 1:
      timerTick(timeLength[state]); 
      break;
    case 2:
      timerTick(timeLength[state]);
      break;
     }
  }
}


// Runs timer and plays audio
void timerTick(byte time) {
  int countdownTimer = time;

  //play Audio
  digitalWrite(audio[state], HIGH);
  delay(500);
  digitalWrite(audio[state], LOW);

  //delay for the length of each clip
  delay(3000);
  
  
   while (countdownTimer > 0 ) { // Countdown timer
//     Serial.println(state);
     switch(state){
       case 0:
        seg1.segDisp(countdownTimer, dp);
        dig1.update();
        break;
      case 1:
        seg2.segDisp(countdownTimer, dp);
        dig1.update();
        break;
      case 2:
        seg3.segDisp(countdownTimer, dp);
        dig1.update();
        break;
     }
    delay(1000);
    countdownTimer--;
  }
   if(countdownTimer <= 0){ // Moves to the next state of play, or back to attract
    if(state == 0){
//      Serial.println("End1");
      seg1.segDisp(countdownTimer, dp);
      dig1.update();
      gameStart = false;
      bb.setLamp(LED_Panel[state], LOW);
      state = 1;
      bb.setLamp(BTN[state], HIGH);
      restart.setAlarm(restartTimer);
    }
    else if(state == 1){
//      Serial.println("End2");
      seg2.segDisp(countdownTimer, dp);
      dig1.update();
      gameStart = false;
      bb.setLamp(LED_Panel[state], LOW);
      state = 2;
      bb.setLamp(BTN[state], HIGH);
      restart.setAlarm(restartTimer);
    }
    else if(state == 2){
//      Serial.println("End3");
      seg3.segDisp(countdownTimer, dp);
      dig1.update();
      bb.setLamp(LED_Panel[state], LOW);
          
      //Play the game ending instructions and wait for them to be over
      digitalWrite(audio[3], HIGH);
      delay(100);
      digitalWrite(audio[3], LOW);
      delay(5000);
      
      gameStart = false;
      state = 0;
    }
  }
}


void restarted(){ // Restarts game if visitor leaves mid game
  state = 0;
  for (int i = 0; i < 8; i++){
		if (i != relay){
				bb.setLamp(i,LOW);

		}
  }
}  


void attractor() { // 7-Seg attract animation
  attract.chaseAnimation8();
}
