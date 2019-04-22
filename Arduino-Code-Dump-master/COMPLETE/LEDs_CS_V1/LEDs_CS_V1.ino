/*
   Roto

 Project: Discovery Worl
 Exhibit: Power Milwaukee
 Author(s): Caleb Shuey
 Date: 7/06/2018

 Original environment:
 Arduino 1.0.6

 Revision notes:

 */


/* ====================================================*/
/* =========    SCRIPT CONFIGURATION   ================*/
/* ====================================================*/

#include <Wire.h>
#include <pwmBoard.h>
#include <ArduinoJson.h>
#include <alarmClock.h>

#define versionString "Roto - [Discovery World] [Power Milwaukee]"

//Constants
const int red = 0;
const int green = 1;
const int blue = 2;
const int duration = 2000;

//Variables
int changeR = 0;
int changeG = 0;
int changeB = 0;

int currentR = 0;
int currentG = 0;
int currentB = 0;

int diffR = 0;
int diffG = 0;
int diffB = 0;

int attR = 0;
int attG = 0;
int attB = 0;


unsigned long fadeTime = 0;
unsigned long timer = 0;

boolean attraction = true;

//PWM
pwmBoard pwm = pwmBoard(00);

//Data Collection
struct data{
  int8_t attract;
  uint32_t r;
  uint32_t g;
  uint32_t b;
};
typedef struct data Data;

void attract(){
  if(attraction == true){
    attR = random(0, 255);
    attG = random(0, 255);
    attB = random(0, 255);
    pwm.setLevel(red, attR);
    pwm.setLevel(green, attG);
    pwm.setLevel(blue, attB);
    pwm.send();
  }
}

//Alarms
repeatAlarm attractor = repeatAlarm(attract);

void setup(){
   Serial.begin(9600); 
   Serial.println("Testing");
   pwm.start();
   attractor.setInterval(500);
   pwm.setLevel(red, 0);
   pwm.setLevel(blue, 255);
   pwm.setLevel(green, 255);
   pwm.send();
   delay(1000);
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
  package.attract = root["attract"];
  package.r = root["r"];
  package.g = root["g"];
  package.b = root["b"];
  printJSON(root);
  return package;
}

void printJSON(JsonObject& obj) {
  String jsonStringified = String("");
  obj.printTo(jsonStringified);
  jsonStringified.concat("\r\n");

//  Serial.write(jsonStringified.c_str());
}

void echo(JsonObject& package){
    package.prettyPrintTo(Serial);
}

void playFeature(Data package){
  if(package.attract == 1){
    attraction = true;
  }
  if(package.attract == 0){
    attraction = false;
    pwm.setLevel(red, package.r);
    pwm.setLevel(green, package.g);
    pwm.setLevel(blue, package.b);
    pwm.send();
//    changeR = currentR - package.r;
//    changeG = currentG - package.g;
//    changeB = currentB - package.b;
//    
//    changeR = abs(changeR);
//    changeG = abs(changeG);
//    changeB = abs(changeB);
//    
//    if(changeR != 0){
//      diffR = changeR/duration;
//    }
//    if(changeG != 0){
//      diffG = changeG/duration;
//    }
//    if(changeB != 0){
//      diffB = changeB/duration;
//    }
//    fadeTime =  millis()+duration;
//    timer = millis();
//    
//    //Sets all of the RGB values over 2 seconds
//    while(timer < fadeTime){ 
//     if(currentR > package.r){
//       pwm.setLevel(red, currentR);
//       currentR = currentR - diffR;
//     } 
//     if(currentR < package.r){
//       pwm.setLevel(red, currentR);
//       currentR = currentR + diffR;
//     }
//     
//     if(currentG > package.g){
//       pwm.setLevel(green, currentG);
//       currentG = currentG - diffG;
//     } 
//     if(currentG < package.g){
//       pwm.setLevel(green, currentG);
//       currentG = currentG + diffG;
//     }
//     
//     if(currentB > package.b){
//       pwm.setLevel(blue, currentB);
//       currentB = currentB - diffB;
//     } 
//     if(currentB < package.b){
//       pwm.setLevel(blue, currentB);
//       currentB = currentB + diffB;
//     }
//     pwm.send();
//     delay(1);
//     timer = millis();   
//    }
  }
}

void loop(){
 attractor.poll();
 if(Serial.available() > 0){
  JsonObject& root = readJson();
   if(root.success()){
     Data package = parseJson(root);
     playFeature(package);
    }
  }
}

//void attract(){
//  if(attraction == true){
//    attR = random(0, 255);
//    attG = random(0, 255);
//    attB = random(0, 255);
//    pwm.setLevel(red, attR);
//    pwm.setLevel(green, attG);
//    pwm.setLevel(blue, attB);
//    pwm.send();
//  }
//}
