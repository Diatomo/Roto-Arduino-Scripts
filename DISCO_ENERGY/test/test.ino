

#include <FastLED.h>

CRGB leds[16];

CHSV hsv[1];

void setup(){
	FastLED.addLeds<NEOPIXEL,4>(leds, 16);
}


void rainbow(){
	for (uint8_t i = 0; i < 16; i++){
		leds[i].setRGB(255,255,0);
	}
	FastLED.show();
}

void loop(){
	rainbow();
	delay(1000);

}
