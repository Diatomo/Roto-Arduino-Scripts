#include <Arduino.h>
#include <Wire.h>
#include "master.h"


uint32_t const timeMultipliers[9] = {10,20,30,40,50,60,70,80,90};
uint32_t const deviceInput[5] = {2, 10, 3, 4, 6};

/*
 *
 *
 *
 */
Master::Master(){
		
		uint8_t encOffset = 4;//pin

		Wire.begin();//init i2c

		//creation of data structure
		for (uint8_t i = 0; i < NUM_BIKES; i++){

				//create cycle
				Cycle cycle;
				cycle.encoder = i + encOffset;
				cycle.acc = 0;
				cycle.energyMeter = 0;
				cycle.energyTimer = 0;

				//add to array
				Cycles[i] = cycle;
		}
}

//Selection Function
void Master::randomDevice(){
		device = uint8_t(random(MAX_DEVICES));
		uint8_t optoCopter = 5;
		if (device == optoCopter){
				if (optoTimer != 0){
						if (millis() - optoTimer < OPTO_TIMER){
								randomDevice();
						}
				}
		}
		else{
				optoCopter = 0;
		}
}

//==========================================================
//				Power-Manipulation Functions
//==========================================================

/*
 *	FxN :: gainPower
 *		@param bike -> &Cycle
 *
 *	+brief :: add power to the energyMeter
 *
 */
void Master::gainPower(Cycle& bike){
		if (bike.acc > accTHRESH && bike.energyMeter < 2000){
			bike.acc = 0;
			bike.energyTimer = millis();
			bike.energyMeter++;
			Serial.println(bike.energyMeter);
		}
}

/*
 *
 *	FxN :: losePower
 *		@param bike ->  Cycle&
 *
 */
void Master::losePower(Cycle& bike){
		uint32_t currentTime = millis() - bike.energyTimer;
		if (currentTime > losePowerTHRESH && bike.energyMeter > 0){
				bike.energyTimer = millis();
				bike.energyMeter--;
		}
}

/*
 *
 *
 *
 *
 */
void Master::adjustPower(){
	for (uint8_t i = 0; i < NUM_BIKES; i++){
			uint32_t currAcc = digitalRead(Cycles[i].encoder);
			int8_t difference = abs(currAcc - Cycles[i].prevAcc);
			Cycles[i].acc += abs(difference);
			Cycles[i].prevAcc = currAcc;
			gainPower(Cycles[i]); // if the bike is being pedaled;
			losePower(Cycles[i]); // if stopped pedaling for duration of time	
	}
}


//==========================================================
//					Transmission Functions
//==========================================================
uint8_t Master::packRelay(bool toggle, uint8_t relay){
		uint8_t command = 0;
		if (toggle){
				command |= 0x10;
		}
		command |= relay;
		return command;
}

uint8_t Master::packLeds(){
	uint8_t command = 0;
	uint8_t mult = random(8) << 4;
	command |= mult;
	command |= device;
	return command;
}

void Master::transmit(uint8_t address, int8_t package){
		uint8_t error = 10;
		Wire.beginTransmission(address);
		Wire.write(package);
		delay(100);
		Serial.print("transmit package :: ");
		Serial.println(package);
		error = Wire.endTransmission();
}


//==========================================================
//				State Functions
//==========================================================
//initialize round
//TODO might need some padding
void Master::initRoundStart(){
		for (uint8_t i = 0; i < NUM_BIKES; i++){
				//if (!roundStarted && Cycles[i].energyMeter){
				if (!roundStarted && Cycles[i].encoder){
					roundStarted = true; 
					roundTimer = millis();
					Serial.println("roundInit");
					randomDevice();
					break;
				}
		}
}

void Master::winState(){
		
		uint8_t totalEnergy = 0;
		for (uint8_t i = 0; i < NUM_BIKES; i++){
				totalEnergy += Cycles[i].energyMeter;
		}
		transmit(leds, packLeds());
		optoTimer = millis();

		//TODO create map
		if (totalEnergy > 16){
				totalEnergy = 16;
		}
		currDevice = device;
		modeClk.setAlarm(timeMultipliers[totalEnergy/2] * totalEnergy / deviceInput[device]);
		reset();
}

bool Master::timeOut(){
		uint32_t currentTime = millis();
		if (modeClk.isSet()) { modeClk.poll(); }
		return (currentTime - roundTimer > roundMaxTimr);
}

void Master::reset(){
		roundTimer = 0;
		roundStarted = false;
		for (uint8_t i = 0; i < NUM_BIKES; i++){
				Cycles[i].acc = 0;
				Cycles[i].energyMeter = 0;
				Cycles[i].energyTimer = 0;
		}
		Serial.println("reset");
}

void Master::update(uint8_t action){
	//transmit(relay, packRelay(HIGH, audioOne));
	delay(200);
	Serial.print("WTF IS THE CURRENT DEVICE!! :: ");
	Serial.println(currDevice);
	transmit(relay, packRelay(HIGH, currDevice));
}

void Master::loop(){
	if (!roundStarted){
		initRoundStart(); //happens when someone started pedaling;
	}
	else{
		if (timeOut()) { winState(); }
	}
	adjustPower();
}
