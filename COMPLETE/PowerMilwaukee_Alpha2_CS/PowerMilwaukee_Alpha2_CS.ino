#include <Wire.h>
#include <alarmClock.h>
#include <ADS1x15.h>
#include <smooth.h>
#include <buttonBoard.h>

// Constants:
const byte bbCount = 1;
const byte runThreshHold = 150; // Change loadcell sensitivity
//const unsigned long debounceTime = 100; // Change the debounce on the button press

// Global Variables:
word previousLoad = 0;
word loadValue = 0;
word currentLoad = 0;
//int activeBTN[] = {0, 0, 0, 0};
byte activeBTN = 0;
String fromPC;

unsigned long peakTime = 0;
unsigned long previousPeakTime = 0;
unsigned long peakToPeak = 0;

boolean buttonSwitched = false;
boolean runDirection = true;

// Pin Assignments:
const byte bbDI = 8;
const byte bbDO = 9;
const byte bbCLK = 10;
const byte bbILT = 11;
const byte bbOLT = 12;
const byte offest = 0;
const byte total = 4;

// Global Classes:
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, bbCount);
buttonSelect select = buttonSelect(&bb,offest,total,true);
ADS1115 adc;
smooth aiFilter = smooth(3);
smooth runPeriod = smooth(3);


void start_adc() {
  while (!Serial);
  Serial.println("Starting...");
  adc.attachTimeoutHandler([]() {
    Serial.println("Timeout");
  });
  adc.attachErrorHandler([](uint8_t n) {
    Serial.print("Error: ");
    Serial.println(n);
  });
  delay(100);
  adc.begin();
  adc.setGain(GAIN_8);
  Wire.setClock(400000);
}

void getLoad()
{
  loadValue = adc.analogRead(0);
  loadValue = (loadValue > 0) ? (loadValue) : 0;
  aiFilter.smoothData(loadValue);
}

void getSPF()
{
  currentLoad = aiFilter.smoothedData;
  if (runDirection == true) 
  {
    if (currentLoad > previousLoad)
    {
      previousLoad = currentLoad;
    }
    else if (currentLoad < previousLoad - runThreshHold)
    {
      runDirection = false;
//      Serial.println(0); Not needed anymore
        Serial.println("s");
        
      previousLoad = currentLoad;
      previousPeakTime = peakTime;
    }
  }
  else if (runDirection == false)
  {
    if (currentLoad < previousLoad)
    {
      previousLoad = currentLoad;
    }
    else if (currentLoad >= previousLoad + runThreshHold)
    {
      runDirection = true;
      
      peakTime = millis();
      peakToPeak = (peakTime - previousPeakTime);
      runPeriod.smoothData(peakToPeak);
//      runCount++;
//      Serial.println("s");
        
      previousLoad = currentLoad;
    }
  }
}

// Alarms
repeatAlarm aiTimer = repeatAlarm(getLoad);
repeatAlarm SPFTimer = repeatAlarm(getSPF);
//repeatAlarm BTN_LED = repeatAlarm(LED);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(500);
  
  start_adc();

  aiTimer.setInterval(10);
  SPFTimer.setInterval(10);

  bb.setLamp(HIGH);
  delay(200);
  bb.setLamp(LOW);
  delay(200);
  bb.setLamp(HIGH);
  delay(200);

  for(int i = 0; i<10; i++)
  {
    getLoad();
    delay(10);
  }
}

void loop() {
  // put your main code here, to run repeatedly: 
  SPFTimer.poll();
  aiTimer.poll();
  select.poll();
  if(select.event() == true){
    activeBTN = select.getState();
    Serial.println(activeBTN);
  }
  if(Serial.available()){
    fromPC = Serial.readString();
    if(fromPC == "t"){
      select.setState(buttonReset);
    }
  }
}
