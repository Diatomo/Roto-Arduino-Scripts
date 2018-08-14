#include "FastLED.h"
#include "alarmClock.h"

#define versionString "Roto - [Discovery World Energy] [Cycle to Power]"
#define COLOR_ORDER RGB


uint32_t ledCounter = 0;

//PINS
uint8_t const DATA_PIN_OUTER = 9;
uint8_t const DATA_PIN_MID = 10;
uint8_t const DATA_PIN_INNER = 8;
uint8_t const BUTTON_PIN = 19;
uint32_t const chanA = 2;
uint32_t const chanB = 3;

// LED PARAMETERS
uint8_t const QUADRANT = 4;
uint32_t const NUM_LEDS_OUTER = 25 * QUADRANT;
uint32_t const NUM_LEDS_MID = 3;
uint32_t const NUM_LEDS_INNER = NUM_LEDS_OUTER / 2;

// COLOR PARAMETERS
uint32_t const MAX_COLORVAL = 360;
uint32_t const MIN_COLORVAL = 0;
uint32_t const satVal = 255;
uint32_t const brightVal = 255;
uint32_t const SENSITIVITY = 20;
uint32_t colorVal = 0; //HSV value {0,100}
uint32_t colorCounter = 0;

//Global Obj. Definitions.
CRGB leds_outer[NUM_LEDS_OUTER];
CRGB leds_mid[NUM_LEDS_MID];
CRGB leds_inner[NUM_LEDS_INNER];

//prototypes
void lightRoutine();

//alarm Clocks
alarmClock ledClk = alarmClock(lightRoutine);

//timers
uint32_t ledTimer = 40;


//switches
boolean midFade = true;
boolean buttonPressed = false;

void setup() {
		Serial.begin(9600);
		pinMode(BUTTON_PIN, INPUT);
		FastLED.addLeds<WS2811, DATA_PIN_OUTER, COLOR_ORDER>(leds_outer, NUM_LEDS_OUTER);
		FastLED.addLeds<WS2811, DATA_PIN_MID, COLOR_ORDER>(leds_mid, NUM_LEDS_MID);
		FastLED.addLeds<WS2811, DATA_PIN_INNER,COLOR_ORDER>(leds_inner, NUM_LEDS_INNER);
		attachInterrupt(digitalPinToInterrupt(2),changeColor,CHANGE); 
}

void changeColor(){
		if (digitalRead(chanA) != digitalRead(chanB)){
				if (colorVal < MAX_COLORVAL){
						colorVal++;
				}
		}
		else{
				if (colorVal > MIN_COLORVAL){
						colorVal--;
				}
		}
}

void lightMiddle(int i){
		int counter = 1;//in case middle needs to split up into sections;
		if (i == 7){//this will be some sectioned unit (sorry for shitty math);
				for (byte j = 0; j < NUM_LEDS_MID / counter; j++){
						leds_mid[j].setHSV(colorVal,satVal,brightVal); 
				}
		}
}

void fade(){
		fadeToBlackBy(leds_inner, NUM_LEDS_INNER, 50);
		fadeToBlackBy(leds_outer, NUM_LEDS_OUTER, 50);
		if (midFade){
				fadeToBlackBy(leds_mid, NUM_LEDS_MID, 30);
		}
}

void setColor(){
		leds_outer[ledCounter].setHSV(colorVal,satVal,brightVal);
		leds_inner[ledCounter/2].setHSV(colorVal,satVal,brightVal);
		lightMiddle(ledCounter);
}

void lightRoutine(){
		setColor();
		fade();
		ledCounter = (ledCounter + 1) % NUM_LEDS_OUTER;
		FastLED.show();
}

void readButt(){ //lel =^.^=
		if (digitalRead(BUTTON_PIN) == LOW && !buttonPressed){
				buttonPressed = true;
				midFade = !midFade;
		}
		else if (digitalRead(BUTTON_PIN) == HIGH && buttonPressed){
				buttonPressed = false;
		}
		if (midFade == false){
				lightMiddle(7);
		}
}

void alarmClockRoutine(){
		if (ledClk.isSet()){
				ledClk.poll();
		}
		else{
				ledClk.setAlarm(ledTimer);
		}
}

void loop(){
		readButt();
		alarmClockRoutine();
}


