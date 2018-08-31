#include <alarmClock.h>

const int bikes[] = {4,5,6};
const float teeth[] = {12, 16, 12};

const unsigned long checkTime = 1;

// variables

boolean state[] = {0,0,0};

void getRPM();
void Print();

float counter[] = {0,0,0};
float RPM[] = {0,0,0};
int repeated = 0;

// alarms
repeatAlarm rpm = repeatAlarm(getRPM);
repeatAlarm serial = repeatAlarm(Print);

void setup(){
  Serial.begin(9600);
  for(int i=0; i<3; i++){
    pinMode(bikes[i], INPUT);
  }
  
  rpm.setInterval(checkTime);
  serial.setInterval(1000);
}

void loop(){
  rpm.poll();
//  serial.poll();
}

void getRPM(){
  for(int i=0; i<3; i++){
    if(digitalRead(bikes[i]) == LOW && state[i] == 0){
      counter[i]++;
      state[i] = 1;
    }
    if(digitalRead(bikes[i]) == HIGH && state[i] == 1){
      state[i] = 0;
    }
  }
  repeated++;
  if(repeated == 500){
    for(int i=0; i<3; i++){ 
      float temp = counter[i]/teeth[i];     
      RPM[i] = temp * 120;
      counter[i] = 0;
    }
    Serial.print("Bike1: ");
  Serial.println(RPM[0]);
  Serial.print("HandBike: ");
  Serial.println(RPM[1]);
  Serial.print("Bike2: ");
  Serial.println(RPM[2]);
    repeated = 0;
  }
}

void Print(){
  Serial.print("Bike1: ");
  Serial.println(RPM[0]);
  Serial.print("HandBike: ");
  Serial.println(RPM[1]);
  Serial.print("Bike2: ");
  Serial.println(RPM[2]);
}

