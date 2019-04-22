

/*
 *
 *
 *
 *
 *
 *
 *
 */


#include "FastLED.h"
#include "sensors.h"
#include "lights.h"

#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define OFF CRGB(0,0,0)
#define ON(i) CRGB(MAX_COLOR_VAL - i*(i), i*i, 0)


Lights::Lights(){

}

void Lights::set_FastLED(){
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

/*
 * 
 * Fxn updateLED
 *  updates LED iterator variable
 * 
 */
void Lights::updateLED(Sensors stats){
		Serial.print(stats.avg);
    int avg = constrain((map(stats.avg, stats.minimum, (stats.maximum - stats.dev),0,NUM_LEDS)),0,NUM_LEDS);
	//adjust iterator based on normalized avg
	
    if (avg < ledItr || (ledItr == 1 && avg == 1)){
      if (ledItr != 0){
        ledItr--;
      }
    }
    else if (avg > ledItr){
      if (ledItr <= NUM_LEDS){
        ledItr = (ledItr + 1);
      }
    }
}


/*
*
* Fxn :: lightLED
*	lights up leds based on updates ledItr
*
*
*/
void Lights::lightLED(Sensors stats, int buttonState){
		if (buttonState == 0){
		  for (int i=0; i < ledItr; i++) {	
				leds[i] = ON(i);//turn on leds
		  }
		}
		for (int i=ledItr;i<NUM_LEDS;i++){
				if (ledItr != 0){
						leds[i] = OFF;//turn off leds
				}
		}
		if (ledItr == 0 && buttonState == 1){
			idleLED(stats);
		}
		FastLED.show();
		delay(100);
}


/*
*
* Fxn :: idleLED
*	idleLED routine
*
*
*/
void Lights::idleLED(Sensors stats){
  leds[0] = OFF;
  FastLED.show();
  delay(500);
  leds[0] = ON(0);
  FastLED.show();
  delay(500); 
  stats.minimum = min(stats.minimum,stats.avg);
}

