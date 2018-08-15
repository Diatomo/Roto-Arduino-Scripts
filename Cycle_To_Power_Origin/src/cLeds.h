
#ifndef cLeds_h
#define cLeds_h


#include <FastLED.h>

class CLeds{
	public:
		CLeds();
		void update(uint8_t);
		void animChase(uint8_t);
		void animTest(uint8_t, uint8_t);
		void animMultiplier(uint8_t, uint8_t);
		void turnoffLeds(uint8_t);
		void testLeds();
		void lightMultiplier(uint8_t);
		void animOuter(uint8_t, uint8_t);
		void quietOff();
		void transmit();
	private:
		uint8_t static const NUM_MULTIPLIERS = 8;
		uint8_t static const NUM_NEO_PINS = 8;
		int32_t static const NUM_LEDS = 150;

		uint8_t static const DIM_OFFSET = 20;
		uint8_t static const LIGHT_OFFSET = 3;
		uint8_t static const MULTI_OFFSET = 3;
		uint8_t static const MULTI_LIGHTS = 45;

		uint32_t outer;
		uint32_t inner;
		
		CRGB leds[150];
};
#endif
