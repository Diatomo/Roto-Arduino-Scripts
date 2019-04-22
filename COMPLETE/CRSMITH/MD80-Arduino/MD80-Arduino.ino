/*
   Roto

 Project: CR. SMITH
 Exhibit: MD80
 Author(s): Charles Stevenson
 Date: 6/13/2018

 Original environment:
 Arduino 1.8.2

 Revision notes:

 */


/* ====================================================*/
/* =========    SCRIPT CONFIGURATION   ================*/
/* ====================================================*/

#include <pwmBoard.h>
#include <ArduinoJson.h>
#include <buttonBoard.h>
#include <SoftwareSerial.h>

#define versionString "Roto - [CRSmith] [MD80]"

// button control variables
boolean buttonDown = false;

const int8_t ON = 1;
const int8_t OFF = 0;

//BUTTON BOARD Config
const uint8_t bbDI = 12; //button board data in
const uint8_t bbDO = 11; //button board data out
const uint8_t bbCLK = 10; //button board clock
const uint8_t bbILT = 9; //IN LATCH
const uint8_t bbOLT = 8; //OUT LATCH
const uint8_t nBB = 1; //number of button boards
const uint8_t nBUTTONS = 1; // number of buttons
const uint8_t button = 0;
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, nBB);//init bb

//PWM BOARD Config
byte const addy = 15;
pwmBoard pwm = pwmBoard(addy);

//debug Software Serial
const byte rxPin = 14;
const byte txPin = 15;
SoftwareSerial testSerial (rxPin, txPin);

//Data Collection
struct data{
  int8_t action;
  int8_t feature;
  uint32_t duration;
};
typedef struct data Data;

/* ===============================================*/
/* ===========   TEST FUNCTIONS   ================*/
/* ===============================================*/
void testPwm(uint8_t testLevel){

  //brighten lights
  for (uint8_t i = 0; i < testLevel; i++){
    fadeIn(5000);
  }

  //dim lights
  for (uint8_t i = testLevel; i > 0; i--){
    fadeOut(5000);
  }

  //set lights to zero.
  pwm.setLevel(0);
  pwm.send();
}

void testButtons(){

  //local inits
  uint32_t const delayTimer = 100;
  uint32_t const numBlinks = 5;
  bb.setLamp(LOW);

  //blink lights
  for (uint32_t i = 0; i < numBlinks; i++){
    bb.setLamp(button, HIGH);
    delay(delayTimer);
    bb.setLamp(button, LOW);
    delay(delayTimer);
  }
}

void setup(){

  //setup Serial
  uint32_t BAUD_RATE = 9600;
  while (!Serial);

  Serial.begin(BAUD_RATE);
  testSerial.begin(BAUD_RATE);
  Serial.println(versionString);
  Serial.print(F("File: "));
  Serial.println(__FILE__);
  Serial.print("Running Startup Tests.....");

  //setup pwm
  pwm.start(); pwm.setLevel(0); pwm.send();

  //run initial tests
  testPwm(255);
  testButtons();
  Serial.println("SUCCESS");
  delay(200);
  Serial.println("Starting Loop.");

 }

/* ===============================================*/
/* ===========   Parse FUNCTIONS   ================*/
/* ===============================================*/

JsonObject& readJson(){
  StaticJsonBuffer<128> jsonBuffer; //heap
  JsonObject& root = jsonBuffer.parseObject(Serial);
  //echo(root);
  return root;
}

Data parseJson(JsonObject& root){
  Data package;
  package.action = root["action"];
  package.feature = root["feature"];
  package.duration = root["duration"];
  printJSON(root);
  return package;
}

void echo(JsonObject& package){
    package.prettyPrintTo(Serial);
}

void playFeature(Data package){
  if (package.action == ON){
    if (package.feature == 0){
		fadeInAmbient(package.duration);
    }
    else{
      fadeIn(package.feature + 2, package.duration); //2 is an offset.
    }
  }
  else if (package.action == OFF){
    if (package.feature == 0){
		fadeOutAmbient(package.duration);
    }
    else{
      fadeOut(package.feature + 2, package.duration); //2 is an offset.
    }
  }
}

/* ===============================================*/
/* ===========   Feature FUNCTIONS   ================*/
/* ===============================================*/

void fadeOutAmbient(uint32_t duration){
  int maxLevel = 255;
  for (int level = maxLevel; level > 0; level--){
    pwm.setLevel(1, level);
    pwm.setLevel(2, level);
    pwm.send();
    delay(duration/maxLevel);
  }
  pwm.setLevel(0);
  pwm.send();
}


void fadeInAmbient(uint32_t duration){
  int maxLevel = 255;
  for (int level = 0; level < maxLevel; level++){
    pwm.setLevel(1, level); //blue
	pwm.setLevel(2, level); //green
    pwm.send();
    delay(duration/maxLevel);
  }

}

void fadeIn(int8_t feature, uint32_t duration){
  int maxLevel = 255;
  for (int level = 0; level < maxLevel; level++){
    pwm.setLevel(feature, level);
    pwm.send();
    delay(duration/maxLevel);
  }
}

void fadeOut(int8_t feature, uint32_t duration){
  int maxLevel = 255;
  for (int level = maxLevel; level > 0; level--){
    pwm.setLevel(feature, level);
    pwm.send();
    delay(duration/maxLevel);
  }
  pwm.setLevel(0);
  pwm.send();
}

void sendButtonPress(){
  StaticJsonBuffer<64> localBuffer;
  JsonObject& rPackage = localBuffer.createObject();
  rPackage["buttonPress"] = true;

  printJSON(rPackage);
}

void printJSON(JsonObject& obj) {
  String jsonStringified = String("");
  obj.printTo(jsonStringified);
  jsonStringified.concat("\r\n");

  Serial.write(jsonStringified.c_str());
}

//feature 0 == red
//feature 1 == blue
//feature 2 == green
//feature 3 == spotlights
//feature 4 == top left light box
//feature 5 == bot left light box
//feature 6 == bot right light box
//feature 7 == top right light box


void loop(){
  //send buttonPush notification
  bb.setLamp(0,HIGH);

  
  if (bb.getButton(button) && !buttonDown){
    sendButtonPress();
    buttonDown = true;   
  }

  if(!bb.getButton(button) && buttonDown) {
    buttonDown = false;
  }

  delay(10);

  //read Json and play Feature
  if (Serial.available() > 0){
    JsonObject& root = readJson();
    if (root.success()){
        Data package = parseJson(root);
        playFeature(package);
    }
  }

  delay(20);
}


