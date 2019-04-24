#include <Keyboard.h>

 /*
 * Author: Milien McDermott
 * Date:4/09/2019
 * IDE V1.6.9
 * Email:mcdermottm3446@my.uwstout.edu
 * Function:Runs Motorized Interactive Cat Excersizer
 */
//include RFID necessities
#include "AddicoreRFID.h"
#include <SPI.h>
#define uchar unsigned char
#define uint unsigned int
uchar fifobytes;
uchar fifoValue;
const int chipSelectPin = 10;
const int NRSTPD = 9;
#define MAX_LEN 16
AddicoreRFID catRFID;

//Echo stuff
const int TRIGPIN[3] = {2,4,6};
const int ECHOPIN[3] = {3,5,7};


//Motor stuff
const int MOTORPIN[6] = {6,5,8,7,1,4};
// enA,enB   in1,in2,   ,in3,in4
const bool FORWARD = true;
const bool REVERSE = false;
const int FAST = 200;
const int SLOW = 100;

//Various variables
const int MOVEPADDING = 10;
float cm;
bool exitProcess;
const int EATDELAY = 5000;
const int RETRIEVALDELAY = 10000;


//SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP
void setup(){

  //setup serial monitor for debugging at 9600 baud rate
  Serial.begin(9600);

  //setup ultrasonic sensors
  say("setup ultrasonic sensors");
  for(int i=0;i<3;i++){
    pinMode(TRIGPIN[i], OUTPUT);
    pinMode(ECHOPIN[i], INPUT);
  }

  //set up all the motor control pins
  say("Set up motor control pins");
  for(int i=0;i<6;i++){
    pinMode(MOTORPIN[i],OUTPUT);
  }
  stopmotors();

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
  delay(1000);
}


//THNKING OPERATION CODE THNKING OPERATION CODE THNKING OPERATION CODE THNKING OPERATION CODE THNKING OPERATION CODE THNKING OPERATION CODE THNKING OPERATION CODE THNKING OPERATION CODE
void sentry(){
  //get initial distances
  say("Get initial distances");
  int initial[6];
  int checkSafe[6];
  //first three
  for(int i=0;i<3;i++){
    initial[i]=getdistance(TRIGPIN[i],ECHOPIN[i]);
  }
  say("Done with first three");
  movemotors(FORWARD,REVERSE,SLOW,200);
  //second three (after turn)
  for(int i=3;i<6;i++){
    initial[i]=getdistance(TRIGPIN[i-3],ECHOPIN[i-3]);
  }
  for(int i=0;i<6;i++){
    checkSafe[i]=initial[i];
  }
  //sentry loop
  say("Entering sentry loop");
  bool safe = true;
  while(safe){
    say("In sentry loop");
    for(int i=0;i<3;i++){
      initial[i]=getdistance(TRIGPIN[i],ECHOPIN[i]);
    }
    for(int i=0;i<3;i++){
      if(checkSafe[i]>((initial[i]+MOVEPADDING)||checkSafe[i]<(initial[i]-MOVEPADDING))){
        safe=false;
        return;
      }
    }
    movemotors(REVERSE,FORWARD,SLOW,200);
    //second three (after turn)
    for(int i=3;i<6;i++){
      initial[i]=getdistance(TRIGPIN[i-3],ECHOPIN[i-3]);
    }
    for(int i=3;i<6;i++){
      if(checkSafe[i]>((initial[i]+MOVEPADDING)||checkSafe[i]<(initial[i]-MOVEPADDING))){
        safe=false;
        return;
      }
    }
  }
}

bool flee(){
  bool caught = false;
  caught = runaway();
  stopmotors();
  return caught;
}

bool runaway(){
  //TODO code to interface with RFID chip
  bool RFIDhit = false;
  int timeout = 0;
  int straitTime = 5000;
  int turnTime = 1000;
  //drives in a square and checks for the cat
  while(!RFIDhit && (timeout<20000)){
    setmotors(FORWARD,FORWARD,FAST);
    RFIDhit = checkForCat(straitTime);
    timeout = timeout + straitTime;
    if(RFIDhit)
      return true;
    setmotors(REVERSE,FORWARD,SLOW);
    RFIDhit = checkForCat(turnTime);
    timeout = timeout + turnTime;
    if(RFIDhit)
      return true;
  }
  return false;
}



//MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS MOTORS
//Call this function if you're just moving and not checking for a cat
void movemotors(bool left, bool right, int spd, int waitTime){
  setmotors(left, right, spd);
  for (int i = 0; i < waitTime; i = i + 200){
    delay(200);
  }
  stopmotors();
}

//use this function to idle while motor runs and you wait for a cat
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
  if(left){
    say("Left called forward");
    digitalWrite(MOTORPIN[2],HIGH);
    digitalWrite(MOTORPIN[3],LOW);
  }
  else{
    say("Left called back");
    digitalWrite(MOTORPIN[2],LOW);
    digitalWrite(MOTORPIN[3],HIGH);
  }

  if(right){
    say("Right called forward");
    digitalWrite(MOTORPIN[4],HIGH);
    digitalWrite(MOTORPIN[5],LOW);
  }
  else{
    say("Right called Back");
    digitalWrite(MOTORPIN[4],LOW);
    digitalWrite(MOTORPIN[5],HIGH);
  }

  analogWrite(MOTORPIN[0],spd);
  analogWrite(MOTORPIN[1],spd);
}

void stopmotors(){
  say("Stopping motors");
  for(int i=2;i<6;i++){
    digitalWrite(MOTORPIN[i],LOW);
  }
  analogWrite(MOTORPIN[0],0);
  analogWrite(MOTORPIN[1],0);
}



//SENSORS SENSORS SENSORS SENSORS SENSORS SENSORS SENSORS SENSORS SENSORS SENSORS SENSORS SENSORS SENSORS SENSORS SENSORS SENSORS SENSORS SENSORS
//returns distance from ultrsonic sensors
int getdistance(int Trigger, int Echo){
  digitalWrite(Trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger, LOW);
  cm = pulseIn(Echo, HIGH) / 58.0; //The echo time is converted into cm
  cm = (int(cm * 100.0)) / 100.0;
  return cm;
}

//checks if RFID tripped
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


//SAY FUNCTION
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
  say("Finished loop");
}
