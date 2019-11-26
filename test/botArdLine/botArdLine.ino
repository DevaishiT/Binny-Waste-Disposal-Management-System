#define M11 4 //left forward
#define M12 5
#define M21 7 //right forward
#define M22 6
#define L1 A15
#define L2 A14
#define L3 A13
#define L4 A12

void setup() {
  Serial.begin(9600);
  delay(1000);
  pinMode(L1, INPUT);
  pinMode(L2, INPUT);  
  pinMode(L3, INPUT);
  pinMode(L4, INPUT);
  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
  pinMode(M21, OUTPUT);
  pinMode(M22, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Booting up, wait 2 seconds");
  delay(2800);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  int dir = 1; // 0 is right
  int s1=digitalRead(L1);
  int s2=digitalRead(L2);
  int s3=digitalRead(L3);
  int s4=digitalRead(L4);
  Serial.print(s1);
  Serial.print("\t");
  Serial.print(s2);
  Serial.print("\t");
  Serial.print(s3);
  Serial.print("\t");
  Serial.print(s4);
  Serial.print("\n");
// 1111
  if(s1 && s2 && s3 && s4){
    backward();
    analogWrite(M12, 80);
    analogWrite(M22, 80);
    delay(500);
  }
// 1110
  else if(s1 && s2 && s3 && !s4){
    forward();
    analogWrite(M11, 80);
    analogWrite(M21, 20);
    delay(50);
  }
// 1100
  else if(s1 && s2 && !s3 && !s4){
    forward();
    analogWrite(M11, 80);
    analogWrite(M21, 40);
    delay(50);
  }
//1001
  else if(s1 && !s2 && !s3 && s4){
    forward();
    analogWrite(M11, 80);
    analogWrite(M21, 80);
    delay(50);
  }
// 1000
  else if(s1 && !s2 && !s3 && !s4){
    forward();
    analogWrite(M11, 80);
    analogWrite(M21, 60);
    delay(50);
  }
// 0111
  if(!s1 && s2 && s3 && s4){
    forward();
    analogWrite(M11, 20);
    analogWrite(M21, 80);
    delay(50);
  }
// 0011
  else if(!s1 && !s2 && s3 && s4){
    forward();
    analogWrite(M11, 40);
    analogWrite(M21, 80);
    delay(50);
  }
// 0001
  else if(!s1 && !s2 && !s3 && s4){
    forward();
    analogWrite(M11, 60);
    analogWrite(M21, 80);
    delay(50);
  }
// 0000
  else if(!s1 && !s2 && !s3 && !s4){
    if (dir){
      forward();
      analogWrite(M11, 20);
      analogWrite(M21, 80);
    }
    if (!dir){
      forward();
      analogWrite(M11, 80);
      analogWrite(M21, 20);
    }
    delay(50);
  }
// 0101
  else if(!s1 && s2 && !s3 && s4){
    stop();
  }

  else {
    forward();
    analogWrite(M11, 80);
    analogWrite(M21, 80);
    delay(50);
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
