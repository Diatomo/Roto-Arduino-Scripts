//DESIGNING A PROPELLOR
//Written by: Charles Stevenson && Brady, 2017
//Owned by Roto, DBF exhibit.
/*
*
*	This script is for DBF exhibit.
*			Specifically the piece that when a button is pressed
*			a propellor rotates and pushes air across four sensors.
*			These sensors read data and cause a series of paralell light strips
*			to light up depending on the force of the air on the sensors. More
*   lights light up if there is more "thrust" across the sensors.
*   
*			**NOTE**
*								I'm not setting the minimum and maximum dynamically. they are static
*								variables. the minimum isn't really te minimum but rather the number 
*								that the sensors average to when the propellor is idle. If this script
*								is taken to another location and changes behavior check these values
*								with the test_output() function.
*
*	Overview ::
*		1) read in the sensor data.
*		2) analyze the data, calculated average and standard deviation.
*			the standard deviation acts as a quality assurance check
*			on how good the reads are for the propellor. Therefore,
*			if it is under a certain amount proceed with...
*		3) Incrementer or Decrement ledIterator depending on data.
*		4) Light up the leds
*		5) reset certain values && repeat.
*
*/

#include <ADS1x15.h>
#include "FastLED.h"

//String Macros
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define OFF CRGB(0,0,0)
#define ON(i) CRGB(MAX_COLOR_VAL - i*(i), i*i, 0)
#define good_read stdDev<error
#define button_ON 0
#define button_OFF 1

//pins
int const DATA_PIN = 8;
int const BUTTON_PIN = 2;

//iterators
byte i;

//settings
byte const BRIGHTNESS = 96;
byte const FRAMES_PER_SECOND = 120;
unsigned int const BAUD_RATE = 9600;
byte const NUM_LEDS = 16;
byte const NUM_SENSORS = 50;//number of sensor reads; not number of sensors
byte const MAX_COLOR_VAL = 255;
float const BUFFER = .8;//buffer set to throw out noisy sensor reads.

//instantiate objects
ADS1115 adc;
CRGB leds[NUM_LEDS];

//global initials
uint32_t airSum = 0;
uint32_t airSums[NUM_SENSORS];
uint32_t avg = 0;
uint32_t stdDev = 0;
uint32_t ledItr = 0;
uint32_t analyzedAvg = 0;
boolean buttonState = false;

uint32_t maximum = 8900;
uint32_t minimum = 6600;
uint32_t error = .3 * maximum;

//Test Variables
uint32_t lastMax = maximum;
uint32_t lastMin = minimum;


/*
 * 
 * Fxn :: start_adc
 *    Configures: ADC
 * 
 */
void start_adc(){
  while (!Serial);
  Serial.println("Starting...");
  adc.attachTimeoutHandler([]() {
    Serial.println("Timeout");
  });
  adc.attachErrorHandler([](uint8_t n) {
    Serial.print("Error: ");
    Serial.println(n);
  });
  delay(100);
  adc.begin();
  adc.setCalibration(adc.resistorDivider(13.3, 3.4));
  adc.setGain(GAIN_8);
  Serial.println(adc.getFullScaleV(0), 4);
  Wire.setClock(400000);  
}

/*
 * Fxn Setup
 *  configures :: Serial I/o
 *               Sensor Array
 *               FastLED
 *               adc
 */
void setup()
{
  Serial.begin(BAUD_RATE);
  pinMode(BUTTON_PIN, INPUT);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // tell FastLED about the LED strip configuration
  FastLED.setBrightness(BRIGHTNESS); // set master brightness control
  start_adc();
}

/*
 * 
 * Fxn :: readSensors
 *  reads sensors and analyzes sensor data
 *
 */   

void readSensors(){
  buttonState = digitalRead(BUTTON_PIN);
  for (i=0; i<NUM_SENSORS; i++){
    airSums[i] = adc.analogRead(i%4);//read new values
  }
}

/*
 * 
 * Fxn :: analyzeSensorData
 *  Analyzes averages, min, and max
 *
 * 
 */
void analyzeSensorData(){
		int counter = 0;
  //average
  for (i=0;i<NUM_SENSORS;i++){
		if ((airSums[i] < (maximum + (BUFFER*maximum))) && ((airSums[i]) > (minimum - (BUFFER*maximum)/3))){
				airSum += airSums[i];
				counter++;
		}
  }
  avg = airSum / counter;

  //stdDev
  counter = 0;
  for (i=0; i<NUM_SENSORS;i++){
		if ((airSums[i] < (maximum + (BUFFER*maximum))) && ((airSums[i]) > (minimum - (BUFFER*maximum)/3))){
				stdDev += sq(airSums[i] - avg);
				counter++;
		}
  }
  stdDev = stdDev / (counter-1);
  stdDev = sqrt(stdDev);
 }

/*
 * 
 * Fxn updateLED
 *  updates LED iterator variable
 * 
 */
void updateLED(){
  if(buttonState == button_ON){
    analyzedAvg = avg;
    avg = constrain((map(avg, minimum, maximum, 0, NUM_LEDS)),0,NUM_LEDS);
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
  else{
    if (avg < ledItr || (ledItr == 1 && avg == 1)){
      if (ledItr != 0){
						buttonState = digitalRead(BUTTON_PIN);
						  if (buttonState == button_OFF){
          ledItr--;
        }
      delay(200);
      }
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
void lightLED(){
  for (i=0; i < ledItr; i++) {	
        leds[i] = ON(i);//turn on leds
	}
	for (i=ledItr;i<NUM_LEDS;i++){
		leds[i] = OFF;//turn off leds
  }
    FastLED.show();
    //reset values
    airSum = 0;
    avg = 0;
}


/*
*
* Fxn :: testOutput
*		outputs global variables that may need to be observed
*
*
*/
void testOutput(){
		Serial.print("\n\n\nANALYZED AVERAGE :: ");
		Serial.print(analyzedAvg);
		Serial.print("\nSTANDARD DEVIATION :: ");
		Serial.print(stdDev);
		Serial.print("\n");
		Serial.print("\nMAXIMUM :: ");
		Serial.print(maximum);
		Serial.print("\nMINIMUM :: ");
		Serial.print(minimum);
		Serial.print("\nAverage_Constrained :: ");
		Serial.print(avg);
		Serial.print("\nledItr :: ");
		Serial.print(ledItr);
		Serial.print("\n");
		Serial.print("\n");
		for (i=0; i<NUM_SENSORS;i++){
				if ((airSums[i] < (maximum + (BUFFER*maximum))) && ((airSums[i]) > (minimum - (BUFFER*maximum)))){
        Serial.print("Sensor");
        Serial.print(i);
        Serial.print(" ");
			Serial.print(airSums[i]);
				Serial.print("\n");
				}
		}
}

/*
 * 
 * Fxn :: Loop
 *  Reads Sensor data
 *  Analyzes Sensor Data
 *  Updates the leds based on that data
 *  
 *  
 */

void loop(){
  buttonState = digitalRead(BUTTON_PIN); 
  if (buttonState == button_ON){
    readSensors();
    analyzeSensorData();
		  if (good_read){
				  updateLED();
		  }
  }
  if (buttonState == button_OFF){
      updateLED();
  }
  lightLED();
}
