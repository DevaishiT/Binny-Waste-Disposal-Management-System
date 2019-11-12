#include <SoftwareSerial.h>
#include <String.h>
#include <Servo.h>

#define stopButton 21
#define armServoPin 8
#define sweepServoPin 9

#define ping_trig 15
#define ping_echo 14
long ping_duration;
int ping_distance;

int ledstate = 0;
#define ledblue 28
#define ledwhite 29
#define ledred 22
#define ledyellow1 23
#define ledyellow2 24
#define ledyellow3 25
#define ledyellow4 26
#define ledgreen 27

#define L1 A15
#define L2 A14
#define L3 A13
#define L4 A12

#define M11 4
#define M12 5
#define M21 7
#define M22 6

int arm_servo_lowered = 135;
int arm_servo_raised = 50;
// TODO: Caliberate values of table_edge, sweep_servo_max and sweep_servo_min
int sweep_servo_min = 70;
int sweep_servo_max = 160; 
int table_edge = 80;

const byte numChars = 64;
char receivedChars[numChars];
boolean newData = false;

boolean onPath = false;

Servo ArmServo;
Servo SweepServo;

void setup()
{
  pinMode(ledblue, OUTPUT);
  pinMode(ledwhite, OUTPUT);
  pinMode(ledred, OUTPUT);
  pinMode(ledyellow1, OUTPUT);
  pinMode(ledyellow2, OUTPUT);
  pinMode(ledyellow3, OUTPUT);
  pinMode(ledyellow4, OUTPUT);
  pinMode(ledgreen, OUTPUT);

  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);

  pinMode(ping_trig, OUTPUT);
  pinMode(ping_echo, INPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(stopButton, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(stopButton), stopISR, RISING);

  ArmServo.attach(armServoPin);
  ArmServo.write(arm_servo_raised);
  SweepServo.attach(sweepServoPin);
  SweepServo.write(sweep_servo_min);

  // set the data rate for the SoftwareSerial port
  Serial1.begin(9600);
  delay(2000);
  Serial1.println("binnybotArduino : Hi binnybotESP");
}

void loop() {
  readSoftwareSerialData();
  processNewSoftwareSerialData();
}

void stopISR(){
  if(onPath)
    onPath = false;
  if(onPath)
    onPath = false;
}

void readSoftwareSerialData() {
  static byte ndx = 0;
  char endMarker = '\n';
  char c;
  while (Serial1.available() > 0 && newData == false) {
    c = Serial1.read();
    if (c != endMarker) {
      receivedChars[ndx] = c;
      ndx++;
      if (ndx >= numChars){
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void processNewSoftwareSerialData(){
  if (newData == true){
    digitalWrite(ledgreen, HIGH);
    delay(100);
    digitalWrite(ledgreen, LOW);
    delay(100);
    digitalWrite(ledgreen, HIGH);
    delay(100);
    digitalWrite(ledgreen, LOW);

    delay(1000);
    Serial1.print("binnybotArduino received: ");
    Serial1.println(receivedChars);
    newData = false;

    if(receivedChars[0] == 'T' &&receivedChars[1] == '0' && receivedChars[2] == '1'){
      approachT01();
    }
    if(receivedChars[0] == 'C' &&receivedChars[1] == '0' && receivedChars[2] == '1'){
      checkT01();
    }
    if(receivedChars[0] == 'T' &&receivedChars[1] == '0' && receivedChars[2] == '2'){
      approachT02();
    }
    if(receivedChars[0] == 'C' &&receivedChars[1] == '0' && receivedChars[2] == '2'){
      checkT02();
    }
  }
}

void approachT01(){
  delay(1000);
  Serial1.println("Moving towards T01");
  onPath = true;

  while(onPath){
    followPath(1);
  }
  stop();
  delay(1000);
  Serial1.println("Reached T01, lowering arm");
  lowerArmNow();
  delay(1000);
  Serial1.println("R01");
  delay(1000);
  Serial1.println("Reached T01, start rotation");
}
void checkT01(){
  delay(1000);
  raiseArmNow();
  delay(1000);
  int minDist = INT16_MAX;
  for(int i=sweep_servo_min; i<sweep_servo_max; i++){
    SweepServo.write(i);
    delay(50);
    int dist = checkDistance();
    Serial1.print("Angle: ");
    Serial1.print(i);
    Serial1.print("Distance: ");
    Serial1.println(dist);
    if(dist < minDist){
      minDist = dist;
    }
  }
  if(minDist < table_edge){
    lowerArmNow();
    delay(1000);
    Serial1.println("R01");
    delay(1000);
    Serial1.println("Remaining kachra found, rotate T01 again");
  }
  else{
    delay(1000);
    Serial1.println("Checked T01, going home");
    takeuturn();
    onPath = true;
    while (onPath)
    {
      followPath(1);
    }
    stop();
    takeuturn();
  }
}
void approachT02(){
  delay(1000);
  Serial1.println("Moving towards T02");
  onPath = true;

  while(onPath){
    followPath(0);
  }
  stop();
  lowerArmNow();

  delay(1000);
  Serial1.println("R02");
  delay(1000);
  Serial1.println("Reached T02, start rotation");
}
void checkT02(){
  delay(1000);
  raiseArmNow();
  delay(1000);
  int minDist = INT16_MAX;
  for(int i=sweep_servo_min; i<sweep_servo_max; i++){
    SweepServo.write(i);
    delay(50);
    int dist = checkDistance();
    Serial1.print("Angle: ");
    Serial1.print(i);
    Serial1.print("Distance: ");
    Serial1.println(dist);
    if(dist < minDist){
      minDist = dist;
    }
  }
  if(minDist < table_edge){
    lowerArmNow();
    delay(1000);
    Serial1.println("R02");
    delay(1000);
    Serial1.println("Remaining kachra found, rotate T02 again");
  }
  else{
    delay(1000);
    Serial1.println("Checked T02, going home");
    takeuturn();
    onPath = true;
    while (onPath)
    {
      followPath(1);
    }
    stop();
    takeuturn();
  }
}

int checkDistance(){
// Clears the trigPin
  digitalWrite(ping_trig, LOW);
  delayMicroseconds(2);
// Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(ping_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(ping_trig, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
  ping_duration = pulseIn(ping_echo, HIGH);
// Calculating the distance
  ping_distance = ping_duration*0.034/2;

  return ping_distance;
}

void lowerArmNow(){
  for(int i=arm_servo_lowered; i<=arm_servo_raised; i++){
    ArmServo.write(i);
    delay(150);
  }
}
void raiseArmNow(){
  for(int i=arm_servo_raised; i>=arm_servo_lowered; i--){
    ArmServo.write(i);
    delay(150);
  }
}

void forward(){
  digitalWrite(M12, LOW);
  digitalWrite(M22, LOW);
}

void backward(){
  digitalWrite(M11, LOW);
  digitalWrite(M21, LOW);
}

void stop(){
  digitalWrite(ledred, HIGH);
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
}

void takeuturn(){
  digitalWrite(ledwhite, HIGH);
  analogWrite(M11, 135);
  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);
  analogWrite(M22, 120);
  delay(5000);
  stop();
  digitalWrite(ledwhite, LOW);
}

void followPath(int dir){
  digitalWrite(ledred, LOW);
  digitalWrite(ledblue, HIGH);
  //int dir = 1; /* 1 = LEFT, 0 = RIGHT */
  int s1=digitalRead(L1);
  int s2=digitalRead(L2);
  int s3=digitalRead(L3);
  int s4=digitalRead(L4);

  digitalWrite(ledyellow1, s1);
  digitalWrite(ledyellow2, s2);
  digitalWrite(ledyellow3, s3);
  digitalWrite(ledyellow4, s4);
// 1111
  if(s1 && s2 && s3 && s4){
    stop();
    delay(250);
    backward();
    analogWrite(M12, 120);
    analogWrite(M22, 120);
    delay(500);
  }
// 1110
  else if(s1 && s2 && s3 && !s4){
    forward();
    analogWrite(M11, 120);
    analogWrite(M21, 30);
    delay(50);
  }
// 1100
  else if(s1 && s2 && !s3 && !s4){
    forward();
    analogWrite(M11, 120);
    analogWrite(M21, 60);
    delay(50);
  }
//1001
  else if(s1 && !s2 && !s3 && s4){
    forward();
    analogWrite(M11, 120);
    analogWrite(M21, 120);
    delay(50);
  }
// 1000
  else if(s1 && !s2 && !s3 && !s4){
    forward();
    analogWrite(M11, 120);
    analogWrite(M21, 60);
    delay(50);
  }
// 0111
  if(!s1 && s2 && s3 && s4){
    forward();
    analogWrite(M11, 30);
    analogWrite(M21, 120);
    delay(50);
  }
// 0011
  else if(!s1 && !s2 && s3 && s4){
    forward();
    analogWrite(M11, 60);
    analogWrite(M21, 120);
    delay(50);
  }
// 0001
  else if(!s1 && !s2 && !s3 && s4){
    forward();
    analogWrite(M11, 60);
    analogWrite(M21, 120);
    delay(50);
  }
// 0000
  else if(!s1 && !s2 && !s3 && !s4){
    if (dir){
      forward();
      analogWrite(M11, 30);
      analogWrite(M21, 120);
    }
    if (!dir){
      forward();
      analogWrite(M11, 120);
      analogWrite(M21, 30);
    }
    delay(50);
  }
// every other input
  else {
    forward();
      analogWrite(M11, 120);
      analogWrite(M21, 120);
  }
  digitalWrite(ledblue, LOW);
  delay(50);
}
