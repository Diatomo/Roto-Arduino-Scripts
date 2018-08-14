/*
 * Roto
 *
 * Project: High Museum
 *
 *
 * Exhibit: Color Cavern
 *
 *
 * Author(s): Brady Schoeffler
 *
 *
 *
 * Original environment:
 * Arduino 1.0.5 - Roto Libraries xxxx.xx.xx
 *
 * Revision notes:
 *
 *
 *
 *
 */

#define versionString "Roto - [Project] [Exhibit]"

// Debug Mode (comment to disable)
//#define debugMode

// Libraries:
//#include <Wire.h> // Wire.h must be included before pwmBoard.h
#include <pwmBoard.h>
#include <FastLED.h>
#include <alarmClock.h>

TBlendType currentBlending;

DEFINE_GRADIENT_PALETTE (COLOR_GRADIENT) 
{
   0,   255,   0,   0,
  78,  255,  30,   0,
  131, 230, 255,   0,
  172,   0, 255,   0,
  212,   0, 0,   255,
  255,148,0,211,
};

CRGBPalette16 myPal = COLOR_GRADIENT;

// Pin Assignments:
#define DATA_PIN 3
#define ENCODER_PIN 17

// Constants:
#define updateTime 20
#define BRIGHTNESS 255
#define NUM_LEDS  1
#define NUM_COLORS 256
const byte numberOfBoards = 1;

CRGB leds[NUM_LEDS];

//prototypes
void colorUpdate();

// Global Variables:
byte red;
byte blue;
byte green;

// Global Classes:
repeatAlarm updateTimer = repeatAlarm(colorUpdate);
pwmBoard colorOut = pwmBoard(0, numberOfBoards);

void setup()
{
	Serial.begin(9600);
	Serial.println(versionString);
	Serial.print("File: ");
	Serial.println(__FILE__);

	/* note: If the board has FETs populated, pins 4-7 must be driven high or low.
	 * If the FETs are used, remove the following block of code.
	 */
  FastLED.addLeds<WS2811, DATA_PIN, BRG>(leds, NUM_LEDS);
  updateTimer.setInterval(updateTime);
  pinMode(ENCODER_PIN, INPUT);
  currentBlending = LINEARBLEND;
  colorOut.start(); //  start the i2c in the arduino and initilize the driver chip
  colorOut.setLevel(0); // turn all of the outputs off


}

void loop()
{
updateTimer.poll();
Serial.println(map(analogRead(ENCODER_PIN), 0, 1023, 0, 255));
}

void colorUpdate() 
{
  int pos = map(analogRead(ENCODER_PIN), 0, 1023, 0, 255);
  leds[0] = ColorFromPalette(myPal, pos);
  red = leds[0].red;
  green = leds[0].green;
  blue = leds[0].blue;
  colorOut.setLevel(0, red);
  colorOut.setLevel(1, green);
  colorOut.setLevel(2, blue);
  
}
