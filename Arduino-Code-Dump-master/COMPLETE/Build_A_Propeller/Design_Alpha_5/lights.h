

/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

#include "stats.h"
#include "FastLED.h"

#ifndef lights_h
#define lights_h

class Lights{

		public:
				Lights();
				void set_FastLED();
				void updateLED(Sensors);
				void lightLED(Sensors, int);
				byte static const NUM_LEDS = 16;
				CRGB leds[NUM_LEDS];
		private:
				void idleLED(Sensors);
				byte static const BRIGHTNESS = 96;
				byte static const MAX_COLOR_VAL = 255;
				byte ledItr = 0;
				byte static const DATA_PIN = 8; 
};

#endif
