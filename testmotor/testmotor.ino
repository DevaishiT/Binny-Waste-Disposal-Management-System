#define M11 5
#define M12 6
void setup() {
  pinMode(M11, OUTPUT);
  pinMode(M12, OUTPUT);
}

void loop() {
  forward();
  delay(5000);
  stop();
  delay(5000);
  backward();
  delay(5000);
  stop();
}

void forward()
{
  analogWrite(M11,135);
  digitalWrite(M12,LOW);
}
void backward()
{
  digitalWrite(M11,LOW);
  analogWrite(M12,135);
}
void stop()
{
  digitalWrite(M11,LOW);
  digitalWrite(M12,LOW);
}
