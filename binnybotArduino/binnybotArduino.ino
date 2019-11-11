#include <SoftwareSerial.h>
#include <String.h>

const byte numChars = 64;
char receivedChars[numChars];
boolean newData = false;

SoftwareSerial SoftSerial(2, 3); // RX, TX

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  // set the data rate for the SoftwareSerial port
  Serial.begin(9600);
  SoftSerial.begin(9600);
  SoftSerial.println("Hi binnybotESP : binnybotArduino");
  delay(2000);
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
for(int i=0; i<20; i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }
  SoftSerial.println("R01");
  delay(500);
//  SoftSerial.println("Reached T01, start rotation");
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
  for(int i=0; i<20; i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
  }
  SoftSerial.println("R02");
  delay(500);
//  SoftSerial.println("Reached T02, start rotation");
  
}
void checkT02(){
  
  for(int i=0; i<20; i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }
  SoftSerial.println("Checked T02, going home");
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
