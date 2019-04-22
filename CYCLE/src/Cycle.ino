
/*
 * ===================================================
 *
 * 					Cycle To Power
 * 	  Charles C. Stevenson			  06/18/18
 *
 * 	  Description:
 * 	  	Cycle To Power Based on platformio.
 *
 *
 * ===================================================
 */

#include <newDigits.h>
#include "master.h"
#include "cRelay.h"
#include "cLeds.h"
#include <Wire.h>
#include <alarmClock.h>

Master overmind = Master();
CRelay relay = CRelay();
CLeds leds = CLeds();

Digits digA = Digits(8);
DigitGroup* groupBike;
DigitGroup* groupDevice;
DigitGroup* centralClock;

bool started = false;
int8_t currDevice = -1;

uint8_t deviceAddy[5] = {17,13,5,9,1};
//0  == correct trace and segment;;
//4 == correct trace and segment
//2 = correct trace and segment

//3 = INCORRECT;;

//device 2 = coplights;
//


uint8_t clockAddress = 20;

//switches
bool leds_upload;
bool relay_upload;

bool eventHappened = false;
uint8_t action = 0;

void updateBikes();
void updateDevice();

alarmClock update = alarmClock(updateBikes);
uint32_t const TIMR = 1000;
uint8_t const Counter = 30;
int8_t counter = 30;

alarmClock deviceClk = alarmClock(updateDevice);
uint32_t const deviceTIMR = 50;

boolean relayON = false;
int32_t total = 0;


//SETUP
//================================================================
void setup_leds_comm(){
		//begin i2c connection (reader :: slave)
		uint8_t address = 10;
		Wire.begin(address);
		Wire.onReceive(event);
		leds.testLeds();
}

void setup_relay_comm(){
		//begin i2c connection (reader :: slave)
		uint8_t address = 11;
		Wire.begin(address);
		Wire.onReceive(event);
}

void setup_master_comm(){
	uint8_t address = 12;
	Wire.begin(address);
	Wire.onReceive(event);
}

void setupPinsInput(int start, int end){
	for (int i = start; i <= end; i++){
		pinMode(i, INPUT);
	}
}

void setupPinsOutput(int start, int end){
	for (int i = start; i <= end; i++){
		pinMode(i, OUTPUT);
	}
}

void setup(){

	uint32_t const BAUD = 9600;
	Serial.begin(BAUD);
	Serial.println("started!");

	pinMode(14,INPUT);
	pinMode(15,INPUT);
	pinMode(16,INPUT);
	leds_upload = digitalRead(14);
	relay_upload = digitalRead(15);

	Serial.print("LEDS :: ");
	Serial.println(leds_upload);
	Serial.print("RELAY :: ");
	Serial.println(relay_upload);

/*
	bikes[0] = dig.addGroup(1,2);
	bikes[1] = dig.addGroup(4,2);
	bikes[2] = dig.addGroup(6,2);
*/
	groupBike = digA.addGroup(31,2);
	groupDevice = digA.addGroup(0x3F,3);
	groupDevice->segDisp(0,false);
	centralClock = digA.addGroup(21,3);

	if (!leds_upload && !relay_upload){
		setupPinsInput(4,6);// start pin, end pin
		pinMode(8, OUTPUT);
		pinMode(9, OUTPUT);
	}
	else{
		setupPinsOutput(4,11);// start pin, end pin
	}

	//depends on circuit board upload	
	if (leds_upload) { setup_leds_comm(); }
	else if (relay_upload) { setup_relay_comm(); }
	else{ setup_master_comm(); }
}

//==================================================================


//event for wire
void event(int howmany){
		action = Wire.read();
		eventHappened = true;
}

//what is this again??TODO
void transmit(){
		Wire.beginTransmission(0b00001010);
		Serial.println("transmit addy");
		Wire.write(20);
		delay(100);
		Serial.println("transmit package");
		Serial.println(20);
		Wire.endTransmission();
}


void updateBikes(){
	if (counter != 0){
		groupBike->segDisp(counter, false);
	}
	uint32_t sum = 0;
	for (uint8_t i = 0; i < 3; i++){
		sum += overmind.Cycles[i].energyMeter;
	}
	centralClock->segDisp(sum, false);
	counter--;
	if (counter == 0 || overmind.roundStarted == false){
		if (counter == 0) { total = sum;}
		counter = 30;
	}
}

void updateDevice(){
	if (!started){
		groupDevice->changeAddress(deviceAddy[overmind.currDevice]);
		started = true;
	}
	groupDevice->segDisp(total, false);
	Serial.print("device :: ");
	Serial.println(overmind.device);
	total = total - 1;
	if (total <= 0){
		relayON = false;
		groupDevice->segDisp(0, false);
		started = false;
	}
}

void loop(){
	//if led and relay read low then activate overmind (master)
	if (!leds_upload && !relay_upload) { 
		overmind.loop(); 
		if (!update.isSet()){
			update.setAlarm(TIMR);
		}
		else{
			update.poll();
		}
	}
		
	if (!deviceClk.isSet() && relayON){
			deviceClk.setAlarm(50);
		}
		else{
			deviceClk.poll();
		}	
	
	//Communication Loop
	if (eventHappened){
		if (leds_upload) { leds.update(action); Serial.println("leds"); }
		else if (relay_upload) { relay.update(action); Serial.println("relay"); }
		else {overmind.update(action); relayON = true; Serial.println("overmind event");}
		eventHappened = false;
		action = 0;
	}	
	if (relay_upload) {relay.loop(); updateDevice();}
}
