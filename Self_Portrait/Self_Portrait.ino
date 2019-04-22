
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
const uint8_t nBB = 1; //number of button boards
const uint8_t nBUTTONS = 1; // number of buttons
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, nBB);//init bb


/*
*
*	FxN :: setup();
*
*/
void setup(){

  uint32_t BAUD_RATE = 9600;
  while (!Serial);

  Serial.begin(BAUD_RATE);
  testSerial.begin(BAUD_RATE);
  Serial.println(versionString);
  Serial.print(F("File: "));
  Serial.println(__FILE__);
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
		send(uint8_t currButt);
		buttonDown = true;   
	}
	if(!currButt && buttonDown) {
		buttonDown = false;
	}
}


/*
*
*	FxN :: loop
*		just calls the buttonHandler;
*/
void loop(){
	for (uint8_t button = 0; button < nBUTTONS; button++){
			buttonHandler(button);
	}
}
