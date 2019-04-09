/*
 * Author: Milien McDermott
 * Date:4/09/2019
 * IDE V1.6.9
 * Email:mcdermottm3446@my.uwstout.edu
 * Function:Runs Motorized Interactive Cat Excersizer
 */
const int TRIGPIN[3] = {2,4,6};
const int ECHOPIN[3] = {3,5,7};


//first 3: motor 1, second 3: motor 2
const int MOTORPIN[6] = {6,5,8,7,1,4};
// enA,enB   in1,in2,   ,in3,in4
const bool FORWARD = true;
const bool REVERSE = false;
const int FAST = 200;
const int SLOW = 50;


const int MOVEPADDING = 10;
float cm;
bool exitProcess;
const int EATDELAY = 5000;
const int RETRIEVALDELAY = 10000;


void setup(){
  //setup serial monitor for debugging at 9600 baud rate
  Serial.begin(9600);
  //setup untrasonic sensors
  say("setup ultrasonic sensors");
  for(int i=0;i<3;i++){
    pinMode(TRIGPIN[i], OUTPUT);
    pinMode(ECHOPIN[i], INPUT);  
  }

  //set up all the motor control pins
  for(int i=0;i<6;i++){
    pinMode(MOTORPIN[i],OUTPUT);
  }

  
  //initial delay
  say("Initial setup delay");
  delay(5000);
}


void sentry(){
  //get initial distances
  int initial[6];
  int checkSafe[6];
  //first three
  for(int i=0;i<3;i++){
    initial[i]=getdistance(TRIGPIN[i],ECHOPIN[i]);
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



bool flee(){
  bool caught = false;
  //TODO Timer
  caught = runaway();
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

bool checkForCat(int waitTime){
  for(int i = 0; i < waitTime; i = i+300){
    delay(300);
    //TODO check for RFID cat hit somehow
  }
}

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


void say(String said){
  Serial.println(said);
}

void loop(){
  say("Entering sentry mode");
  sentry();
  say("Exited sentry mode");
  flee();
}
