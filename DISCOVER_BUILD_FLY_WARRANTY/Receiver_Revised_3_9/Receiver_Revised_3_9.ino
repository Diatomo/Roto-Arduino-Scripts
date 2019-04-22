#include <Servo.h> 
#include <ArduinoJson.h>
#include <alarmClock.h>


//PIN CONFIGURATION
int servoPin = 3;
int motorPin1 = 8;
int motorPin2 = 9;
int motorPin3 = 10;
int motorPin4 = 11;

//SERVO OBJECTS
Servo servo;
Servo motor1; 
Servo motor2; 
Servo motor3; 
Servo motor4;

//movement VARS
byte angle = 88;
byte motors = 0;

//read-in values
unsigned char START_BYTE = 0x53;
unsigned char averageX = 0;
unsigned char averageY = 0;
unsigned char checksumHigh = 0;
unsigned char checksumLow = 0;
int calculatedChecksum;

//startbyte
boolean syncByteFound = 0;
boolean signalLost;

//Alarm Clocks
int signalLostTimeout = 1000;
int timeoutShutdown = 5000;
uint32_t timer;

void signalLostHandler();

alarmClock signalLostTimer = alarmClock(signalLostHandler);

void setupMotors(){

	servo.attach(servoPin);
	motor1.attach(motorPin1);
	motor2.attach(motorPin2);
	motor3.attach(motorPin3);
	motor4.attach(motorPin4);

	delay(2000);
	
	motor1.write(motors);
	motor2.write(motors);
	motor3.write(motors);
	motor4.write(motors);
	servo.write(angle);
}

void setup() {
	Serial.begin(9600);
	Serial.println("Initializing");
	setupMotors();
	signalLostTimer.setAlarm(signalLostTimeout);
}

JsonObject& readJson(){
	StaticJsonBuffer<160> jsonBuffer;	//heap
	JsonObject& root = jsonBuffer.parseObject(Serial);
	/*IF DEBUG*/
	//echo(root);
	return root;
}


void parseJson(JsonObject& root){
    angle = root["angle"];
    motors = root["motors"];
    checksumHigh = root["checksumHigh"];
    checksumLow = root["checksumLow"];
}

void echo(JsonObject& package){
		package.prettyPrintTo(Serial);
}

void loop() {

/*UNESSECARY, as we have a checksum that will determine accurate information sent
  unsigned char rxByte = 0;

  if (syncByteFound == 0) {
    rxByte = Serial.read();
    if (rxByte == 0x53)
      syncByteFound = 1;
  }
*/

  if (Serial.available() > 3) {
	parseJson(readJson());

  /*OLD ASSIGNMENTS
    angle = Serial.read();
    motors = Serial.read();
    checksumHigh = Serial.read();
    checksumLow = Serial.read();
  */
    signalLost = false;
    timer = millis() + timeoutShutdown;
    calculatedChecksum = angle + motors;
    signalLostTimer.setAlarm(signalLostTimeout);

    if (calculatedChecksum == (checksumLow << 8) | checksumHigh) {
      Serial.println("[Checksum Passed]");
      if (angle < 97 && angle > 77) {
        servo.write(angle);
      }
      Serial.print("angle value:  ");
      Serial.println(angle);
      if (motors < 99) {
        motor1.write(motors);
        motor2.write(motors);
        motor3.write(motors);
        motor4.write(motors);
      }
      else {
       motors = 99;
       motor1.write(motors);
       motor2.write(motors);
       motor3.write(motors);
       motor4.write(motors);
      }
      Serial.print("motor value:  ");
      Serial.println(motors);
      //delay(25);
    }

    else{
      Serial.println("[Checksum FAILED]");
		syncByteFound = 0;
  	}

  signalLostTimer.poll();
  if (signalLost && millis() > timer) {
    //signalLost = true;
    angle = 86; //random angle value??
    motors = 15; //random motor value??
    motor1.write(motors);
    motor2.write(motors);
    motor3.write(motors);
    motor4.write(motors);
    servo.write(angle);
  }
}



void signalLostHandler() {
  Serial.println("Signal LOST! Bringing System Offline");
  signalLost = true;
  angle = 88; //random angle value??
  motors = 100;//random motor value??
  motor1.write(motors);
  motor2.write(motors);
  motor3.write(motors);
  motor4.write(motors);
  servo.write(angle);
}

