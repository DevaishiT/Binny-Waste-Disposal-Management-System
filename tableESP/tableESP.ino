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

#include <EspMQTTClient.h>

#define led 12
#define btn1 14
#define btn2 13

EspMQTTClient client(
  "Hansu",
  "hellohello",
  "hansudows.local",
  "tableESPClient",
  1883
);

void buttonOnePressed(){
  client.publish("binny/topic", "Go clean Table01 you Kamchor");
  Serial.println("Button One Pressed");  
}
void buttonTwoPressed(){
  client.publish("binny/topic", "Table02... Now!!!");
  Serial.println("Button Two Pressed");  
}


void setup() {

  Serial.begin(9600);
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
	
  pinMode(led, OUTPUT);
  pinMode(btn1, INPUT);
  pinMode(btn2, INPUT);
}

// This function is called once everything is connected (Wifi and MQTT)
void onConnectionEstablished()
{
  // Subscribe to "binny/topic" and display received message to Serial
  client.subscribe("binny/topic", [](const String & payload) {
    Serial.println(payload);
    if(payload == "T01"){
      digitalWrite(led, HIGH);
      delay(1000);
      digitalWrite(led, LOW);
    }
    if(payload == "T02"){
      digitalWrite(led, HIGH);
      delay(500);
      digitalWrite(led, LOW);
      delay(500);
      digitalWrite(led, HIGH);
      delay(500);
      digitalWrite(led, LOW);
    }
  });

  // Publish a message to "binny/topic"
  client.publish("binny/topic", "Ho gaya connect, 5 seconds baad dobara dekhta hu"); // You can activate the retain flag by setting the third parameter to true

  // Execute delayed instructions
  client.executeDelayed(5 * 1000, []() {
    client.publish("binny/topic", "Sab chumma chal raha, carry on");
  });
}
void loop() {
  if(digitalRead(btn1)){
    buttonOnePressed();
    delay(200);
  }
  if(digitalRead(btn2)){
    buttonTwoPressed();
    delay(200);
  }
  client.loop();
}
