
/*
   Roto
 
 Project: High Museum
 Exhibit: Color Cubby
 Author(s): Charles Stevenson, Timothy Smith
 Date: 8/8/18
 
 Original environment:
 Arduino 1.8.2
 
 Revision notes:
 
 	I threw this together in about 15minutes. 
 	NOT TESTED!! Will Test on Final Exhibit.
 
 */
#define NOT_AN_INTERRUPT -1


//Libraries
//#include <Wire.h>
#include <pwmBoard.h>
#include <Wire.h>
#include <FastLED.h>

//Definitions
#define chanA 2
#define chanB 3

//RGB and HSV variables
int red = 0;
int green = 0;
int blue = 0;

uint8_t sat = 255; //saturation
uint8_t val = 255; //brightness

//PWM BOARD Config
byte const addy = 15;
pwmBoard pwm = pwmBoard(addy);

//Wheel sensitivity and max color count
uint32_t const SENSITIVITY = 32; //SENSITIVITY - 32 is a perfect 360

//Current color count and sensitivity counter
byte currColor = 0;
int sensCounter = SENSITIVITY/2;

CHSV hsv(255,255,255);
CRGB rgb(0,0,0);


/* ===============================================*/
/* ===========   TEST FUNCTIONS   ================*/
/* ===============================================*/
/*
*	FxN :: setupPWM
*		tests pwm colors in advance
*
*/
void setupPWM(){
	uint8_t MAX_COLOR = 255;
	uint8_t numChannels = 3;
	for (uint8_t i = 0; i < numChannels; i++){
		pwm.setLevel(i, MAX_COLOR);
		pwm.send();
		delay(250);
		if (i != 0){
			pwm.setLevel(i, 0);
		}
	}
	hsv2rgb_rainbow(hsv, rgb);
	pwm.setLevel(0,rgb.r);
	pwm.setLevel(1,rgb.g);
	pwm.setLevel(2,rgb.b);
  	pwm.send();
}


/*
*
 * FxN :: setup
 *
 *
 */
void setup(){

	//Open Serial
	while (!Serial);
	Serial.begin(9600);

	//pins
	pinMode(chanA, INPUT);
	pinMode(chanB, INPUT);

	//interrupt
	attachInterrupt(digitalPinToInterrupt(2),changeColor,CHANGE); 

	//setupBoards
	pwm.start();
	setupPWM();
}



/*
*
 *
 * FxN :: changeColor
 *   Interrupt Function, that counts up and down the encoder values.
 *
 */
void changeColor(){
  if (digitalRead(chanA) != digitalRead(chanB)){
    if (sensCounter < SENSITIVITY){
      sensCounter++;
    }
  }
  else{
    if (sensCounter > 0){
      sensCounter--;
    }
  }
}


/*
*
 *
 * FxN :: analyzeColor
 *   Analyzes when a change in ecoder values cascades to a recorded change in value
 *
 */
boolean analyzeColor(){
  boolean change = false;
  if (sensCounter >= SENSITIVITY){
      currColor++;
      sensCounter = SENSITIVITY/2;
      change = true;
  }
  else if (sensCounter <= 0){
    if (currColor > 0){
      currColor--;}
    if (currColor == 0){
      currColor = 255;
    }
      sensCounter = SENSITIVITY/2;
      change = true;
    
  }
  return change;
}


/*
*
 *
 * FxN :: updateColor
 *   updates color when a change occurs
 *
 */
void updateColor(){
	hsv.h = currColor;
	hsv2rgb_rainbow(hsv, rgb);
	pwm.setLevel(0,rgb.r);
	pwm.setLevel(1,rgb.g);
	pwm.setLevel(2,rgb.b);
	pwm.send();
}


/*
*
 * FxN :: loop
 *   loops by analyzing a change then updated to match to corresponding change;
 *
 */
void loop(){
  if (analyzeColor()) { 
    updateColor(); 
  }
}


