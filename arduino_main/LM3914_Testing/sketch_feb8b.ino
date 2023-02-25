// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(10, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(13, HIGH);
  for (int i = 0; i < 256; i++) {
    analogWrite(10, i);  // turn the LED on (HIGH is the voltage level)
    Serial.println(i);
    delay(10);
  }
  for (int i = 255; i >= 0; i--) {
    analogWrite(10, i);  // turn the LED on (HIGH is the voltage level)
    Serial.println(i);
    delay(10);
  }
  digitalWrite(10, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(13, LOW);
  delay(1000);                      // wait for a second
}
