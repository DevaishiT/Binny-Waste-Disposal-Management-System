#include <SoftwareSerial.h>
#include <EspMQTTClient.h>
#include <String.h>

/*    D0  : GPIO 16
      D1  : GPIO 5
      D2  : GPIO 4
      D3  : GPIO 0
      D4  : GPIO 2
      D5  : GPIO 14
      D6  : GPIO 12
      D7  : GPIO 13
      D8  : GPIO 15
      RX  : GPIO 3
      TX  : GPIO 1
      SD2 : GPIO 9
      SD3 : GPIO 10     */

#define rx 14           // D5
#define tx 12           // D6
#define BAUD_RATE 9600

const byte numChars = 64;
char receivedChars[numChars];
boolean newData = false;

SoftwareSerial SoftSerial;

EspMQTTClient client(
  "Hansu",
  "hellohello",
  "hansudows.local",
  "binnybotESPClient",
  1883
);

void setup() {

  Serial.begin(9600);
  SoftSerial.begin(BAUD_RATE, rx, tx, SWSERIAL_8N1, false, 95, 11);

  SoftSerial.println("Hi binnybotArduino : binnyBotESP");
  Serial.println("Hi binnybotArduino : binnyBotESP");
  delay(2000);
  
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output

}

// This function is called once everything is connected (Wifi and MQTT)
void onConnectionEstablished()
{
  // Subscribe to "binny/toBinnybotESP" and display received message to Serial
  client.subscribe("binny/toBinnybotESP", [](const String & payload) {
    SoftSerial.println(payload);
    Serial.println(payload);
  });

// Publish a message to "binny/toTableESP"
  client.publish("binny/toTableESP", "Hi TableESP : binnyBotESP"); // You can activate the retain flag by setting the third parameter to true

// Execute delayed instructions
//  client.executeDelayed(5 * 1000, []() {
//    client.publish("binny/toTableESP", "Sab chumma chal raha, carry on");
//  });
}
void loop() {
  readSSData();
  processNewSSData();
  client.loop();
}

void readSSData() {
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

void processNewSSData(){
  if (newData == true) {
    Serial.println(receivedChars);
    if(receivedChars[0] == 'R' &&receivedChars[1] == '0' && receivedChars[2] == '1'){
      Serial.println("R01");
      client.publish("binny/toTableESP", "R01");
    }
    if(receivedChars[0] == 'R' &&receivedChars[1] == '0' && receivedChars[2] == '2'){
      Serial.println("R02");
      client.publish("binny/toTableESP", "R02");
    }
    client.publish("binny/toTableESP", receivedChars);
    newData = false;
  }
}