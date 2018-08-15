/*
 * Roto
 * Example Code
 *
 * Library:
 * pwmBoard
 *
 * Author(s):
 * Keegan Morrow
 *
 */

// Libraries:
#include <Wire.h> // Wire.h must be included before pwmBoard.h
#include <pwmBoard.h>

// Pin Assignments:
const byte fet4 = 4;
const byte fet5 = 5;
const byte fet6 = 6;
const byte fet7 = 7;

// Constants:
const byte numberOfBoards = 2;

// Global Variables:

// Global Classes:
/*
 * NOTE: The 8CH PWM board uses the hardware i2c interface
 * in the arduino and must be connected to the following pins:
 * SDA --> AN4 D18
 * SCL --> AN5 D19
 *
 * NOTE: Terminating resistors need to turned on on the last bord
 * on the bus only (if there is only one, it is the last).
 *
 * DIP switch settings:
 * Switches 1-4 set the address, 5 and 6 are the terminating resistors
 *  1  Address bit 3 (MSB)
 *  2  Address bit 2
 *  3  Address bit 1
 *  4  Address bit 0 (LSB)
 *  5  Terminator
 *  6  Terminator
 */
pwmBoard dimmer = pwmBoard(0, numberOfBoards);

void setup()
{
	pinMode(fet4, OUTPUT);
	pinMode(fet5, OUTPUT);
	pinMode(fet6, OUTPUT);
	pinMode(fet7, OUTPUT);
	digitalWrite(fet4, LOW);
	digitalWrite(fet5, LOW);
	digitalWrite(fet6, LOW);
	digitalWrite(fet7, LOW);

  dimmer.start(); //  start the i2c in the arduino and initilize the driver chip
  dimmer.setLevel(0); // turn all of the outputs off

  //dimmer.autoUpdate = false; // turn autoUpdate off and call .update() for better animation performance
}

void loop()
{
	static byte baseLevel;

	for (byte i = 0; i < (numberOfBoards * 8); i++)
	{
		dimmer.setLevel(i, baseLevel + i);
		delay(30);
	}

	baseLevel += (numberOfBoards * 8);
}

/*
alternate methods of sending levels:

dimmer.setLevel(index, level); //set output 'index' to 'level' in the output buffer
dimmer.update(); //send the output buffer

dimmer.getPtr()[index] = level;  //set output 'index' to 'level' in the output buffer
dimmer.update(); //send the output buffer

*(dimmer.getPtr()+index) = level;  //set output 'index' to 'level' in the output buffer
dimmer.update(); //send the output buffer

dimmer.setLevel(level); //sets all outputs to 'level' in the output buffer
dimmer.update(); //send the output buffer
*/


