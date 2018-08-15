

#ifndef master_h
#define master_h

#include <Arduino.h>
#include <Wire.h>
#include <alarmClock.h>


//struct template
struct cycle{
		uint8_t encoder;
		uint32_t acc;
		uint32_t prevAcc;
		uint8_t energyMeter;
		uint32_t energyTimer;
		//DigitGroup* display;
};
typedef cycle Cycle;

class Master{
		public:
				Master();
				void initRoundStart();
				void randomDevice();
				void gainPower(Cycle&);
				void losePower(Cycle&);
				void adjustPower();
				uint8_t packRelay(bool, uint8_t);
				void winState();
				void transmit(uint8_t, int8_t);
				bool timeOut();
				void reset();
				void loop();
				void update(uint8_t action);
				uint8_t packLeds();
				
				alarmClock modeClk;

				//constants
				uint32_t static const NUM_BIKES = 3;
				uint32_t static const NUM_LEDS = 12;
				uint32_t static const accTHRESH = 10; 
				uint32_t static const emTHRESH = 20;
				uint32_t static const losePowerTHRESH = 3000;

				//slave boards
				uint8_t static const leds = 10;
				uint8_t static const relay = 11;

			
				//round Timer
				uint32_t static const roundMaxTimr = 30000;
				uint32_t roundTimer = 0;
				bool roundStarted = false;

				//devices
				uint8_t device;
				uint8_t const MAX_DEVICES = 5;
				uint8_t audioOne = 5;
				uint8_t audioTwo = 6;
				uint32_t const OPTO_TIMER = 60000 * 6;//optocopter Max Timer
				uint32_t optoTimer = 0; //optocopter

				//collection of bikes
				Cycle Cycles[NUM_BIKES];

				//alarmClocks	
				uint32_t resetTimr = 30000; // 30seconds
				uint8_t currDevice = 0;

};
#endif
