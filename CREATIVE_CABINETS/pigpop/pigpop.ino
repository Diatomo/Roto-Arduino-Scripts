/*
 * Roto
 *
 * Project:
 * Striner
 *
 * Exhibit:
 * Pig Popper
 *
 * Author(s):
 * Keegan Morrow
 *
 *
 * Original environment:
 * Arduino 1.0.6 - Roto Libraries xxxx.xx.xx
 *
 * Revision notes:
 *
 *
 *
 *
 */

// Libraries:
//#include <SPI.h>
//#include <LED2801.h>
#include <FastLED.h>
#include <buttonBoard.h>
#include <digit.h>
#include <alarmClock.h>
#include "color.h"


// Pin Assignments:
const byte fet4 = 4;
const byte fet5 = 5;
const byte fet6 = 6;
const byte fet7 = 7;

const byte segData = 3;

const byte bbDI = 18;
const byte bbDO = 17;
const byte bbCLK = 16;
const byte bbILT = 15;
const byte bbOLT = 14;

// Constants:
const unsigned long p1color = 0xFF0000;
const unsigned long p2color = 0x00FF00;
const unsigned long offColor = 0x000000;
const unsigned long attractOffColor = 0x0000FF;
const unsigned long attractOnColor = 0x00FF00;

const byte bbCount = 2;
const byte LEDcount = 16;
const byte buttonsPerPlayer = 8;

const byte scoreDigitCount = 3;
const byte timeDigitCount = 3;
const byte p1ScoreDigitOffset = 0;
const byte timeDigitOffset = 6;
const byte p2ScoreDigitOffset = 3;

const byte midiVel = 127;
const byte midiMusic = 47;
const byte p1sounds[] = {51, 52, 53, 54};
const byte p2sounds[] = {55, 56, 57, 58};

const unsigned long gameRunTime = 23000;
const unsigned long attractStepTime = 750;
const unsigned long endGameDelayTime = 3000;

const byte locArray[] = {1, 6, 3, 7, 0, 1, 3, 5, 4, 7, 5, 2, 3, 6, 2, 0, 1, 7, 4, 0, 6, 5};
uint8_t const NUMLEDS = 16;
CRGB leds[NUMLEDS];
const byte revLed[] = {7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8};

// Global Variables:
boolean gameActive;
byte p1score, p2score;

boolean startUp = false;

// Global Classes:
buttonBoard buttons = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, bbCount);

//LED2801 LEDs = LED2801(LEDcount);


uint8_t const p1Addy = 0;
uint8_t const p1Length = 3;
uint8_t const p2Addy = 3;
uint8_t const p2Length = 3;

digit p1ScoreDisplay = digit(segData, p1Addy, p1Length);
digit p2ScoreDisplay = digit(segData, p2Addy, p2Length);
digit timeDisplay = digit(segData+1, 15, 2);

void stopGame();
void refreshDisplay();
void refreshButtons();
void attractIndexHandler();
void sleep();

alarmClock gameTimer = alarmClock(stopGame);
repeatAlarm displayRefreshTimer = repeatAlarm(refreshDisplay);
repeatAlarm buttonRefreshTimer = repeatAlarm(refreshButtons);
repeatAlarm attractIndexTimer = repeatAlarm(attractIndexHandler);
repeatAlarm sleepTimer = repeatAlarm(sleep);

void setup()
{
	Serial.begin(31250); // MIDI
	//Serial.begin(9600); // TEST

	pinMode(fet4, OUTPUT);
	pinMode(fet5, OUTPUT);
	pinMode(fet6, OUTPUT);
	pinMode(fet7, OUTPUT);
	digitalWrite(fet4, LOW);
	digitalWrite(fet5, LOW);
	digitalWrite(fet6, LOW);
	digitalWrite(fet7, LOW);

	delay(500);

	uint8_t const Data = 11;
	uint8_t const Clock = 13;
	FastLED.addLeds<WS2801, Data, Clock, RGB>(leds, NUMLEDS);
	//LEDs.setColorOrder(&_RGB);
	//LEDs.setAll(100);
	for (uint8_t i = 0; i < NUMLEDS; i++){
		leds[i] = CRGB::White;
	}
	FastLED.show();

	p1ScoreDisplay.segDisp(test);
	p2ScoreDisplay.segDisp(test);
	timeDisplay.segDisp(test);
	delay(2000);
	p1ScoreDisplay.segDisp(1);
	p2ScoreDisplay.segDisp(2);
	timeDisplay.segDisp(3);
	
	//LEDs.setAll(0);
	for (uint8_t i = 0; i < NUMLEDS; i++){
		leds[i] = CRGB::Black;
	}
	FastLED.show();

	delay(1000);
	p1ScoreDisplay.segDisp(blank);
	p2ScoreDisplay.segDisp(blank);
	timeDisplay.segDisp(blank);
	delay(500);


	if (buttons.countPressed() > 0)
	{
		buttons.attachHook(testLights);
		while (true)
		{
			for (byte i = 0; i < buttons.getSize(); i++)
			{
				buttons.byteWrite(i, buttons.byteRead(i));
			}
			p1ScoreDisplay.segDisp(buttons.countPressed());
			delay(1);
		}
	}

	buttons.attachHook(gameLights);

	displayRefreshTimer.setInterval(20);
	buttonRefreshTimer.setInterval(5);
	attractIndexTimer.setInterval(attractStepTime);
    sleepTimer.setInterval(3600000);
	buttons.autoUpdate = false;
//	LEDs.autoUpdate = false;
}

void loop()
{
        if (startUp == false)
        {
          startUp = true;
          delay(2000);
          digitalWrite(fet4, HIGH);
        }
        
	static byte p1node;
	static byte p2node;

	buttons.update();

	if ((buttons.countPressed() > 0) && (gameActive == false))
	{
		gameActive = true;
		p1score = 0;
		p2score = 0;
		startMusic();
		gameTimer.setAlarm(gameRunTime);
		p1node = getNextLoc1();
		p2node = getNextLoc2();
		if (buttons.getButton(p1node)) { p1node = getNextLoc1(); }
		if (buttons.getButton(p2node)) { p2node = getNextLoc2(); }
		buttons.setLamp(false);
		buttons.setLamp(p1node, true);
		buttons.setLamp(p2node, true);
		buttons.update();
	}

	if (gameActive)
	{
		if ((buttons.countPressed(0, buttonsPerPlayer) == 1) && (buttons.getButton(p1node)))
		{
			playSound1();
			buttons.setLamp(p1node, false);
			p1node = getNextLoc1();
			buttons.setLamp(p1node, true);
			p1score++;
		}
		if ((buttons.countPressed(8, buttonsPerPlayer) == 1) && (buttons.getButton(p2node)))
		{
			playSound2();
			buttons.setLamp(p2node, false);
			p2node = getNextLoc2();
			buttons.setLamp(p2node, true);
			p2score++;
		}
	}

	buttonRefreshTimer.poll();
	gameTimer.poll();
	displayRefreshTimer.poll();
	attractIndexTimer.poll();
    sleepTimer.poll();
}

void sleep()
{
	Serial.write(0x90);
	Serial.write(p1sounds[0]);
	Serial.write(midiVel);
}

void attractIndexHandler()
{
	if (gameActive) { return; }

	static byte lastIndex;
	byte attractIndex = random(0, 15);
	while (attractIndex == lastIndex)
	{
		attractIndex = random(0, 15);
	}
	lastIndex = attractIndex;
	buttons.setLamp(false);
	buttons.setLamp(attractIndex, true);
}

void stopGame()
{
	buttons.setLamp(false);
	buttons.update();
	// LEDs.setColor(0x000000);
	for (uint8_t i = 0; i < NUMLEDS; i++){
		leds[i] = CRGB::Black;
	}
	FastLED.show();
	gameActive = false;
	timeDisplay.segDisp(dash);
	stopMusic();
	delay(endGameDelayTime);
}

void refreshButtons()
{
	buttons.update();
}

void refreshDisplay()
{
	p1ScoreDisplay.segDisp(p1score);
	p2ScoreDisplay.segDisp(p2score);
	//Serial.println("refreshing display");
	if (gameActive) { timeDisplay.segDisp((gameTimer.getRemainingTime() / 1000) + 1); }
	else { timeDisplay.segDisp(dash); }
}

byte getNextLoc1()
{
	static byte index;
	index++;
	if (index >= sizeof(locArray)) { index = 0; }
	return locArray[index];
}

byte getNextLoc2()
{
	static byte index;
	index++;
	if (index >= sizeof(locArray)) { index = 0; }
	return locArray[index] + 8;
}

void startMusic()
{
	Serial.write(0x90);
	Serial.write(midiMusic);
	Serial.write(midiVel);
}

void stopMusic()
{
	Serial.write(0x80);
	Serial.write(midiMusic);
	Serial.write(0);
}

void playSound1()
{
	static byte index;
	Serial.write(0x90);
	Serial.write(p1sounds[index++]);
	Serial.write(midiVel);
	if (index >= sizeof(p1sounds)) { index = 0; }
}

void playSound2()
{
	static byte index;
	Serial.write(0x90);
	Serial.write(p2sounds[index++]);
	Serial.write(midiVel);
	if (index >= sizeof(p2sounds)) { index = 0; }
}

/*
* This function is called during buttons.update() if it is attached
*/
void gameLights()
{
	uint8_t numButtons = 8;
	if (gameActive){
		for (byte i = 0; i < 8; i++)
		{
			//LEDs.setColor(i, buttons.getLampState(i) ? p1color : offColor);
			leds[revLed[i]] = buttons.getLampState(i) ? CRGB::Red : CRGB::Black;
			//LEDs.setColor(i + 8, buttons.getLampState(i + 8) ? p2color : offColor);
			leds[revLed[i+8]] = buttons.getLampState(i+8) ? CRGB::Green : CRGB::Black;
		}
	}
	else
		for (byte i = 0; i < 16; i++)
		{
			//LEDs.setColor(i, buttons.getLampState(i) ? attractOnColor : attractOffColor);
			leds[revLed[i]] = buttons.getLampState(i) ? CRGB::Blue : CRGB::Green;
		}
	//LEDs.send();
	FastLED.show();
}

/*
* This function is called during buttons.update() if it is attached
*/
void testLights()
{
	for (byte i = 0; i < 16; i++){
		leds[revLed[i]] = buttons.getLampState(i) ? CRGB::Red : CRGB::Green;
		//LEDs.setColor(i, buttons.getLampState(i) ? 0xff0000 : 0x00ff00);
		if (buttons.getLampState(i) == true){
			Serial.println(i);
		}	
	}
	FastLED.show();
	//LEDs.send();
}
