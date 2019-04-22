/*
 * Roto
 *
 * Project: NMAI
 *
 *
 * Exhibit: Pueblo Model
 *
 * Library:
 * buttonBoard, alarmClock, FastLED, OutputExtend
 *
 * Author(s):
 * Brady Schoeffler
 *
 */

#define versionString "Roto - [NMAI] [Pueblo Model]"

// Libraries:
#include <alarmClock.h>
#include <outputExtend.h>
#include <buttonBoard.h>
#include <FastLED.h>

// Pin Assignments:
const byte fet4 = 4;
const byte fet5 = 5;
const byte fet6 = 6;
const byte fet7 = 7;

const byte bbDI = 8;
const byte bbDO = 9;
const byte bbCLK = 10;
const byte bbILT = 11;
const byte bbOLT = 12;

const byte dataPin = 14;
const byte clockPin = 15;
const byte latchPin = 16;

const byte DATA_PIN_1 = 2;
const byte CLOCK_PIN_1 = 3;
const byte DATA_PIN_2 = 18;
const byte CLOCK_PIN_2 = 19;
const byte DATA_PIN_3 = 4;
const byte CLOCK_PIN_3 = 5;
const byte DATA_PIN_4 = 6;
const byte CLOCK_PIN_4 = 7;
// Constants:
const byte bbCount = 1;
const byte numberOfBoards = 1;

#define NUM_STRIPS 4
#define NUM_LEDS_PER_STRIP 17

CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];

// Global Variables:
int j = 0;
boolean heatFlow;
boolean rainFlow;
boolean summerSun;
boolean winterSun;
boolean btnStates[6] = {
  false,false,false,false,false,false};

// Global Classes:
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, bbCount);
outputExtend outputs = outputExtend(dataPin, clockPin, latchPin, numberOfBoards);

//prototypes
void pollButtons();
void resetButtons();

repeatAlarm pollTimer = repeatAlarm(pollButtons); // calls the function pollButtons() on a set interval
alarmClock selectTimeout = alarmClock(resetButtons);

buttonSelect select = buttonSelect(&bb, 0, 6, true); // this is a helper class for a one of many group of buttons
// Called as buttonSelect( [pointer to buttonBoard], [starting button number], [number of buttons], [reset state] );


void setup()
{
  Serial.begin(9600);
  Serial.println(versionString);
  Serial.print("File: ");
  Serial.println(__FILE__);

  pinMode(fet4, OUTPUT);
  pinMode(fet5, OUTPUT);
  pinMode(fet6, OUTPUT);
  pinMode(fet7, OUTPUT);
  digitalWrite(fet4, LOW);
  digitalWrite(fet5, LOW);
  digitalWrite(fet6, LOW);
  digitalWrite(fet7, LOW);
  FastLED.addLeds<WS2801, DATA_PIN_1,CLOCK_PIN_1>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2801, DATA_PIN_2,CLOCK_PIN_2>(leds[1], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2801, DATA_PIN_3,CLOCK_PIN_3>(leds[2], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2801, DATA_PIN_4,CLOCK_PIN_4>(leds[3], NUM_LEDS_PER_STRIP);

  // Set outputs to low
  outputs.byteWrite(0, 0x00);
  bb.setLamp(LOW);
  delay(500);
  bb.setLamp(HIGH);
  delay(500);
  outputs.update();
  pollTimer.setInterval(10);
  ledstest();
}

void loop()
{
  pollTimer.poll();
  selectTimeout.poll();
  updateLEDs();
  /*
  Serial.print("heat flow:  ");
 Serial.print(heatFlow); 
   Serial.print("      rain flow:  ");
 Serial.print(rainFlow); 
   Serial.print("      winter sun:  ");
 Serial.print(winterSun); 
   Serial.print("      summer sun:  ");
 Serial.println(summerSun); */
}

void chaseLED(uint32_t color, byte pos, int flip) {
  if (j > 6) j = 0;
  for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
    if ((i+(j*flip))%6==0) leds[pos][i] = color;
    leds[pos][i].nscale8(175);
  }
  FastLED.show();
  j++;
}    


void ledsOff(byte pos) {
  for (int i=0; i < NUM_LEDS_PER_STRIP; i++) {
    leds[pos][i] = 0x000000;
  }
  FastLED.show();
}


void pollButtons()
{
  if (select.poll()) {
      outputs.byteWrite(0, 0x00);
    updateLEDs();
    selectTimeout.setAlarm(5000); // reset the timeout timer if the group of buttons has changed state
  }
}

void resetButtons()
{
  select.setState(buttonReset);
  for (int i = 0; i < 5; i++) {
    btnStates[i] = false;
  }
  rainFlow = false;
  heatFlow = false;
  winterSun = false;
  summerSun = false;
  outputs.byteWrite(0, 0x00);
}

void updateLEDs()
{
  int i = select.getState();
      outputs.extendedWrite(i, HIGH);
      if (i==2) heatFlow = true, rainFlow = false, winterSun = true, summerSun = false;
      else if (i==3) summerSun = true, heatFlow = false, rainFlow = false, winterSun = false;
      else if (i==5) rainFlow = true, heatFlow = false, winterSun = false, summerSun = false;
      else heatFlow = false, rainFlow = false, winterSun = false, summerSun = false;
  if (heatFlow) chaseLED(0xFF4500,0,-1);
  else ledsOff(0);
  if (rainFlow) chaseLED(0x0050FA,1,1);
  else ledsOff(1);
  if (winterSun) chaseLED(0xFFD900,2,1);
  else ledsOff(2);
  if (summerSun) chaseLED(0xFFD900,3,1);
  else ledsOff(3);
  delay(75);
}

void ledstest()
{
  for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
    leds[0][i] = 0xFF0000;
    leds[1][i] = 0xFF0000;
    leds[2][i] = 0xFF0000;
    leds[3][i] = 0xFF0000;
  }
  FastLED.show();
  delay(500);
  for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
    leds[0][i] = 0x00FF00;
    leds[1][i] = 0x00FF00;
    leds[2][i] = 0x00FF00;
    leds[3][i] = 0x00FF00;
  }
  FastLED.show();
  delay(500);
  for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
    leds[0][i] = 0x00FF00;
    leds[1][i] = 0x00FF00;    
    leds[2][i] = 0x00FF00;
    leds[3][i] = 0x00FF00;
  }
  FastLED.show();
  delay(500);
}





