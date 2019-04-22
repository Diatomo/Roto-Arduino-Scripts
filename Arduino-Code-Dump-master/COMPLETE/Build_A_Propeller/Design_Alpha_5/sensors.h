

/*
 *
 *
 *
 *
 *
 *
 *
 *
 */


#include <ADS1x15.h>
#include "stats.h"

#ifndef sensors_h
#define sensors_h


class Sensors{
		
		public:
				//constructor
				Sensors();
				
				//settings
				int static const BUTTON_PIN = 2;
				int static const NUM_SENSORS = 4;
				int static const DENOISE_COUNTER = 1;
				
				//sensor arrays
				int buttonState;
				int air[NUM_SENSORS * DENOISE_COUNTER];
				int deviations[NUM_SENSORS * DENOISE_COUNTER];

				//statistics
				int devCounter = 0;
				int avgDev = 0;
				int dev = 0;
				int avg = 0;
				int maximum = 9000;
				int minimum = 7000;
				
				//objects
				Stats statistics;
				ADS1115 adc;

				//functions
				void set_adc();
				void read();
				void analyzeSensorData();
				void processStats();
		
				//help functions
				void processSensorData();
				void filter();
};

#endif
