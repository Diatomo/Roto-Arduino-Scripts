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
#include <FastLED.h>

const byte NEOPIN = 9;

// Constants:
const byte bbCount = 1;
const byte numberOfBoards = 1;

const int NUM_LEDS = 25;

CRGB leds[NUM_LEDS];
CHSV aes[NUM_LEDS];

void setup(){
	FastLED.addLeds<NEOPIXEL, NEOPIN>(leds, NUM_LEDS);
	for (uint32_t i = 0; i < NUM_LEDS; i++){
		aes[i].setHSV(0,255,255);
	}
}

void loop(){
	rainbow();
}

void toggleLEDs(){
	for (int i = 0; i < NUM_LEDS; i++){
			leds[i] = CRGB::White;
	}
}

void rainbow(){
	for (uint32_t i = 0; i < NUM_LEDS; i++){
		if (aes[i].value == 0) { aes[i].value = 255; }	
		aes[i].hue++;
		hsv2rgb_rainbow(aes[i], leds[i]);
	}
	delay(50);
	FastLED.show();

}

void updateLeds(){
	FastLED.show();
	delay(500);
}
