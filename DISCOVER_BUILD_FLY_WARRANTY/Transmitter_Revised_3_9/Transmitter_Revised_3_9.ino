#include <alarmClock.h>
#include <ArduinoJson.h>


/*pins assignments*/
uint8_t const analogY 16;
uint8_t const analogX 17;
uint8_t const trimPot 14;

unsigned char START_BYTE = 0x53;

unsigned char motorValue = 0;
unsigned char servoValue = 0;
unsigned char checksumHigh = 0;
unsigned char checksumLow = 0;

int calculatedChecksum = 0;
int calculatedChecksumTest = 0;
uint32_t sumX;
uint32_t sumY;

int counter;
int averageX;
int averageY;
int angle;
int motors;
int meanCounter = 500;
int deltaMotors;

int lastPosX;
int lastPosY;
int lastMotor;

boolean sleep = true;

byte MOTOR_MAX;
byte MOTOR_MIN = 35;

byte ANGLE_MIN = 78;
byte ANGLE_MAX = 96;
byte ANGLE_HOME = (ANGLE_MIN + ANGLE_MAX) / 2;
int walkawayTimeout = 5000;

/*ALARM CLOCK*/
void walkawayHandler();
void activeCheck();

alarmClock walkawayTimer = alarmClock(walkawayHandler);
repeatAlarm activeUser = repeatAlarm(activeCheck);

/*JSON DATA PAYLOAD*/
struct data{
	char dSTART_BYTE = 0x53;
	int dAngle;
	int dMotors;
	int dchecksumHigh;
	int dchecksumLow;
};
typedef struct data Data;

void setupPins(){
  pinMode(trimPot, INPUT);
  pinMode(analogX, INPUT);
  pinMode(analogY, INPUT);
}

void setup() {
  activeUser.setInterval(200);
  MOTOR_MAX = map(analogRead(trimPot), 0, 1023, MOTOR_MIN, 99); //99 I guess is ideally the max the motors can go without the trim pot
  setupPins();
  Serial.begin(9600);
  resetExhibit();
}

void constrainServerAngle(){
    // Constrain the Angle to be within the bounds of our Servo
    angle = map(averageX, 72, 814, 0, 180);
	angle = constrain(angle, ANGLE_MIN, ANGLE_MAX); //ccs added this.
    if (angle < 100 && angle > 85) {
      angle = ANGLE_HOME;
    }
    else if (angle >= 100) {
      angle = map(angle, 100, 180, ANGLE_HOME, ANGLE_MAX);
      angle = constrain(angle, ANGLE_HOME, ANGLE_MAX);
    }
    else if (angle <= 85) {
      angle = map(angle, 85, 0, ANGLE_HOME, ANGLE_MIN);
      angle = constrain(angle, ANGLE_MIN, ANGLE_HOME);
    }
}

void constrainMotorThrottle(){
    // Constrain the throttle to be within the bounds of our Motors
    motors = map(averageY, 38, 800, MOTOR_MIN, MOTOR_MAX);
    motors = constrain(motors, MOTOR_MIN, MOTOR_MAX);
}

void sampleJoystick(){

	//sample
	while (counter < meanCounter){
		sumX += analogRead(analogX);
		sumY += analogRead(analogY);
		counter++;
	}

	//update
    lastPosX = sumX / meanCounter;
    lastPosY = sumY / meanCounter;
    averageX = sumX / meanCounter;
    averageY = sumY / meanCounter;
	
	//reset
	sumX = 0;
	sumY = 0;
	counter = 0;
}

void pollClocks(){
	activeUser.poll();
	walkawayTimer.poll();
}


void loop() {
	MOTOR_MAX = map(analogRead(trimPot), 0, 1023, 0, 99);
	pollClocks();
	sampleJoystick();
	constrainServerAngle();
	constrainMotorThrottle();

	if (sleep) {
		update(ANGLE_HOME, 15);
	}

	else{

		//relationship between angle and motor velocity?
		if (angle == 78 || angle == 90){
			if (motors > 60){
	  			motors -= 10;
			}
		}

	//throttles growth
		if (motors - lastMotor > 20){
			motors = lastMotor + 10;
			//caps maximum
			if (motors > MOTOR_MAX){
	  			motors = MOTOR_MAX;
		}
	}
	update(angle, motors);
	lastMotor = motors;

	}
}




//=================================================================================
//								STATE CHANGES
//=================================================================================

void resetExhibit() {
  angle = ANGLE_HOME;
  motors = 0;
  sleep = true;
}

void activeCheck() {
  if (lastPosY > 500 || lastPosY < 300  || lastPosX  < 365 || lastPosX > 525) {
    walkawayTimer.setAlarm(walkawayTimeout);
    sleep = false;
  }
}

//=================================================================================
//								COMMUNICATION
//=================================================================================

/*JSON DATA AGGREGATION*/
Data parseJson(int angle, int motors){

	/*preliminary calculations to get ready for packing*/
	calculatedChecksum =  angle + averageY;
	checksumHigh = calculatedChecksum & 0xFF;
	checksumLow = calculatedChecksum >> 8;
	calculatedChecksumTest = (checksumLow << 8) | checksumHigh;
	
	/*package the data*/
	Data package;
	package.dSTART_BYTE = 0x53;
	package.dAngle;
	package.dMotors;
	package.dchecksumHigh;
	package.dchecksumLow;
	return package;
}


void sendData(Data package){
	StaticJsonBuffer<160> localBuffer;
	JsonObject& deliver = localBuffer.createObject();
	deliver["START_BYTE"] = package.dSTART_BYTE;
	deliver["angle"] = package.dAngle;
	deliver["motors"] = package.dMotors;
	deliver["checksumHigh"] = package.dchecksumHigh;
	deliver["checksumLow"] = package.dchecksumLow;
	deliver.printTo(Serial);
	/*if need a debug*/
	//echo(deliver);
}

void echo(JsonObject& package){
		package.prettyPrintTo(Serial);
}

void update(int angle, int motors) {
	Data package; //constructor
	package = parseJson(angle,motors);//populate
	sendData(package);
}



/*	OLD CODE 

  calculatedChecksum =  angle + averageY;
  checksumHigh = calculatedChecksum & 0xFF;
  checksumLow = calculatedChecksum >> 8;
  calculatedChecksumTest = (checksumLow << 8) | checksumHigh;
 

  Serial.write(START_BYTE);
  Serial.write(angle);
  Serial.write(motors);
  Serial.write(checksumHigh);
  Serial.write(checksumLow);

*/

