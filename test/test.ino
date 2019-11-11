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


#define ss_rx 2
#define ss_tx 3

int arm_servo_lowered = 60;
int arm_servo_raised = 135;
int sweep_servo_min = 30;
int sweep_servo_max = 120;
int table_edge = 80;

const byte numChars = 64;
char receivedChars[numChars];
boolean newData = false;

boolean onPathToT01 = false;
boolean onPathToT02 = false;

Servo ArmServo;
Servo SweepServo;
SoftwareSerial SoftSerial(ss_rx, ss_tx); // RX, TX

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
  SweepServo.attach(sweepServoPin);

  // set the data rate for the SoftwareSerial port
  Serial.begin(9600);
  // SoftSerial.begin(9600);
  // delay(2000);
  // SoftSerial.println("Hi binnybotESP : binnybotArduino");
  
}

void loop() {
  
}

void stopISR(){
  if(onPathToT01)
    onPathToT01 = false;
  if(onPathToT02)
    onPathToT02 = false;

  ledstate = ~ ledstate;
}

void readSoftwareSerialData() {
  static byte ndx = 0;
  char endMarker = '\n';
  char c;
  while (SoftSerial.available() > 0 && newData == false) {
    c = SoftSerial.read();
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

void approachT01(){
  SoftSerial.println("Moving towards T01");
  onPathToT01 = true;
  while(onPathToT01){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }
  
  SoftSerial.println("R01");
  delay(500);
  SoftSerial.println("Reached T01, start rotation");
}
void checkT01(){
  for(int i=0; i<20; i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  SoftSerial.println("Checked T01, going home");
}
void approachT02(){
  SoftSerial.println("Moving towards T02");
  onPathToT02 = true;
  while(onPathToT02){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
  }
  delay(5000);
  ArmServo.write(arm_servo_lowered);
// TODO: Decrease the speed of Servo
  delay(1000);
  SoftSerial.println("R02");
  delay(500);
  SoftSerial.println("Reached T02, start rotation");
  
}
void checkT02(){
  ArmServo.write(arm_servo_raised);
// TODO: Decrease the speed of Servo
  delay(1000);
  int minDist;
  for(int i=sweep_servo_min; i<sweep_servo_max; i++){
    SweepServo.write(i);
    delay(50);
    int dist = checkDistance();
    SoftSerial.print("Angle: ");
    SoftSerial.print(i);
    SoftSerial.print("Distance: ");
    SoftSerial.println(dist);
    if(dist < minDist){
      minDist = dist;
    }
  }
  if(minDist < table_edge){
    ArmServo.write(arm_servo_lowered);
  // TODO: Decrease the speed of Servo
    delay(1000);
    SoftSerial.println("R02");
    delay(500);
    SoftSerial.println("Remaining kachra found, rotate T02 again");
  }
  else{
    SoftSerial.println("Checked T02, going home");
  // TODO: Add code to go home
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

void processNewSoftwareSerialData(){
  if (newData == true){
    Serial.println(receivedChars);
    SoftSerial.println(receivedChars);
    newData = false;

    if(receivedChars[0] == 'T' &&receivedChars[1] == '0' && receivedChars[2] == '1'){
      Serial.println("T01");
      approachT01();
    }
    if(receivedChars[0] == 'C' &&receivedChars[1] == '0' && receivedChars[2] == '1'){
      Serial.println("C01");
      checkT01();
    }
    if(receivedChars[0] == 'T' &&receivedChars[1] == '0' && receivedChars[2] == '2'){
      Serial.println("T02");
      approachT02();
    }
    if(receivedChars[0] == 'C' &&receivedChars[1] == '0' && receivedChars[2] == '2'){
      Serial.println("C02");
      checkT02();
    }
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
  digitalWrite(M11, LOW);
  digitalWrite(M12, LOW);
  digitalWrite(M21, LOW);
  digitalWrite(M22, LOW);
}