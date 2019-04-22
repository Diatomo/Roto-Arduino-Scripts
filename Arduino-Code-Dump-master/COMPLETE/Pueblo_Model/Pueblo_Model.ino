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

/* FETS Assignments */
const byte fet4 = 4;
const byte fet5 = 5;
const byte fet6 = 6;
const byte fet7 = 7;


/* BUTTON CONFIGURATION */
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


/*
const byte DATA_PIN_2 = 18;
const byte CLOCK_PIN_2 = 19;
const byte DATA_PIN_3 = 4;
const byte CLOCK_PIN_3 = 5;
const byte DATA_PIN_4 = 6;
const byte CLOCK_PIN_4 = 7;
*/


// Constants:
const byte bbCount = 1;
const byte numberOfBoards = 1;

const int NUM_LEDS = 25;

CRGB leds[NUM_LEDS];


// Global Classes:
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, bbCount);
outputExtend oe = outputExtend(dataPin, clockPin, latchPin, numberOfBoards);

//global variables.
byte toggleButton = 0;
bool toggle = false;
byte houseLights = 0;

void setup(){
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
  FastLED.addLeds<WS2801, DATA_PIN_1, CLOCK_PIN_1>(leds, NUM_LEDS);

  // Set outputs to low
  oe.byteWrite(0, 0x00);
  bb.setLamp(HIGH);
}

void loop(){
	if (pollButton()) { toggleLEDs(); }
	updateLeds();
}

bool pollButton(){
	return bb.getButton(toggleButton);
}

void toggleLEDs(){
	for (int i = 0; i < NUM_LEDS; i++){
		if (toggle){
			leds[i] = CRGB::White;
		}
		else{
			leds[i] = CRGB::Black;
		}
	}
	oe.extendedWrite(houseLights,toggle);
	toggle = !toggle;
}

void updateLeds(){
	FastLED.show();
	delay(500);
}
