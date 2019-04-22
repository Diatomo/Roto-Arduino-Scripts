


#include <FastLED.h>

CRGB leds[50];


void setup(){

	FastLED.addLeds<NEOPIXEL,9>(leds, 50);
	
	for (uint8_t i = 0; i < 50; i++){
		leds[i].setRGB(255,0,150);
	}
	FastLED.show();

}

void loop(){

}

