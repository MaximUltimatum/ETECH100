/*
 * Author: Milien McDermott
 * Date:4/09/2019
 * IDE V1.6.9
 * Email:mcdermottm3446@my.uwstout.edu
 * Function:Runs Motorized Interactive Cat Excersizer
 */
//include RFID stuff
#include <AddicoreRFID.h>
#include <SPI.h>
AddicoreRFID catRFID;
uchar fifobytes;
uchar fifoValue;
const int chipSelectPin = 10
const int NRSTPD = 9;
#define MAX_LEN = 16

//Echo stuff
const int trig = 3;
const int echo = 2;
#define EchoInputCount 2
#define TriggerPin  2
// echo pin will be interrupt 1 on pin 3
#define DelayBetweenPings 50 // it works to about 5 milliseconds between pings
volatile  unsigned long PingTime[EchoInputCount];
volatile int Counter = EchoInputCount;
volatile  unsigned long edgeTime;
volatile  uint8_t PCintLast;
int PinMask = B1000; // pin 3
float Measurements[EchoInputCount];
unsigned long TimeoutTimer;
int 

//Motor stuff
const int MOTORPIN[6] = {6,5,8,7,1,4};
// enA,enB   in1,in2,   ,in3,in4
const bool FORWARD = true;
const bool REVERSE = false;
const int FAST = 200;
const int SLOW = 50;

//Various variables
const int MOVEPADDING = 10;
float cm;
bool exitProcess;
const int EATDELAY = 5000;
const int RETRIEVALDELAY = 10000;

//SETUP CODE
void setup(){
  //setup serial monitor for debugging at 9600 baud rate
  Serial.begin(9600);
  //setup ultrasonic sensors
  say("setup ultrasonic sensors");
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT); 

  //set up all the motor control pins
  say("Set up motor control pins");
  for(int i=0;i<6;i++){
    pinMode(MOTORPIN[i],OUTPUT);
  }

  //Start RFID stuff
  say("Setup RFID");
  SPI.begin();
  pinMode(chipSelectPin,OUTPUT);
  digitalWrite(chipSelectPin, LOW);         // Activate the RFID reader
  pinMode(NRSTPD,OUTPUT);                     // Set digital pin 10 , Not Reset and Power-down
  digitalWrite(NRSTPD, HIGH);
  catRFID.AddicoreRFID_Init();
  
  //initial delay
  say("Initial setup delay");
  delay(5000);
}

//Thinking Operation code
void sentry(){
  //get initial distances
  int initial[6];
  int checkSafe[6];
  //first three
  for(int i=0;i<3;i++){
    initial[i]=getdistance();
  }
  //TODO move motors
  //second three (after turn)
  for(int i=3;i<6;i++){
    initial[i]=getdistance(TRIGPIN[i],ECHOPIN[i]);
  }
  for(int i=0;i<6;i++){
    checkSafe[6]=initial[6];
  }
  //sentry loop
  bool safe = true;
  while(safe){
    for(int i=0;i<3;i++){
      checkSafe[i]=getdistance(TRIGPIN[i],ECHOPIN[i]);
    }
    for(int i=0;i<3;i++){
      if(checkSafe[i]>((initial[i]+MOVEPADDING)||checkSafe[i]<(initial[i]-MOVEPADDING))){
        safe=false;
        break;
      }
    }
    //TODO move motors
    //second three (after turn)
    for(int i=3;i<6;i++){
      checkSafe[i]=getdistance(TRIGPIN[i],ECHOPIN[i]);
    }
    for(int i=3;i<6;i++){
      if(checkSafe[i]>((initial[i]+MOVEPADDING)||checkSafe[i]<(initial[i]-MOVEPADDING))){
        safe=false;
        break;
      }
    }
  }
}

//returns distance from ultrsonic sensors
int getdistance(){
  PingIt(); // Manage ping data

  if ( ((unsigned long)(millis() - TimeoutTimer) >= 1000)) {
    PingTrigger(TriggerPin); // Send another ping
    Counter = 0;
    TimeoutTimer = millis();
  }
  return Measurements;
}

bool flee(){
  bool caught = false;
  //TODO Timer
  caught = runaway();
  stopmotors();
  return caught;
}

bool runaway(){
  //TODO code to interface with RFID chip
  bool RFIDhit = false;
  int timeout = 0;
  
  //drives in a square and checks for the cat
  while(!RFIDhit && (timeout<20000)){
    setmotors(FORWARD,FORWARD,FAST);
    RFIDhit = checkForCat(5000);
    timeout = timeout + 5000;
    if(RFIDhit)
      return true;
    setmotors(REVERSE,FORWARD,SLOW);
    RFIDhit = checkForCat(1000);
    timeout = timeout + 1000;
    if(RFIDhit)
      return true;
  }
  return false;
}

//use this function to idle while motor runs
bool checkForCat(int waitTime){
  for(int i = 0; i < waitTime; i = i+300){
    delay(300);
    if(wastouched()){ 
      return true;
    }
  }
  return false;
}

//this function sets motor values
void setmotors(bool left, bool right,int spd){ //spd is 0-255)
  //pass these into the motor
  if(right){
    digitalWrite(MOTORPIN[2],HIGH);
    digitalWrite(MOTORPIN[3],LOW);
  }
  else{
    digitalWrite(MOTORPIN[2],LOW);
    digitalWrite(MOTORPIN[3],HIGH);
  }

  if(left){
    digitalWrite(MOTORPIN[4],HIGH);
    digitalWrite(MOTORPIN[5],LOW);
  }
  else{
    digitalWrite(MOTORPIN[4],LOW);
    digitalWrite(MOTORPIN[5],HIGH);
  }

  analogWrite(MOTORPIN[0],spd);
  analogWrite(MOTORPIN[1],spd);
}

void stopmotors(){
  for(int i=2;i<6;i++){
    digitalWrite(MOTORPIN[i],LOW);
  }
}

bool wastouched(){
  uchar status;
  uchar str[MAX_LEN];
  str[1] = 0x4400;

  status = catRFID.AddicoreRFID_Request(PICC_REQIDL, str);
  if(status == MI_OK){
    say("Cat got to it");
    return true;
  }
  else{
    say("No cat detected");
    catRFID.AddicoreRFID_Halt();
    return false;
  }
}


//SONAR CODE
void PintTimer( )
{
  uint8_t pin;
  static unsigned long cTime;
  cTime = micros();         // micros() return a uint32_t
  pin = PIND >> 3 & 1;      // Quickly get the state of  pin 3
  if (pin)edgeTime = cTime; //Pulse went HIGH store the start time
  else { // Pulse Went low calculate the duratoin
    PingTime[Counter % EchoInputCount] = cTime - edgeTime; // Calculate the change in time  NOTE: the "% EchoInputCount" prevents the count from overflowing the array look up % remainder calculation
    Counter++;
  }
}
void debug()
{
  char S[20];
  static unsigned long PingTimer;
  if ((unsigned long)(millis() - PingTimer) >= 1) {
    PingTimer = millis();
    for (int c = 0; c < EchoInputCount; c++) {
      Serial.print(dtostrf(Measurements[c], 6, 1, S));
    }
    Serial.println();
  }
}

float microsecondsToInches(long microseconds)
{
  return (float) microseconds / 74 / 2;
}

float microsecondsToCentimeters(long microseconds)
{
  return (float)microseconds / 29 / 2;
}

void PingTrigger(int Pin)
{

  digitalWrite(Pin, LOW);
  delayMicroseconds(1);
  digitalWrite(Pin, HIGH); // Trigger another pulse
  delayMicroseconds(10);
  digitalWrite(Pin, LOW);
}

void PingIt()
{
  unsigned long PT[EchoInputCount];
  static unsigned long PingTimer;
  if (Counter >= EchoInputCount) {
    if ( ((unsigned long)(millis() - PingTimer) >= DelayBetweenPings)) {
      PingTimer = millis();
      cli ();         // clear interrupts flag
      for (int c = 0; c < EchoInputCount; c++) {
        PT[c] = PingTime[c];
      }
      sei ();         // set interrupts flag
      for (int c = 0; c < EchoInputCount; c++) {
        if (PT[c] < 23200) Measurements[c] = (float) (microsecondsToCentimeters(PT[c]));
      }
      //      Measurements = (float) (microsecondsToInches(PT));
      debug();
      delay(10);
      PingTrigger(TriggerPin); // Send another ping
      Counter = 0;
      TimeoutTimer = millis();

    }
  }
}

//printout shortcut
void say(String said){
  Serial.println(said);
}

void loop(){
  say("Entering sentry mode");
  sentry();
  say("Exited sentry mode");
  if(flee()){ //returns true if the cat catches it
    delay(EATDELAY);
  }
  else{
    delay(RETRIEVALDELAY);
  }
  say("Finished loop") 
}
