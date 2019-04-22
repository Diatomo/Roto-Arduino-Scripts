

#include <pwmBoard.h>

pwmBoard pwm = pwmBoard(0);

bool buttonDown = false;
uint8_t curr = 0;

struct colors{
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
}; typedef colors Colors;


Colors shadeA;
Colors shadeB;
Colors shadeC;
Colors shadeD;
Colors shadeE;
Colors shadeF;

Colors shades[6] = {shadeA, shadeB, shadeC, shadeD, shadeE, shadeF};

void setup(){
	Serial.begin(9600);
	pinMode(10, INPUT);

	shades[0].r = 214;
	shades[0].g = 255;
	shades[0].b = 99;

	shades[1].r = 154;
	shades[1].g = 199;
	shades[1].b = 60;

	shades[2].r = 157;
	shades[2].g = 255;
	shades[2].b = 253;

	shades[3].r = 243;
	shades[3].g = 165;
	shades[3].b = 125;

	shades[4].r = 238;
	shades[4].g = 89;
	shades[4].b = 166;

	shades[5].r = 84;
	shades[5].g = 122;
	shades[5].b = 255;

	pwm.start();

}


void cycleColor(){
	pwm.setLevel(0, shades[curr].r);
	pwm.setLevel(1, shades[curr].g);
	pwm.setLevel(2, shades[curr].b);
	Serial.println("SHADES");
	Serial.println(shades[curr].r);
	Serial.println(shades[curr].g);
	Serial.println(shades[curr].b);
}


void loop(){

	bool button = !digitalRead(10);
	if (button && !buttonDown){
		Serial.println("button down");
		buttonDown = true;
		curr = (curr + 1) % 6;
		cycleColor();
		delay(500);
	}
	else if (buttonDown && !button){
		buttonDown = false;
	}
	pwm.send();
}
