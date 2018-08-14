


#include "FastLED.h"

#define LED_TYPE    WS2801
#define COLOR_ORDER RGB


#define ON_RED(i) CRGB(255, 0 , 0)
#define ON_GREEN(i) CRGB(0,255,0)

int MAX_COLOR_VAL = 255;
byte const NUM_LEDS = 4;
CRGB leds[NUM_LEDS];

byte counter = 0;
boolean red = true;

int const DATA_PIN = 8;
int const CLOCK_PIN = 9;
int const LATCH_PIN = 10;

void setup(){
		Serial.begin(9600);
		FastLED.addLeds<LED_TYPE, DATA_PIN,CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // tell FastLED about the LED strip configuration
  FastLED.setBrightness(255); // set master brightness control

}


void updateLED(){
		
		for (byte i = 0; i < NUM_LEDS; i++){
		       if (counter == 0){
						leds[i] = CRGB::Red;
				}
				else if (counter == 1){
						leds[i] = CRGB::Green;
				}
				else if (counter == 2){
						leds[i] = CRGB::Blue;
				}
		}
		if (counter == 0){
				Serial.println("Red");
				}
		else if (counter == 1){
				Serial.println("Green");
		}
		else if (counter == 2){
				Serial.println("Blue");

		}
		FastLED.show();
		counter = (counter + 1) % 3;
		delay(3000);
}

void loop(){
		updateLED();
}
