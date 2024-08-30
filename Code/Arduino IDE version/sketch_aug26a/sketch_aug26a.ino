int relayPin = 7; // Digital pin connected to the transistor that controls the relay

void setup() {
  pinMode(relayPin, OUTPUT);
}

void loop() {
  digitalWrite(relayPin, HIGH); // Turn off the relay
  delay(5000);                  // Wait 5 seconds
  digitalWrite(relayPin, LOW);  // Turn on the relay
  delay(5000);                  // Wait 5 seconds
}
