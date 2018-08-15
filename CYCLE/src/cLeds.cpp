
#include <Arduino.h>
#include "cLeds.h"
#include "alarmClock.h"
#include <Wire.h>

#define versionString "Roto - [Discovery World Energy] [Cycle to Power]"
#define COLOR_ORDER RGB

int32_t const LENGTH_LEDS[8] = {92,95,50,50,50,50,50,50};
uint8_t const neoPins[8] = {4,5,6,7,8,9,10,11};
uint8_t const MultiIndex[8] = { 0,5,10,15,20,25,30,35 };

uint32_t delay_Time = 30;

CLeds::CLeds(){

	/*
	 * FastLED seems to prefer a constant expression that I can only get to work
	 * with a literal.
	 */
	//constructors
	FastLED.addLeds<NEOPIXEL,4>(leds, LENGTH_LEDS[0]); 
	FastLED.addLeds<NEOPIXEL,5>(leds, LENGTH_LEDS[1]); 
	FastLED.addLeds<NEOPIXEL,6>(leds, LENGTH_LEDS[2]);//monkeys 
	FastLED.addLeds<NEOPIXEL,7>(leds, LENGTH_LEDS[3]);//optocopter 
	FastLED.addLeds<NEOPIXEL,8>(leds, LENGTH_LEDS[4]);//blender 
	FastLED.addLeds<NEOPIXEL,9>(leds, LENGTH_LEDS[5]);
	FastLED.addLeds<NEOPIXEL,10>(leds, LENGTH_LEDS[6]);//chainsaw
	FastLED.addLeds<NEOPIXEL,11>(leds, LENGTH_LEDS[7]);
	
	//constructors
	FastLED.addLeds<NEOPIXEL,4>(leds, LENGTH_LEDS[0]); 
	FastLED.addLeds<NEOPIXEL,5>(leds, LENGTH_LEDS[1]); 
	FastLED.addLeds<NEOPIXEL,6>(leds, LENGTH_LEDS[2]);//monkeys 
	FastLED.addLeds<NEOPIXEL,7>(leds, LENGTH_LEDS[3]);//optocopter 
	FastLED.addLeds<NEOPIXEL,8>(leds, LENGTH_LEDS[4]);//blender 
	FastLED.addLeds<NEOPIXEL,9>(leds, LENGTH_LEDS[5]);
	FastLED.addLeds<NEOPIXEL,10>(leds, LENGTH_LEDS[6]);//chainsaw
	FastLED.addLeds<NEOPIXEL,11>(leds, LENGTH_LEDS[7]);
	
	//pin declarations && constants;
	outer = 0;
	inner = 1;
	Wire.begin();
}


void CLeds::testLeds(){
	for (uint8_t i = 0; i< NUM_NEO_PINS; i++){
		animTest(0, i);
		//turnoffLeds();
	}
	delay(100);
}

void CLeds::update(uint8_t action){
	uint8_t device = action & 0x0F;
	uint8_t multiplier = (action & 0xF0) >> 4;
	animMultiplier(device, multiplier);
	quietOff();
	animOuter(device, multiplier);
	quietOff();
	FastLED[4].showLeds();
	animChase(device);
	turnoffLeds(device);//Testing only
	transmit();
}


//ANIMATION FUNCTIONS
//=====================================================================================

void CLeds::animTest(uint8_t device, uint8_t ledChase){
	uint8_t index = 0;
	while (index < LENGTH_LEDS[ledChase] + DIM_OFFSET){
		for (uint8_t i = 0; i < LENGTH_LEDS[ledChase]; i++){
			leds[i].fadeToBlackBy(32); // fade by 25% aka 64/256ths
		}
		if (index < LENGTH_LEDS[ledChase]){
			leds[index].setRGB(255, 0, 150);
		}
		delay(20);
		FastLED[ledChase].showLeds();
		index++;
	}
}

//===================================================================
//		Animation Functions
//===================================================================
//all of these are water-falling called one after the other. Which should be fine
//as at this moment in the program everything is locked out. meaning no signals should be
//passed between boards.

void CLeds::animMultiplier(uint8_t device, uint8_t multiplier){
	uint8_t section = (LENGTH_LEDS[inner] - MULTI_LIGHTS) / NUM_MULTIPLIERS;
	uint8_t index = 0;
	bool multiplierOn = false;
	while (index < (section * multiplier) + DIM_OFFSET){
		for (uint8_t i = 0; i < (section * multiplier); i++){
				leds[i].fadeToBlackBy(32); // fade by 25% aka 64/256ths
		}
		if (index < (section * multiplier) + LIGHT_OFFSET){
			leds[index].setRGB(150, 255, 0);
		}
		else{
			if (!multiplierOn){
				quietOff();
				lightMultiplier(multiplier);
				multiplierOn = true;
			}
		}
		delay(delay_Time);
		FastLED[inner].showLeds();
		index++;
	}
}

void CLeds::lightMultiplier(uint8_t multiplier){

	//get the start and end position. Which because the innerlights are connected to the multiplier lights
	//I subtract the added multiplier lights and then have to make up for their positions because of a wiring offset;
	uint8_t start = LENGTH_LEDS[inner] - MULTI_LIGHTS + MultiIndex[(multiplier + MULTI_OFFSET) % NUM_MULTIPLIERS];
	uint8_t end = LENGTH_LEDS[inner] - MULTI_LIGHTS + MultiIndex[(multiplier + MULTI_OFFSET) % NUM_MULTIPLIERS] + 5;
	for (uint8_t j = start; j < end; j++){
		leds[j].setRGB(150,255,0);
	}
	delay(delay_Time);
	FastLED[inner].showLeds();
}



void CLeds::turnoffLeds(uint8_t device){
		for (int i = 0; i < NUM_LEDS; i++){
			leds[i].setRGB(0,0,0);
		}
		FastLED.show();
}

void CLeds::animOuter(uint8_t device, uint8_t multiplier){
	int8_t section = (LENGTH_LEDS[outer] / NUM_MULTIPLIERS);
	int8_t start = section * multiplier;//((multiplier + MULTI_OFFSET) % NUM_MULTIPLIERS);
	Serial.print("start :: ");
	Serial.println(start);
	int8_t end = ((device>3) ? LENGTH_LEDS[outer] * 0.75 : LENGTH_LEDS[outer] * 0.25);
	if (start == 0) { start = LENGTH_LEDS[outer] - 1; }
	while (start != end){
		for (int8_t i = 0; i < LENGTH_LEDS[outer]; i++){
				leds[i].fadeToBlackBy(64); // fade by 25% aka 64/256ths
		}
		leds[start].setRGB(150,225,0);
		start--;
		if (start < 0){
			start = LENGTH_LEDS[outer] - 1;
		}
		FastLED[outer].showLeds();
		delay(delay_Time);
	}
}


void CLeds::animChase(uint8_t device){
	uint8_t index = 0;
	device += 2;
	Serial.print("Device :: ");
	Serial.println(device);
		while (index < LENGTH_LEDS[device]){
			for (uint8_t i = 0; i < LENGTH_LEDS[device]; i++){
					leds[i].fadeToBlackBy(32); // fade by 25% aka 64/256ths
			}
			if (index < LENGTH_LEDS[device]){
					leds[index].setRGB(255, 0, 150);
			}
			FastLED[device].showLeds();
			delay(delay_Time);
			index++;
		}
	}

void CLeds::quietOff(){
	for (int i = 0; i < NUM_LEDS; i++){
		leds[i].setRGB(0,0,0);
	}	
}

//=============================================================================================

void CLeds::transmit(){
		uint8_t error = 10;
		uint8_t address = 12; // goes to the master
		uint8_t package = 1; //just to trigger a timer and a relay
		Wire.beginTransmission(address);
		Serial.println("transmit addy");
		Wire.write(package);
		delay(100);
		Serial.println("transmit package");
		Serial.println(package);
		error = Wire.endTransmission();
		Serial.println(error);
		Serial.println("transmit end");
}
