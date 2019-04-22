/*
   Roto

 Project: High Museum of Art
 Exhibit: Daydream Builder
 Author(s): Charles Stevenson/Caleb Shuey
 Date: 8/13/2018

 Original environment:
 Arduino 1.8.2

 Revision notes:
 */


#include <alarmClock.h>
#include <buttonBoard.h>
#include <outputExtend.h>
#include <SoftwareSerial.h>

//Constants
const int movies[] = {0,1,2,3};
const int movieButtons[] = {8,9,10,11};

//variables
int activeBTN = -1;
int audioActive[] = {0,0,0,0,0,0,0,0};
int buttonActive[] = {0,0,0,0,0,0,0,0};

//BUTTON BOARD Config
const uint8_t bbDI = 12; //button board data in
const uint8_t bbDO = 11; //button board data out
const uint8_t bbCLK = 10; //button board clock
const uint8_t bbILT = 9; //IN LATCH
const uint8_t bbOLT = 8; //OUT LATCH
const uint8_t nBB = 2; //number of button boards
const uint8_t nBUTTONS = 12; // number of buttons
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, nBB);//init bb
buttonSelect select = buttonSelect(&bb,8,4,false);

//Output Extend Config
const uint8_t oeData = 4;
const uint8_t oeClock = 5;
const uint8_t oeLatch = 6;
const uint8_t nOE = 1;
outputExtend oe = outputExtend(oeData, oeClock, oeLatch, nOE);

void Check();

//AlarmClock
repeatAlarm check = repeatAlarm(Check);

//Software Serial
#define rx 3
#define tx 2
SoftwareSerial MIDISerial(rx,tx);

void testButtons(){
  bb.setLamp(HIGH);
  delay(1000);
  bb.setLamp(LOW);
}

void setup(){
  Serial.begin(9600);
  testButtons();
  check.setInterval(50);
  MIDISerial.begin(31250);
}

void loop(){
  check.poll();
  select.poll();
  if(select.event() == true){
    activeBTN = select.getState();
    oe.extendedWrite(movies[activeBTN], HIGH);
    delay(100);
    oe.extendedWrite(movies[activeBTN], LOW);
  }
}

void Check(){
  for(int i =0;i<8;i++){
    if(bb.getButton(i) == true && buttonActive[i] == 0){
      buttonActive[i] = 1;
      if(audioActive[i] == 0){
        bb.setLamp(i, HIGH);
        audioActive[i] = 1;
        sendOn(i);
      }
      else if(audioActive[i] != 0){
        bb.setLamp(i, LOW);
        audioActive[i] = 0;
        sendOff(i);
      }
    }
    else if(bb.getButton(i) == false){
      buttonActive[i] = 0;
    }
  }
}

void sendOn(int channel){
  channel = channel+1;
  MIDISerial.write(0x90);
  MIDISerial.write(channel);
  MIDISerial.write(127);
}

void sendOff(int channel){
  channel = channel+1;
  MIDISerial.write(0x80);
  MIDISerial.write(channel);
  MIDISerial.write(127);
}


