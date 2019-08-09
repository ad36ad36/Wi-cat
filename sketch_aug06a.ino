const int stepPin = 15;
const int dirPin = 33;
const int stepsPerRevolution = 200;

void setup() {
  // put your setup code here, to run once:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(dirPin,HIGH);
  for(int x = 0; x<200; x++) {
    digitalWrite(stepPin, HIGH);
    //delay(2);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    //delay(2);
    delayMicroseconds(500);
    Serial.println(x);
  }
  delay(3000);
}
