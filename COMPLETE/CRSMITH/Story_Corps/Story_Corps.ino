
#include <pwmBoard.h>

#define ON 1
#define OFF 0

byte const addy = 15;
pwmBoard pwm = pwmBoard(addy);

int level = 0;
int minLevel = 0;
int maxLevel = 150;

uint32_t const BAUD_RATE = 9600;

void setup(){
	Serial.begin(BAUD_RATE);
	pwm.start(); pwm.setLevel(0); pwm.send(); //init pwm and set init params
}

void rampUp(){
	for (int i = 0; i < maxLevel; i++){
		pwm.setLevel(i); 
		pwm.send();
		delay(20);
	}
}

void rampDown(){
	for (int i = maxLevel; i >= 0; i--){
		pwm.setLevel(i); 
		pwm.send();
		delay(10);
	}
}

void loop(){
	int input = Serial.read();
	if (input == ON){ rampUp(); }
	else if (input == OFF){ rampDown(); }
}
