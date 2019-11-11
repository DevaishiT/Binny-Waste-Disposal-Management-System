#include <SoftwareSerial.h>
#include <String.h>
#include <Servo.h>

#define stopButton 3
#define armServoPin 9
#define sweepServoPin 10

int arm_servo_lowered = 30;
int arm_servo_raised = 120;
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
SoftwareSerial SoftSerial(2, 3); // RX, TX

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(stopButton, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(stopButton), stopISR, RISING);

  ArmServo.attach(armServoPin);
  SweepServo.attach(sweepServoPin);

  // set the data rate for the SoftwareSerial port
  Serial.begin(9600);
  SoftSerial.begin(9600);
  delay(2000);
  SoftSerial.println("Hi binnybotESP : binnybotArduino");
}

void stopISR(){
  if(onPathToT01)
    onPathToT01 = false;
  if(onPathToT02)
    onPathToT02 = false;
}
void loop() {
  readSoftwareSerialData();
  processNewSoftwareSerialData();
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
  while(onPathtoT01){
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
  while(onPathtoT02){
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

void checkDistance(){
  ;
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
