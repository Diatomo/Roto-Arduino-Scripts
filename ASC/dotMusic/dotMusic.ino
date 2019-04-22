

/*
   Roto
 
 Project: Adventure Science Museum
 Exhibit: Dot Music
 Author(s): Charles Stevenson
 Date: 10/11/18
 
 Original environment:
 Arduino 1.8.2
 
 Revision notes:

 
 */

#define debug false

#include <LIDARLite.h>
#include <alarmClock.h>
#include <SoftwareSerial.h>
#include <PacketSerial.h>

uint32_t sensorVal = 0;

PacketSerial myPacketSerial;
LIDARLite sensor;
void exhibit();
alarmClock clk = alarmClock(exhibit);
uint32_t timr = 50; 

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
	sensor.begin(0,true);
	myPacketSerial.setStream(&Serial);
}


void readSensors(){
		sensorVal = sensor.distance(true);
}


/*
*
*	FxN :: play
*		plays a sound
*/
void play(){

	//Serial.print(sensorVal);
	Serial.write(sensorVal);
	//uint8_t myPackets[1] = {sensorVal};
	//myPacketSerial.send(myPackets,1);
	//if ((sensor->brokenBeam && sensor->active) || (sensor->currSound != sensor->prevSound)){
	/*
	MIDISerial.write(0x91);
	MIDISerial.write(sensor->currSound);
	MIDISerial.write(127);
	Serial.print("sound :: ");
	Serial.println(sensor->currSound);
	sensor->prevSound = sensor->currSound;
	sensor->active = false;
	played = true;
	*/
	//}
}


void exhibit(){
	readSensors();
	play();
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
