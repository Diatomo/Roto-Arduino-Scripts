//DESIGNING A PROPELLOR
//originally written by Brady
//edited by Charles Stevenson, August 30th 2017
//Owned by Roto, DBF exhibit.
/*
*
*	This script is for DBF exhibit.
*			Specifically the piece that when a button is pressed
*			a propellor rotates and pushes air across four sensors.
*			These sensors read data and cause a series of light strips
*			to light up depending on the force of the air on the sensors.
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

#include "sensors.h"
#include "lights.h"

#define index sense.NUM_SENSORS * sense.DENOISE_COUNTER


//pins
int const DATA_PIN = 8;
int const BUTTON_PIN = 2;

//iterators
byte i;

//settings
unsigned int const BAUD_RATE = 9600;

//instantiate objects
Lights ledController;
Sensors sense;

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
  sense.set_adc();
  ledController.set_FastLED();
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
  
  sense.read();//reads sensor data returns state;
  sense.analyzeSensorData();//analyze average, stdDev, && update max;
  ledController.updateLED(sense);
  ledController.lightLED(sense, sense.buttonState);
  delay(20);
}
