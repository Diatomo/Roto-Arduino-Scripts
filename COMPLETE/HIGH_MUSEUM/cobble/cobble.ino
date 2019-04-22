/*
 * Roto
 *
 * Project:
 *
 *
 * Exhibit:
 *
 *
 * Author(s):
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
#include <Wire.h>
#include <LIDARLite.h>
#include <alarmClock.h>
#include <outputExtend.h>

// Pin Assignments:
const byte outData = 4;
const byte outClock = 5;
const byte outLatch = 6;
const byte outCount = 2;



// Constants:
int distanceThreshhold = 300;

const int gameLength = 16;

// Global Variables:
int error =0;
unsigned int values[5] = {0,0,0,0,0};
boolean sensorState[5] = {0,0,0,0,0}; // 0 = no kid under sensor, 1 = kid under sensor.
boolean lightState[5] = {0,0,0,0,0}; // 0 = light off, 1 = light on.

int count1 = 0;
int count2 = 1;
byte gameArray[gameLength] = {4,2,1,3,0, 3,2,1,4,0, 1,0,4,3,2, 1};

void readSensors();
void game();
void updateLights();
// Global Classes:
repeatAlarm sensorTimer = repeatAlarm(readSensors);
repeatAlarm gameTimer = repeatAlarm(game);
repeatAlarm lightTimer = repeatAlarm(updateLights);

LIDARLite sensors;
outputExtend outputs = outputExtend(outData, outClock, outLatch, outCount);

void setup()
{
	Serial.begin(9600);
	Serial.println(versionString);
	Serial.print("File: ");
	Serial.println(__FILE__);

        Wire.begin();

        //startSensors();
	/* note: If the board has FETs populated, pins 4-7 must be driven high or low.
	 * If the FETs are used, remove the following block of code.
	 */

    Serial.println("Hello World");
    
    sensorTimer.setInterval(50);
    gameTimer.setInterval(50);
    lightTimer.setInterval(100);
}

void loop()
{
  sensorTimer.poll();
  gameTimer.poll();
  lightTimer.poll();
  
//  for (int i = 0; i < 5; i++)
//  {
//    lightState[i] = sensorState[i];
//  }
  
  Serial.print("SensorValues; ");
  for(int i = 0; i < 5; i++)
  {
    Serial.print(i);
    Serial.print(": ");
    Serial.print(values[i]);
    Serial.print(", ");
  }
  
  Serial.print("SensorStates; ");
  Serial.println();  for(int i = 0; i < 5; i++)
  {
    Serial.print(i);
    Serial.print(": ");
    Serial.print(sensorState[i]);
    Serial.print(", ");
  } Serial.print("LightStates; ");
  Serial.println();  for(int i = 0; i < 5; i++)
  {
    Serial.print(i);
    Serial.print(": ");
    Serial.print(lightState[i]);
    Serial.print(", ");
  }
  Serial.println();
  Serial.print("count: ");
  Serial.println(count1);
  Serial.print("gameArray: ");
  Serial.println(gameArray[count1]);
  Serial.print("sensorState: ");
  Serial.println(sensorState[gameArray[count1]]);
  Serial.print("count: ");
  Serial.println(count2);
  Serial.print("gameArray: ");
  Serial.println(gameArray[count2]);
  Serial.print("sensorState: ");
  Serial.println(sensorState[gameArray[count2]]);
}

void playAudio(byte audio)
{  
  outputs.extendedWrite(audio+8, 1); 
  delay(50);
  outputs.byteWrite(1,0x0);
}

void updateLights()
{
  for(int i =0; i<5; i++)
  {
    if(i == gameArray[count1] || i == gameArray[count2])
    {
      lightState[i] = 1;
    }
    else
    {
      lightState[i] = 0;
    }
  }
  
  for (int i = 0; i < 5; i++)
  {
    outputs.extendedWrite(i, lightState[i]);
  }
  for (int i = 5; i < 8; i++)
  {
    outputs.extendedWrite(i, 0);
    outputs.extendedWrite(i+8, 0);
  }
}

void game()
{
  
  if(sensorState[gameArray[count1]] == 1)
  {
    playAudio(gameArray[count1]);
    count1++;
    if (count1 == count2) count1++;
    if (count1 >= gameLength) count1 = 0;
  }  
  
  if(sensorState[gameArray[count2]] == 1)
  {
    playAudio(gameArray[count2]);
    count2++;
    if (count2 == count1) count2++;
    if (count2 >= gameLength) count2 = 0;
  }  
}

void readSensors()
{
  //Sensor 1
  Wire.beginTransmission(0x70);
  Wire.write(B00000001);
  error = Wire.endTransmission();
  Wire.beginTransmission(0x71);
  Wire.write(B00000000);
  error = Wire.endTransmission();
  values[0] = sensors.distance(false);
  
  //Sensor 2
  Wire.beginTransmission(0x70);
  Wire.write(B00000010);
  error = Wire.endTransmission();
  Wire.beginTransmission(0x71);
  Wire.write(B00000000);
  error = Wire.endTransmission();
  values[1] = sensors.distance(false);
  
  //Sensor 3
  Wire.beginTransmission(0x70);
  Wire.write(B00000100);
  error = Wire.endTransmission();
  Wire.beginTransmission(0x71);
  Wire.write(B00000000);
  error = Wire.endTransmission();
  values[2] = sensors.distance(false);
  
  //Sensor 4
  Wire.beginTransmission(0x70);
  Wire.write(B00001000);
  error = Wire.endTransmission();
  Wire.beginTransmission(0x71);
  Wire.write(B00000000);
  error = Wire.endTransmission();
  values[3] = sensors.distance(false);
  
  //Sensor 5
  Wire.beginTransmission(0x70);
  Wire.write(B00000000);
  error = Wire.endTransmission();
  Wire.beginTransmission(0x71);
  Wire.write(B00000001);
  error = Wire.endTransmission();
  values[4] = sensors.distance(false);
  
  updateSensorStates();
}

void updateSensorStates()
{
  for (int i = 0; i < 5; i++)
  {
    if (values[i] <= distanceThreshhold)
    {
      sensorState[i] = 1;
    }
    else
    {
      sensorState[i] = 0;
    }
  }
}

void startSensors()
{
  //Sensor 1
  Wire.beginTransmission(0x70);
  Wire.write(B00000001);
  error = Wire.endTransmission();
  Wire.beginTransmission(0x71);
  Wire.write(B00000000);
  error = Wire.endTransmission();
  sensors.begin(0, false);
  sensors.configure(3);
  Serial.println("Sensor 1 says 'Hello World!'");
  
  //Sensor 2
  Wire.beginTransmission(0x70);
  Wire.write(B00000010);
  error = Wire.endTransmission();
  Wire.beginTransmission(0x71);
  Wire.write(B00000000);
  error = Wire.endTransmission();
  sensors.begin(0, false);
  sensors.configure(3);
  Serial.println("Sensor 2 says 'Hello World!'");
  
  //Sensor 3
  Wire.beginTransmission(0x70);
  Wire.write(B00000100);
  error = Wire.endTransmission();
  Wire.beginTransmission(0x71);
  Wire.write(B00000000);
  error = Wire.endTransmission();
  sensors.begin(0, false);
  sensors.configure(3);
  Serial.println("Sensor 3 says 'Hello World!'");
  
  //Sensor 4
  Wire.beginTransmission(0x70);
  Wire.write(B00001000);
  error = Wire.endTransmission();
  Wire.beginTransmission(0x71);
  Wire.write(B00000000);
  error = Wire.endTransmission();
  sensors.begin(0, false);
  sensors.configure(3);
  Serial.println("Sensor 4 says 'Hello World!'");
  
//  //Sensor 5
//  Wire.beginTransmission(0x70);
//  Wire.write(B00000000);
//  error = Wire.endTransmission();
//  Wire.beginTransmission(0x71);
//  Wire.write(B00000001);
//  error = Wire.endTransmission();
//  sensors.begin(0, false);
//  sensors.configure(3);
//  Serial.println("Sensor 5 says 'Hello World!'");
}
