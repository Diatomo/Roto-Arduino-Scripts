
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

#define debug false

#include <Wire.h>
#include <LIDARLite.h>
#include <alarmClock.h>
#include <SoftwareSerial.h>

//Sounds
uint8_t const NUM_HERTZ = 8;
uint8_t const soundsPerBeam = 3;
uint8_t soundPattern[15] = {3,2,1, 5,4,3, 7,6,5, 1,8,7, 3,2,1};
bool played = false;

//LIDARS
uint8_t const nLIDARS = 5;
uint8_t const iicHubCap = 4;
uint8_t const tx = 2;
uint8_t const rx = 3;

uint32_t const maximum_distance = 255;

//distance measuring sensor
LIDARLite sensors;

//Control Structures
//stats (mean)
uint32_t MAXIMUM;
uint32_t counter;
uint32_t sum;
uint8_t const N = 5;

//struct
//Data basket
struct lidarData{
	uint32_t value;
	uint32_t maximum;
	int8_t currSound;
	int8_t prevSound;
	int8_t playableSounds[soundsPerBeam];
	bool brokenBeam;
	bool active;
};

lidarData sensorA;
lidarData sensorB;
lidarData sensorC;
lidarData sensorD;
lidarData sensorE;
lidarData lidars[nLIDARS] = {sensorA, sensorB, sensorC, sensorD, sensorE};

SoftwareSerial MIDISerial(rx,tx);

void exhibit();
alarmClock clk = alarmClock(exhibit);
uint32_t timr = 10; 


void setupLIDAR(){
	uint8_t counter = 0;
	for (uint8_t lidar = 0; lidar < nLIDARS; lidar++){
		lidars[lidar].value = 0;
		lidars[lidar].maximum = getMax(lidar);
		for(uint8_t i = 0; i < soundsPerBeam; i++){
			lidars[lidar].playableSounds[i] = soundPattern[counter];
			counter++;
		}
		lidars[lidar].brokenBeam = false;
	}
}

/*
*
*	FxN :: getMax
*		setups Maximum.
*
*/
uint32_t getMax(uint8_t sensor){

/*
	//initials
	uint8_t counter = 0;
	uint32_t acc = 0;
	uint8_t mode = 99;

	//setupHub
	setupHub(sensor);
	while (counter < mode){
		acc += sensors.distance(false);
		counter++;
	}
	*/
	return maximum_distance;
}

/*
*
* FxN :: setup
*
*/
void setup(){  
    //Open Serial && write version
	uint32_t const BAUD_RATE = 9600;
	uint32_t const MIDI_BAUD = 31250;
    Serial.begin(BAUD_RATE);
	MIDISerial.begin(MIDI_BAUD);


	pinMode(14,OUTPUT);
	pinMode(15,OUTPUT);
	digitalWrite(14,LOW);
	digitalWrite(15,LOW);
	delay(50);
	digitalWrite(14,HIGH);
	digitalWrite(15,HIGH);
	Wire.begin();
	setupLIDAR();
	testSound();
}

void setupHub(uint8_t sensor){
		
		if (sensor < iicHubCap){
			Wire.beginTransmission(0x70);
			Wire.write(1 << sensor);
			delay(1);
			Wire.endTransmission();
			Wire.beginTransmission(0x71);
			Wire.write(0b00000000);
			delay(1);
			Wire.endTransmission();
		}
		else{
			Wire.beginTransmission(0x70);
			Wire.write(0b00000000);
			delay(1);
			Wire.endTransmission();
			Wire.beginTransmission(0x71);
			Wire.write(0b00000001);
			delay(1);
			Wire.endTransmission();
		}
}


void readSensors(){
	for (uint8_t sensor = 0; sensor < nLIDARS; sensor++){
		setupHub(sensor);
		delay(5);
		lidars[sensor].value = sensors.distance(true);
	}
}


/*
*
*	FxN :: calcSound
*		Calculates which sound to play. 
*
*/
void calcSound(lidarData* sensor, uint8_t lidar){
	int static const FLOOR_OFFSET = 5;
	int static const ROOF_OFFSET = 50;
	uint32_t maxDist = sensor->maximum - FLOOR_OFFSET; 
	if (sensor->value < maxDist && sensor->value > 7){ //7 is there to deflect noise...
		if (sensor->brokenBeam == false){
			sensor->brokenBeam = true;
			sensor->active = true;
		}
		if (sensor->value < 135){
			sensor->currSound = sensor->playableSounds[0];
		}
		else if (sensor->value > 135 && sensor->value < 185){
			sensor->currSound = sensor->playableSounds[1];
		}
		else if (sensor->value > 185){
			sensor->currSound = sensor->playableSounds[2];
		}
	}
	else{
		sensor->brokenBeam = false;
	}
}


void testSound(){
	for (uint8_t sound = 0; sound < 8; sound++){
		Serial.print("sound :: ");
		Serial.println(sound+1);
		MIDISerial.write(0x91);
		MIDISerial.write(sound+1);
		MIDISerial.write(127);
		delay(1000);
	}
}

/*
*
*	FxN :: play
*		plays a sound
*/
void play(lidarData* sensor){
	if ((sensor->brokenBeam && sensor->active) || (sensor->currSound != sensor->prevSound)){
		//Serial.println("play");
		MIDISerial.write(0x91);
		MIDISerial.write(sensor->currSound);
		MIDISerial.write(127);
		Serial.print("sound :: ");
		Serial.println(sensor->currSound);
		sensor->prevSound = sensor->currSound;
		sensor->active = false;
		played = true;
	}
}


/*
	uint32_t value;
	uint32_t maximum;
	int8_t currSound;
	int8_t prevSound;
	int8_t playableSounds[soundsPerBeam];
	bool brokenBeam;
*/
void printOutSensorData(lidarData sensor, uint8_t lidar){
	Serial.print("LiDaR ID :: ");
	Serial.println(lidar);
	Serial.print("Lidar Measurement :: ");
	Serial.println(sensor.value);
	Serial.print("Lidar Maximum :: ");
	Serial.println(sensor.maximum);
	Serial.print("Lidar CurrSound :: ");
	Serial.println(sensor.currSound);
	Serial.print("Lidar prevSound :: ");
	Serial.println(sensor.prevSound);
	Serial.print("Lidar brokenBeam :: ");
	Serial.println(sensor.brokenBeam);
	Serial.println();
	delay(1000);
}

void exhibit(){
	readSensors();
	for (uint8_t lidar = 0; lidar < nLIDARS; lidar++){
			calcSound(&lidars[lidar], lidar);
			play(&lidars[lidar]);
		}
}

/*
*
* FxN :: loop
*
*/
void loop(){
	if (clk.isSet()){
		clk.poll();
	}
	else{
		clk.setAlarm(timr);
	}
}
