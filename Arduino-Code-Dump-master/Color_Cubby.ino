
/*
   Roto
 
 Project: High Museum
 Exhibit: Color Cubby
 Author(s): Charles Stevenson
 Date: 7/23/18
 
 Original environment:
 Arduino 1.8.2
 
 Revision notes:

 	I through this together in about 15minutes. 
	NOT TESTED!! Will Test on Final Exhibit.

 */


//PWM BOARD Config
byte const addy = 15;
pwmBoard pwm = pwmBoard(addy);


uint32_t const SENSITIVITY = 10000; //SENSITIVITY
uint8_t const MAX_COLOR_VAL = 254; 

uint8_t currHertz = 0;//starting color;
int sensCounter = SENSITIVITY/2;


/* ===============================================*/
/* ===========   TEST FUNCTIONS   ================*/
/* ===============================================*/
void setupPWM(){
	//brighten lights
	for (uint8_t i = 0; i < MAX_COLOR_VAL; i++){
	pwm.setLevel(i);
	pwm.send();
	}

	//dim lights
	for (uint8_t i = MAX_COLOR_VAL; i > 0; i--){
	pwm.setLevel(i);
	pwm.send();
	}

	//set lights to zero.
	pwm.setLevel(0);
	pwm.send();
}


/*
*
* FxN :: setup
*
*
*/
void setup(){
    
    //Open Serial && write version
    Serial.begin(BAUD_RATE);
    Serial.println(versionString);
    Serial.print(F("File: "));
    Serial.println(__FILE__);

    //pins
    pinMode(chanA, INPUT);
    pinMode(chanB, INPUT);

    //interrupt
    attachInterrupt(digitalPinToInterrupt(2),changeColor,CHANGE); 

    //setupBoards
    setupPWM();
}



/*
*
*
* FxN :: changeColor
*   Interrupt Function, that counts up and down the encoder values.
*
*/
void changeColor(){
    if (digitalRead(chanA) != digitalRead(chanB)){
        if (sensCounter < SENSITIVITY && (currHertz < (NUM_HERTZ-1))){
            sensCounter++;
        }
    }
    else{
        if (sensCounter > 0 && (currHertz > 0)){
            sensCounter--;
        }
    }
}


/*
*
*
* FxN :: analyzeColor
*   Analyzes when a change in ecoder values cascades to a recorded change in value
*
*/
boolean analyzeColor(){
    boolean change = false;
    if (sensCounter >= SENSITIVITY){
        if (currColor < MAX_COLOR_VAL){
            currColor++;
            sensCounter = SENSITIVITY/2;
            change = true;
        }
    }
    else if (sensCounter <= 0){
        if (currColor > 0){
            currColor--;
            sensCounter = SENSITIVITY/2;
            change = true;
        }
    }
    return change;
}

/*
*
*
* FxN :: updateColor
*   updates color when a change occurs
*
*/
void updateColor(){
	pwm.setLevel(currColor);
	pwm.send();
}

/*
*
* FxN :: loop
*   loops by analyzing a change then updated to match to corresponding change;
*
*/
void loop(){
    if (analyzeColor()) { updateColor(); }
}


