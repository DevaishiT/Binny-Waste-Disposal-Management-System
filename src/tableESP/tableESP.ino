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

#define M11 5
//#define M12 6

EspMQTTClient client(
  "AndroidAP",
  "01234567",
  "hansudows.local",
  "tableESPClient",
  1883
);

void buttonOnePressed(){
  // client.publish("binny/toBinnybotESP", "Go clean Table01");
  delay(20);
  client.publish("binny/toBinnybotESP", "T01");
  Serial.println("Button One Pressed");  
}
void buttonTwoPressed(){
  // client.publish("binny/toBinnybotESP", "Go clean Table02");
  delay(20);
  client.publish("binny/toBinnybotESP", "T02");
  Serial.println("Button Two Pressed");  
}


void setup() {

  Serial.begin(9600);
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
	
  pinMode(led, OUTPUT);
  pinMode(btn1, INPUT);
  pinMode(btn2, INPUT);
  pinMode(M11, OUTPUT);
  //pinMode(M12, OUTPUT);
  
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  delay(200);
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  delay(200);
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
}

// This function is called once everything is connected (Wifi and MQTT)
void onConnectionEstablished()
{
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(100);
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(1000);
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(100);
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(1000);
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(100);
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  // Publish a message to "binny/toBinnybotESP"
  client.publish("binny/toBinnybotESP", "Hi BinnybotESP : TableESP");
  
// Subscribe to "binny/topic" and display received message to Serial
  client.subscribe("binny/toTableESP", [](const String & payload) {
    Serial.println(payload);
    if(payload == "R01" || payload == "MR01" || payload == "MoR01" || payload == "AR01"){
      // client.publish("binny/toBinnybotESP", "Rotating Table 01 now...");
      for(int i=0; i<3; i++){
        digitalWrite(led, HIGH);
        delay(50);
        digitalWrite(led, LOW);
        delay(100);
      }
      //digitalWrite(M11,LOW);
      analogWrite(M11, 560);
      delay(15000);
      digitalWrite(M11,LOW);
      delay(1000);
      // client.publish("binny/toBinnybotESP", "Table 01 cleared. Initiate checker sweep");
      delay(2000);
      client.publish("binny/toBinnybotESP", "C01");
    }
    if(payload == "R02" || payload == "MR02" || payload == "MoR02" || payload == "AR01"){
      // client.publish("binny/toBinnybotESP", "Rotating Table 02 now...");
      for(int i=0; i<3; i++){
        digitalWrite(led, HIGH);
        delay(50);
        digitalWrite(led, LOW);
        delay(50);
        digitalWrite(led, HIGH);
        delay(50);
        digitalWrite(led, LOW);
        delay(500);
      }
      //digitalWrite(M11,LOW);
      analogWrite(M11,560);
      delay(15000);
      digitalWrite(M11,LOW);
      delay(1000);
      // client.publish("binny/toBinnybotESP", "Table 02 cleared. Initiate checker sweep");
      delay(2000);
      client.publish("binny/toBinnybotESP", "C02");
      
    }
  });


  // Execute delayed instructions
//  client.executeDelayed(5 * 1000, []() {
//    client.publish("binny/toBinnybotESP", "Sab chumma chal raha, carry on");
//  });
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
