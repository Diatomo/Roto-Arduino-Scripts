/*
*Roto
*
*Project: 
* Knox Cubes
*
*Author(s): 
* Charles C. Stevenson
*
*Original environment:
* Arduino 1.0.5 - Roto Libraries xxxx.xx.xx
*
* Notes:
*
*	Description :: 
*		Knox Cubes is an exhibit for the Akron Psychology Musuem. Invented by a Medical Officer at Ellis Island, 
*		Dr. Howard Andrew Knox, to test the intelligence of immigrants. It operates exactly like the game simon
*		says where there is a sequence delivered to the player in which the play replicates by pushing the same
*		sequence of buttons. 
*
*
*	Program ::
*		1) Sequences are delivered in a predetermined order
*		2) Player tries to replicate sequence within 30 seconds
*			a) If input is correct plays correct sound and moves on to next in the sequence
*			b) If input is incorrect plays incorrect sound and remains on the same element in the sequence
*		3) Once all inputs are entered correct before the timer runs out, victory is achieved and animation plays.
*
*/


//Libraries
#include <buttonBoard.h>
#include <alarmClock.h>

#define versionString "Roto - Akron Physchology :: Knox Cubes"

//Settings
uint32_t const BAUDRATE = 9600;
uint8_t const nBB = 2;
uint8_t const nButtons = 4;
uint8_t const nMODES = 3;
uint8_t const OFFSET = 3;
uint32_t const TIMER = 60000;
uint8_t const correctSound = 12;
uint8_t const incorrectSound = 13;
uint8_t const victorySound = 14;
uint8_t const silent = 15;

uint8_t const start = 3;

//attraction variables
uint8_t tempCounter = 0;
unsigned long chaser = 1000;
unsigned long pTime = millis();
bool lit = false;


//Button Board Settings
const uint8_t bbDataIn = 18;
const uint8_t bbDataOut = 17;
const uint8_t bbClk = 16;
const uint8_t bbLtIn = 15;
const uint8_t bbLtOut = 14;

//objects
buttonBoard bb = buttonBoard(bbDataIn, bbDataOut, bbClk, bbLtIn, bbLtOut, nBB);
void reset(); //prototype
alarmClock alarm = alarmClock(reset);


//modes :: Answer Sequences
uint8_t const sEasy = 3;
uint8_t easy[sEasy] = {1,4,2};

uint8_t const sMedium = 5;
uint8_t medium[sMedium] = {1,3,2,4,1};


uint8_t const sHard = 9;
uint8_t hard[sHard] = {1,4,3,4,2,3,3,2,2};

uint8_t sizes[nMODES] = {sEasy,sMedium,sHard};
uint8_t* modes[nMODES] = {easy,medium,hard}; //TODO array of pointers pointing to arrays.

bool win = true;
int8_t difficulty = -1;

void setup(){
		Serial.begin(BAUDRATE);
		Serial.println(versionString);

		//led check
		for (byte i = 0; i <= nButtons+OFFSET; i++){
				bb.setLamp(i,HIGH);
		}
		delay(1000);
		bb.setLamp(LOW);
}


/*
		FxN :: attract
				Bouncer algorithm to attract individuals
*/
void attract(){
	if (millis() - pTime > chaser){
		for (byte i = nButtons; i <= nButtons + OFFSET; i++){
				if (lit == false){
						bb.setLamp(i , HIGH);
				}
				else if (lit == true){
						bb.setLamp(i , LOW);
				}
		}
		pTime = millis();
		lit = !lit;
	}
}


/*
		FxN :: reset
				resets game state
*/
void reset(){
		win = true;
		difficulty = -1;
		bb.setLamp(LOW);
		alarm.unSetAlarm();
}


/*
		FxN :: playSound
				plays parameterized Sound
*/
void playSound(uint8_t const sound){
		bb.setLamp(silent,HIGH);
		delay(50);
		bb.setLamp(sound, HIGH);
		delay(50);
		bb.setLamp(silent, LOW);
		delay(50);
		bb.setLamp(sound,LOW);
}

/*
		FxN :: play
		Params ::
				arr[] :: correct Sequence
				size :: number of elements in sequence
		Initiates gameplay delivers sequence and then opens user to answer.

*/
void play(uint8_t arr[], uint8_t const size){
		win = false;
		simon(arr, size);
		answer(arr, size);
}

/*
		FxN :: simon
		Params ::
				arr[] :: correct Sequence
				size :: number of elements in sequence
		Delivers sequence to player
*/
void simon(uint8_t arr[], uint8_t const size){
		for (byte i = 0; i < size; i++){//TODO Check to see how fast these should show.
				bb.setLamp(arr[i]+OFFSET, HIGH);
				delay(500);
				bb.setLamp(arr[i]+OFFSET, LOW);
				delay(500);
		}
}

/*
		FxN :: answer
		Params ::
				arr[] :: correct Sequence
				size :: number of elements in sequence
		Registers correct and incorrect answers from player.
*/
void answer(uint8_t arr[], uint8_t const size){
		alarm.setAlarm(TIMER);
		byte counter = 0;
		while (win == false){
				alarm.poll();
				for (byte i = nButtons; i <= nButtons+(OFFSET); i++){
						if (bb.getButton(i) == HIGH){
								bool correctAnswer = (arr[counter] == i-(OFFSET));
								if (correctAnswer == true){//if correct answer
										bb.setLamp(i, HIGH);
										playSound(correctSound);//TAKE THIS LINE OUT to avoid playing sound every correct press.
										counter++;
										delay(500);
										if (counter == size){//win state check
												win = true;
												victory();
										}
										bb.setLamp(i, LOW);

								}
								else if (correctAnswer == false){//if incorrect answer
										playSound(incorrectSound);
										failure();
										reset();
								}
						}
				}
		}
}


/*

		FxN :: failure
				Instructions for failure animation
*/
void failure(){
		for (byte i = 0; i <= nButtons+OFFSET; i++){
				bb.setLamp(i, HIGH);
		}
		delay(2000);
		bb.setLamp(LOW);
}

/*
		FxN :: victory
				Instructions for ending animation sequence
*/
void victory(){
		delay(300);
		playSound(victorySound);
		for (byte j = 0; j < nButtons; j++){
				bb.setLamp(LOW);
				delay(300);
				for (byte i = 0; i <= nButtons + OFFSET; i++){
						bb.setLamp(i, HIGH);
				}
		delay(300);
		}
		delay(3000);
		bb.setLamp(LOW);
		reset();
}


/*
		FxN :: loop
				Waits for mode to be entered.
*/
void loop(){
		attract();
		alarm.poll();
		for (byte i = 0; i < nButtons-1; i++){//searches for difficulty to set
				if (bb.getButton(i) == HIGH){
						if (difficulty != i){
								alarm.setAlarm(TIMER);
								difficulty = i;
								for (byte j = 0; j < nButtons; j++){
										bb.setLamp(j, LOW);
								}
								delay(50);
								bb.setLamp(difficulty, HIGH);
						}
				}
		}
		if (difficulty > -1){//starts when difficult is set
				if (bb.getButton(start) == HIGH){
						bb.setLamp(LOW);
						bb.setLamp(difficulty, HIGH);
						bb.setLamp(start, HIGH);
						delay(1000);
						play(modes[difficulty], sizes[difficulty]);
				}
		}
}
