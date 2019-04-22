
/*
   Roto
 
 Project: HIGH Museum
 Exhibit: Noodle Forest
 Author(s): Charles Stevenson
 Date: 7/23/18
 
 Original environment:
 Arduino 1.8.2
 
 Revision notes:

 
 */

#include <Wire.h>
#include <LIDARLite.h>
#include <alarmClock.h>
#include <outputExtend.h>

//Sounds
uint8_t const NUM_HERTZ = 8;

//LIDARS
uint8_t const NUM_SENSORS = 4;

//arr
LIDARLite sensors[NUM_SENSORS];
int maxDistances[NUM_SENSORS];

//test sensor TODO delete this later***
LIDARLite testSens;
int testDist;


//Control Structures
//====================================================
//control structures (lockouts or different sounds);
int8_t currSound = 8;

//Lockout may not be needed =)
uint32_t pTime = 0;
uint32_t const TIMR = 3000;

//stats (mean)
uint32_t MAXIMUM;
uint32_t counter;
uint32_t sum;
uint8_t const N = 5;


//Boards
//====================================================
//pin configurations...see schematic for details
uint32_t const oeData = 6;
uint32_t const oeClock = 5;
uint32_t const oeLatch = 4;
uint8_t const NUM_OE = 1;

//boards
outputExtend oe = outputExtend(oeData, oeClock, oeLatch, NUM_OE); 

/*
*
* FxN :: setupOE
*	setup outputExtend's configuration
*
*/
void setupOE(){
    //oe Extended.
    for (byte i = 0; i < NUM_HERTZ; i++){
        oe.extendedWrite(i,HIGH);
        delay(200);
        oe.extendedWrite(i,LOW);
    }
}

/*
*
* FxN :: setupLIDAR
*	setup LIDAR configuration
*	setup max distance array
*
*/
void setupLIDAR(){
	testSens.begin(1,true);
	testSens.configure(0);
	testDist = testSens.distance();
}

/*
*
*	FxN :: getMax
*		setups Maximum.
*
*/
uint32_t getMax(){
	uint8_t counter = 0;
	uint32_t acc = 0;
	while (counter < 150){
		acc += testSens.distance();
		counter++;
	}
	return acc / counter;
}

/*
*
* FxN :: setup
*
*/
void setup(){  
    //Open Serial && write version
	uint32_t const BAUD_RATE = 9600;
    Serial.begin(BAUD_RATE);
	pTime = millis();
	//setupOE(HIGH); setupOE(LOW);
	setupOE();
	setupLIDAR();
	MAXIMUM = getMax();
}


/*
*
*	FxN :: measure
*		just measures the sensors;;
*
*/
int measure(LIDARLite sensor){
	return sensor.distance();
}

/*
*
*	FxN :: calcSound
*		Calculates which sound to play. 
*
*/
uint8_t calcSound(LIDARLite sensor, int measurement){
	int static const OFFSET = 5; 
	byte sound = -1;
	bool isSet = false;
	uint32_t maxDist = MAXIMUM - OFFSET;
	if (measurement <= maxDist){
		int unit = maxDist / NUM_HERTZ;
		for (int x = 0; x <= NUM_HERTZ; x++){
			if (measurement < (unit * x) + OFFSET && !isSet){
				sound = x;
				isSet = true;
			}
		}
	}
	return sound;
}

/*
*
*	FxN :: play
*		plays a sound
*/
void play(uint8_t sound){
	if (sound > -1 && sound != pSound){
		oe.extendedWrite(sound, HIGH);
		delay(50);
		oe.extendedWrite(sound, LOW);
		delay(50);
	}
	pTime = millis();
}

/*
*
*	FxN :: average
*		Takes the average of the liDar
*
*/
void average(LIDARLite sensor){
	sum += testSens.distance();
	counter++;
}


/*
*
* FxN :: loop
*
*/
void loop(){
	uint32_t currTime = millis();
	average(testSens);
	if (counter > N){
		play(calcSound(testSens, (sum / counter)));
		counter = 0;
		sum = 0;
	}
}
