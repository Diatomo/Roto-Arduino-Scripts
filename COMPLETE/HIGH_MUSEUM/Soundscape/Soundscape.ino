
/*
   Roto

 Project: High Museum of Art
 Exhibit: Curiosity Cabinets
 Author(s): Charles Stevenson
 Date: 6/13/2018

 Original environment:
 Arduino 1.8.2

 Revision notes:

 	
 */


#include <ArduinoJson.h>
#include <buttonBoard.h>

boolean buttonDown = false;

//BUTTON BOARD Config
const uint8_t bbDI = 12; //button board data in
const uint8_t bbDO = 11; //button board data out
const uint8_t bbCLK = 10; //button board clock
const uint8_t bbILT = 9; //IN LATCH
const uint8_t bbOLT = 8; //OUT LATCH
const uint8_t nBB = 2; //number of button boards
const uint8_t nBUTTONS = 13; // number of buttons
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, nBB);//init bb

bool buttonState[nBUTTONS] = {false,false,false,false,false,false,false,false,false,false,false,false,false};
int8_t activeButt = -1;


//Data Collection
struct data{
	bool resetData;
};
typedef struct data Data; 

bool lockout = false;

void testButtons(){
	bb.setLamp(HIGH);
	delay(5000);
	bb.setLamp(LOW);
}

/*
*
*	FxN :: setup();
*
*/
void setup(){

  uint32_t BAUD_RATE = 9600;
  while (!Serial);

  testButtons();

  Serial.begin(BAUD_RATE);
}

/*
*
*	FxN :: echo
*		for testing, repeats package to PC
*/
void echo(JsonObject& package){
    package.prettyPrintTo(Serial);
}


/*
*
*	FxN :: send
*		Sends Json to PC
*/
void send(uint8_t currButt){
  StaticJsonBuffer<64> localBuffer;
  JsonObject& rPackage = localBuffer.createObject();
  rPackage["button"] = currButt;
  printJSON(rPackage);
}


/*
*
*	FxN :: printJSON
*		prints JSON to PC
*/
void printJSON(JsonObject& obj) {
  String jsonStringified = String("");
  obj.printTo(jsonStringified);
  jsonStringified.concat("\r\n");
  Serial.write(jsonStringified.c_str());
}

/*
*
*	FxN :: buttonHandler
*		Handles Button Presses
*/
bool buttonHandler(uint8_t button){
	bool currButt = bb.getButton(button);
	if (currButt && !buttonDown){
		send(button);
		activeButt = button;
		buttonState[button] = !buttonState[button];
		bb.setLamp(button, buttonState[button]);
		buttonDown = true;   
	}
	if(activeButt != -1 && !bb.getButton(activeButt) && buttonDown) {
		activeButt == -1;
		Serial.println("keyup");
		buttonDown = false;
		if (activeButt == 12){
			lockout = true;
		}
		//bb.setLamp(activeButt, LOW);
	}
}


void reset(){
	for (uint8_t button = 0; button < nBUTTONS; button++){
		buttonState[button] = false;
		bb.setLamp(button, buttonState[button]);
	}
	lockout = false;
}

/*
*
*	FxN :: loop
*		just calls the buttonHandler;
*/
void loop(){
	for (uint8_t button = 0; button < nBUTTONS; button++){
		if (!lockout) { buttonHandler(button); }
	}
	
	// ADDED BY JACOB
	read();
	// END ADDED BY JACOB
}

/**************   JACOB'S CODE  */

JsonObject& readJson(){
  StaticJsonBuffer<128> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(Serial);
  return root;
}

Data parseJson(JsonObject& root){
	Data package;
	package.resetData = root["reset"];
	return package;
}

// i skipped parsing into a struct, as its a little overkill for just one prop
void read(){
	if(Serial.available() > 0){
		JsonObject& root = readJson();
		Data package = parseJson(root);
		if(root.success() && parseJson(root).resetData){
			reset();
		}
	}
}

/**************   END JACOB'S CODE */


