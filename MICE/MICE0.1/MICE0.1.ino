/*
 * Author: SMRAZA KEEN
 * Date:2016/6/29
 * IDE V1.6.9
 * Email:TechnicSmraza@outlook.com
 * Function:
 */
const int TRIGPINS[3] = {2,4,6};
const int ECHOPINS[3] = {3,5,7};
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
    pinMode(TrigPin[i], OUTPUT);
    pinMode(EchoPin[i], INPUT);  
  }
  
  //initial delay
  say("Initial setup delay");
  delay(5000);
}


void loop(){
  say("Entering sentry mode");
  sentry();
  say("Exited sentry mode");
  flee();
}


void sentry(){
  //get initial distances
  int initial[6];
  int checkSafe[6];
  //first three
  for(int i=0;i<3;i++){
    initial[i]=getdistance(TrigPin[i],EchoPin[i]);
  }
  //TODO move motors
  //second three (after turn)
  for(int i=3;i<6;i++){
    initial[i]=getdistance(TrigPin[i],EchoPin[i]);
  }
  for(int i=0;i<6;i++){
    checkSafe[6]=initial[6];
  }
  //sentry loop
  bool safe = true;
  while(safe){
    for(int i=0;i<3;i++){
      checkSafe[i]=getdistance(TrigPin[i],EchoPin[i]);
    }
    for(int i=0;i<3;i++){
      if checkSafe[i]>(initial[i]+movePadding)||checksafe[i]<(initial[i]-movePadding){
        safe=false;
        break;
      }
    }
    //TODO move motors
    //second three (after turn)
    for(int i=3;i<6;i++){
      checkSafe[i]=getdistance(TrigPin[i],EchoPin[i]);
    }
    for(int i=3;i<6;i++){
      if checkSafe[i]>(initial[i]+movePadding)||checksafe[i]<(initial[i]-movePadding){
        safe=false;
        break;
      }
    }
  }
}

bool flee(){
  bool caught = false;
  //TODO somehow run two functions
  //TODO Timer
  //TODO Run motors to avoid capture
}


void movemotors(right,left){
  //pass these into the motor
  say("I do absolutely nothing");
}

int getdistance(Trigger,Echo){
  digitalWrite(Trigger, LOW);       
  delayMicroseconds(2);
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger, LOW);
  cm = pulseIn(Echo, HIGH) / 58.0; //The echo time is converted into cm
  cm = (int(cm * 100.0)) / 100.0;
  return cm;
}


void say(string said){
  Serial.println(said);
}
