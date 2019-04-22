/*
 * Roto
 *
 * Project: High Museum
 *
 *
 * Exhibit: Color Cavern
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

#define versionString "Roto - [Project] [Exhibit]"

// Debug Mode (comment to disable)
//#define debugMode

// Libraries:
#include <FastLED.h>
#include <DmxSimple.h>


uint8_t const NUMLEDS = 1;
		
CHSV aesthetic[NUMLEDS];
CHSV aestheticDIM[NUMLEDS];
CRGB leds[NUMLEDS];
CRGB ledsDIM[NUMLEDS];



void setup(){
	Serial.begin(9600);
	DmxSimple.usePin(3);
	DmxSimple.maxChannel(64);
	DmxSimple.usePin(17);
	DmxSimple.maxChannel(64);
	aesthetic[0].setHSV(0,255,255);
	aestheticDIM[0].setHSV(0,250,200);
}


/*
 *
 * FxN :: rainbow
 *
 */
void rainbow(){
	aesthetic[0].hue++;
	aestheticDIM[0].hue++;
	hsv2rgb_rainbow(aesthetic[0], leds[0]);
	hsv2rgb_rainbow(aestheticDIM[0], ledsDIM[0]);
	delay(50);
	FastLED.show();
}

void loop(){
	rainbow();
	DmxSimple.usePin(3);
	DmxSimple.write(4,leds[0].r);
	DmxSimple.write(5,leds[0].g);
	DmxSimple.write(6,leds[0].b);
	DmxSimple.write(3,ledsDIM[0].b);
	DmxSimple.write(2,ledsDIM[0].g);
	DmxSimple.write(1,ledsDIM[0].r);
}
