/*
   Roto
 
 Project:
 Exploration Place - DBF
 
 Exhibit:
 Design A Propellor
 
 Author(s):
 Charles Stevenson
 Brady Schoeffler
 
 
 Original environment:
 Arduino 1.8.2
 
 Revision notes:
 
 
 This script is for DBF exhibit.
 Specifically the piece that when a button is pressed
 a propellor rotates and pushes air across four sensors.
 These sensors read data and cause a series of paralell light strips
 to light up depending on the force of the air on the sensors. More
 lights light up if there is more "thrust" across the sensors.
 
 *     **NOTE**
 I'm not setting the minimum and maximum dynamically. they are static
 variables. the minimum isn't really te minimum but rather the number
 that the sensors average to when the propellor is idle. If this script
 is taken to another location and changes behavior check these values
 with the test_output() function.
 
 Overview ::
 1) read in the sensor data.
 2) analyze the data, calculated average and standard deviation.
 the standard deviation acts as a quality assurance check
 on how good the reads are for the propellor. Therefore,
 if it is under a certain amount proceed with...
 3) Incrementer or Decrement ledIterator depending on data.
 4) Light up the leds
 5) reset certain values && repeat.
 
 */

#define versionString "Roto - [Exploration Place] [Design a Propellor]"


#include <ADS1x15.h>
#include "FastLED.h"
#include "alarmClock.h"
#include <EEPROM.h>
//ALL SORTS OF FUNKY GLOBALS!! It's like a receipt at the grocery store...if only OOP!

//String Macros
#define LED_TYPE    WS2811
#define COLOR_ORDER BRG

#define OFF CRGB(0,0,0)
#define ON(i) CRGB(MAX_COLOR_VAL - (MAX_COLOR_VAL/NUM_LEDS * i), (MAX_COLOR_VAL/NUM_LEDS * i) , 0)
#define milliToSeconds(x) (x / 1000)

#define good_read stdDev<error
#define button_ON 0
#define button_OFF 1

//pins
int const DATA_PIN = 8;
int const BUTTON_PIN = 13;
int const MOTOR_PIN = 6;
int const GEAR_PIN = 14;

//iterators
byte i;
byte j;

//settings
byte const BRIGHTNESS = 255;
byte const FRAMES_PER_SECOND = 120;
unsigned int const BAUD_RATE = 9600;
byte const NUM_SENSORS = 28;//number of sensor reads; not number of sensors
byte const MAX_COLOR_VAL = 255;
float const BUFFER = .75;//buffer set to throw out noisy sensor reads.

//prototypes
void gearCounter();
void resetGearCounter();
void updateLED();
uint32_t calculateLoad();

//instantiate objects
ADS1115 adc;
//LED SETTINGS!!!!!
uint8_t const LED_GROUP_SIZE = 4;
byte const NUM_LEDS = 16;
CRGB leds[NUM_LEDS * LED_GROUP_SIZE];

//global initials
int address = 0;
uint32_t airSum = 0;
uint32_t airSums[NUM_SENSORS];
uint8_t const AVG_BUFFER_SIZE = 5;
uint32_t avg_buffer[AVG_BUFFER_SIZE] = {0,0,0,0,0};
uint8_t epochCounter = 0;
uint32_t avg = 0;
uint32_t pAvg = 0;
uint32_t stdDev = 0;
uint32_t ledItr = 0; //ledItr is an increment / decrement so the led bar on the prop increments and decrements by one.
uint32_t analyzedAvg = 0;
uint32_t baseLoad = 0;
uint32_t gearCount = 0;
uint32_t gearCountInterval = 100;
uint32_t updateLEDInterval = 200;
uint32_t absolute_maximum = 10000;//CHANGE ME TO CHANGE MAXIMUM!!
boolean buttonState = false;
boolean resetAbs = false;


//Test Variables
uint32_t maximum = .9 * absolute_maximum;
uint32_t minimum = 6500;
float error_rate = .5;
uint32_t error = error_rate * maximum;

alarmClock alarm = alarmClock(resetGearCounter);
alarmClock updateLEDAlarm = alarmClock(updateLED);
uint32_t const resetTimer = 1000;


/*=========================================================*/
/* ---- 		CALIBRATION AND SETUP 			---*/
/*=========================================================*/
/*

 Fxn :: start_adc
 Configures: ADC
 
 */
void start_adc() {
  while (!Serial);
  Serial.println("Starting...");
  adc.attachTimeoutHandler([]() {
    Serial.println("Timeout");
  }
  );
  adc.attachErrorHandler([](uint8_t n) {
    Serial.print("Error: ");
    Serial.println(n);
  }
);
  delay(100);
  adc.begin();
  //adc.setCalibration(adc.resistorDivider(13.3, 3.4));
  adc.setGain(GAIN_8);
  Serial.println(adc.getFullScaleV(0), 4);
  Wire.setClock(400000);
  Serial.println("adc setup");
}


uint32_t calibrateGearSensor(){
		digitalWrite(MOTOR_PIN,LOW);
		uint32_t const INTERVAL = 1000; //calibration time;
		uint32_t initTime = millis();
		uint32_t gearCount = 0;
		while (millis() - initTime < INTERVAL){
				if (digitalRead(GEAR_PIN) == HIGH){
						gearCount++;
				}
		}
		digitalWrite(MOTOR_PIN,HIGH);
		return gearCount / milliToSeconds(INTERVAL);
}

void lightsFlash(){
		byte z = 0;
		//messy last minute edit to get the leds to flash to indicate a reset;
		for (z = 0; z < 10; z++){
		  for (i = 0; i < NUM_LEDS; i++) { //turn all lights below ledItr ON
			for (j = 0; j < LED_GROUP_SIZE; j++){
			  leds[(i * LED_GROUP_SIZE) + j] = ON(i); 
			}
		  }
		  FastLED.show();
		  ledItr = 0;
		  delay(500);
		  for (i = 0; i < NUM_LEDS; i++) { //turn all lights above ledItr OFF
			for (j = 0; j < LED_GROUP_SIZE; j++){
			  leds[(i * LED_GROUP_SIZE) + j] = OFF;
			}
		  }
		  FastLED.show();
		  delay(500);
		}
}

void resetAbsoluteMaximum(){
		absolute_maximum = 10000;
		digitalWrite(MOTOR_PIN, HIGH);
		lightsFlash();
		Serial.println("RESETTING ABSOLUTES!!");
}

/*
   Fxn Setup
 configures :: Serial I/o
 Sensor Array
 FastLED
 adc
 */
void setup()
{
  //Open Serial && write version stuff
  Serial.begin(BAUD_RATE);
  Serial.println(versionString);
  Serial.print(F("File: "));
  Serial.println(__FILE__);

  //set pins I/O
  pinMode(BUTTON_PIN, INPUT);
  pinMode(GEAR_PIN, INPUT);
  pinMode(MOTOR_PIN,OUTPUT);
  pinMode(DATA_PIN,OUTPUT);

  //init LED library
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS * LED_GROUP_SIZE).setCorrection(TypicalLEDStrip); // tell FastLED about the LED strip configuration
  FastLED.setBrightness(BRIGHTNESS); // set master brightness control

  //init analog to digital converter.
  start_adc();
  //init interrupt
  attachInterrupt(digitalPinToInterrupt(2),gearCounter,RISING); 
  //baseLoad = calibrateGearSensor();
  
  //EEPROM stuff
  /*
  if (digitalRead(BUTTON_PIN) == LOW){
		resetAbsoluteMaximum();
  }
  else{
		//EEPROM.get(address, absolute_maximum);//EEPROM!!
		absolute_maximum = 11000;
  }*/
  absolute_maximum = 11000;
  maximum = absolute_maximum * .9;
}


/*=========================================================*/
/*--- 				HELPER FUNCTIONS 			---*/
/*=========================================================*/
/*
		FxN :: resetEpoch
				resets values recalculated every epoch or cycle
				that is, read sensors, light leds, reset values.
		
*/
void resetEpoch(){
		airSum = 0;
		avg = 0;
}

void alarmProtocol(){
  alarm.poll();
  updateLEDAlarm.poll();
  // set alarms
  if (alarm.isSet() == false){
    alarm.setAlarm(resetTimer);
  }
  if(updateLEDAlarm.isSet() == false){
    updateLEDAlarm.setAlarm(updateLEDInterval);   
  }
}
/*
		FxN :: gearCounter
				interrupt gear counter;
*/
void gearCounter(){
		gearCount++;
}

/*
		FxN :: resetGearCounter
				alarmClock reset and reporter
*/
void resetGearCounter(){
		reportData();
		gearCount = 0;
}

/*
		FxN :: reportData
				writes data to the Serial Comm.

*/
void reportData(){//DO NOT NEED THIS FOR FINAL!!
  Serial.print("Average ");//Floating value (will change depending on airflow)
  Serial.print(pAvg/100);
  Serial.println();
  delay(50);
  Serial.print("Maximum ");//may not change if absolute max doesn't change
  Serial.print(maximum/100);
  Serial.println();
  delay(50);
  Serial.print("Absolute Maximum "); //SHOULDN'T CHANGE
  Serial.print(absolute_maximum/100);
  Serial.println();
}

/*
		Fxn :: readSensors
				reads air flow sensors
 
 */

void readSensors() {
  for (i = 0; i < NUM_SENSORS; i++) {
    airSums[i] = adc.analogRead(i % 4); //read new values
  }
}

/*
		FxN :: bufferAverage
				Calculates the average of averages!
				(yet another average function (sorry!)) =^.^=
*/
uint32_t bufferAvg(){
		uint32_t tempAvg = 0;
		for (i = 0; i < AVG_BUFFER_SIZE; i++){
				tempAvg += avg_buffer[i];
		}
		return tempAvg / AVG_BUFFER_SIZE;
}



/*=========================================================*/
/*--- 			ANALYSIS (MEAN && STD) FUNCTIONS		---*/
/*=========================================================*/
/*

		FxN :: calculateAvg
				calculates a mean between N air sensor reads.

*/
uint32_t calculateAvg(){
  uint32_t tempAvg = 0;
  byte counter = 0;
  for (i = 0; i < NUM_SENSORS; i++) {
    if ((airSums[i] < (maximum + (BUFFER * maximum))) && ((airSums[i]) > (minimum - (BUFFER * maximum) / 3))) {//Discards noisy values.
      airSum += airSums[i];
      counter++;
    }
  }
  tempAvg = airSum / counter;
  return tempAvg;
}


/*

		FxN :: calculateStd
				calculates a standard deviation between N air sensor reads.
				to determine whether the sensor reads were too noisy.

*/
uint32_t calculateStd(){
  uint32_t tempStd = 0;
  byte counter = 0;
  for (i = 0; i < NUM_SENSORS; i++) {
    if ((airSums[i] < (maximum + (BUFFER * maximum))) && ((airSums[i]) > (minimum - (BUFFER * maximum) / 3))) {
      tempStd += sq(airSums[i] - avg);
      counter++;
    }
  }
  tempStd = tempStd / (counter - 1);
  tempStd = sqrt(tempStd);
  return tempStd;
}

/*

		FxN :: calculateLoad (DEPRECATED NOT USED!!)
				calculates deltaLoad()
				or how much drag the propellors are creating
				read from the gear tooth sensor

*/
uint32_t calculateLoad(){//TODO question :: what does the gear tooth sensor tell me about the exhibit??
		float deltaLoad = baseLoad - (gearCount / milliToSeconds(gearCountInterval));
		gearCount = 0;
		return deltaLoad;
}

/*

 Fxn :: analyzeSensorData
 Calculates Average and Standard Deviation
 
*/
void analyzeSensorData() {
  avg = calculateAvg();
  pAvg = avg;
  stdDev = calculateStd();
}

/*
 *  FxN :: updateAverage()
 *    appends an average every epoch to an array / buffer.
 *    
 */
void updateAverage(){
    avg_buffer[epochCounter] = avg;//append epochs average
	epochCounter = (epochCounter + 1) % AVG_BUFFER_SIZE; //increment to a new epoch
}

/*=========================================================*/
/*---		LED UPDATE AND SHOW FUNCTIONS			---*/
/*=========================================================*/


void updateAbsoluteMax(uint32_t avg){
/*
	if (avg > absolute_maximum){
				absolute_maximum = avg;
				//EEPROM.put(address, absolute_maximum);//DELETE COMMENT TO ACTIVATE EEPROM
				maximum = .9 * absolute_maximum;
	}*/

}


/*
 Fxn :;  updateLED
		updates LED iterator variable
		ledItr is just a counter that steps up and steps down so all the lights
		don't light up at once but rather work up to a maximum or minimum value.
*/
void updateLED() {
  if (buttonState == button_ON) {
    avg = bufferAvg();//calculate average of averages
	updateAbsoluteMax(avg);
    avg = constrain((map(avg, minimum, maximum, 0, NUM_LEDS)), 0, NUM_LEDS);//constrain avg scale {min, max} to num of LEDS {0,n};
    if (avg < ledItr || (ledItr == 1 && avg == 1)) {
      if (ledItr != 0) {
        ledItr--;
      }
    }
    else if (avg > ledItr) { //increments ledItr
      if (ledItr <= NUM_LEDS) {
        ledItr = (ledItr + 1);
      }
    }
  }
  else {
    if (avg < ledItr || (ledItr == 1 && avg == 1)) {
      if (ledItr != 0) {
        buttonState = digitalRead(BUTTON_PIN);
        if (buttonState == button_OFF) {
          ledItr--;
        }
      }
    }
  }
}

/*
 Fxn :: lightLED
 lights up leds based on updates ledItr
*/
void lightLED() {
  for (i = 0; i < ledItr; i++) { //turn all lights below ledItr ON
    for (j = 0; j < LED_GROUP_SIZE; j++){
      leds[(i * LED_GROUP_SIZE) + j] = ON(i); 
    }
  }
  for (i = ledItr; i < NUM_LEDS; i++) { //turn all lights above ledItr OFF
    for (j = 0; j < LED_GROUP_SIZE; j++){
      leds[(i * LED_GROUP_SIZE) + j] = OFF;
    }
  }
  if (ledItr == 0 && buttonState == button_ON ) {//turn on one light when button is pressed.
    for (j = 0; j < LED_GROUP_SIZE; j++){
      leds[j] = ON(0);
    }
  }
  FastLED.show();
}


/*=========================================================*/
/*				 LOOOOOOOOOOOOOOOOOOOOOP				   */
/*=========================================================*/
/*

 Fxn :: Loop
 Reads Sensor data
 Analyzes Sensor Data
 Updates the leds based on that data

*/
void loop() {
  buttonState = digitalRead(BUTTON_PIN);
  alarmProtocol();
  if (buttonState == button_ON) {
    digitalWrite(MOTOR_PIN, LOW);
    readSensors();
    analyzeSensorData();
    if (good_read){
      updateAverage();
    }
  }
  if (buttonState == button_OFF) {
    digitalWrite(MOTOR_PIN, HIGH);
  }
  lightLED();
  resetEpoch();
}
