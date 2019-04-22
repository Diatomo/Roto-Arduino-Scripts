



/*
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
#include "sensors.h"
#include <Arduino.h>

#define size NUM_SENSORS * DENOISE_COUNTER

Sensors::Sensors(){

}

void Sensors::set_adc(){
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

void Sensors::read(){
  buttonState = digitalRead(BUTTON_PIN);
  for (int i=0; i<size; i++){
    air[i] = adc.analogRead(i);//read new values
	Serial.print(air[i]);
	Serial.print("\n");
  }
}

/*
 * 
 * Fxn :: analyzeSensorData
 *  Analyzes averages, stdDev, max
 *
 * 
 */
void Sensors::analyzeSensorData(){
		while (devCounter <= size){
				processStats();
				processSensorData();
		}
		filter();
		devCounter = 0;
}


void Sensors::processStats(){
		avg = statistics.average(air, size);
		Serial.print("\norig average! :: ");
		Serial.print(avg);
		Serial.print("\n");
		dev = statistics.stdDev(air, size);
}

void Sensors::processSensorData(){
		deviations[devCounter] = dev;
		devCounter++;
		if (devCounter == size){
				Serial.print("\nDEViation STATS");
				avgDev = statistics.average(deviations, size);
				Serial.print("avgDev");
				Serial.print("\n");
		}
}

void Sensors::filter(){
		int temp = 0;
		int counter = 0;
		for (int i=0; i<size;i++){
				if ((air[i] > avg-avgDev) && (air[i] < avg-avgDev)){
						temp += air[i];
						counter++;
				}
		}
		avg = temp/counter;
		Serial.print("\nNew average");
		Serial.print(avg);
		Serial.print("\n\n");
		maximum = max(maximum,avg);
}

