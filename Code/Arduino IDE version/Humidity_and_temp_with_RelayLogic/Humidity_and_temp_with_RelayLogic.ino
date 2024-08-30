#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>

// Create an AHT20 sensor object
Adafruit_AHTX0 aht;

// Define the pin connected to the relay
#define RELAY_PIN 7

// Define the humidity thresholds
#define HUMIDITY_THRESHOLD_HIGH 63.0  // Turn on the relay (dehumidifier) if humidity > 63%
#define HUMIDITY_THRESHOLD_LOW 48.0   // Turn off the relay (dehumidifier) if humidity < 48%

// Define the debounce delay in milliseconds
#define DEBOUNCE_DELAY 5000  // 5 seconds

// Variables for debounce logic
float lastHumidity = 0.0;                // Stores the last stable humidity reading
unsigned long lastDebounceTime = 0;      // Stores the last time humidity changed significantly

void setup() {
  Serial.begin(9600);  // Start serial communication

  // Initialize the relay pin as output
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // Ensure the relay is off initially (assuming active-low relay)

  // Initialize the AHT20 sensor
  if (!aht.begin()) {
    Serial.println("Could not find AHT20 sensor! Check wiring.");
    while (1);  // Halt the program if the sensor is not found
  }
  Serial.println("AHT20 sensor found!");
}

void loop() {
  sensors_event_t humidity, temp;

  // Get the humidity and temperature data from the AHT20 sensor
  aht.getEvent(&humidity, &temp);

  // Print the humidity and temperature values for debugging
  Serial.print("Humidity: ");
  Serial.print(humidity.relative_humidity);
  Serial.print(" %\tTemperature: ");
  Serial.print(temp.temperature);
  Serial.println(" *C");

  // Check if the humidity has changed significantly (by more than 2%)
  if (abs(humidity.relative_humidity - lastHumidity) > 2.0) {
    Serial.println("Significant humidity change detected.");
    lastHumidity = humidity.relative_humidity; // Update the last stable humidity
    lastDebounceTime = millis();  // Reset the debounce timer
  }

  // Print the current time and the last debounce time for debugging
  Serial.print("Current millis: ");
  Serial.println(millis());
  Serial.print("Last debounce time: ");
  Serial.println(lastDebounceTime);
  Serial.print("Time since last change: ");
  Serial.println(millis() - lastDebounceTime);

  // Check if the debounce delay has passed
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    Serial.println("Debounce delay passed. Evaluating relay...");
    // Only control the relay if the humidity has been stable for the debounce period
    if (lastHumidity > HUMIDITY_THRESHOLD_HIGH) {
      Serial.println("Humidity above threshold. Turning on relay.");
      digitalWrite(RELAY_PIN, LOW);  // Turn on the relay (dehumidifier)
    } else if (lastHumidity < HUMIDITY_THRESHOLD_LOW) {
      Serial.println("Humidity below threshold. Turning off relay.");
      digitalWrite(RELAY_PIN, HIGH); // Turn off the relay (dehumidifier)
    }
  } else {
    Serial.println("Debounce delay not yet passed.");
  }

  delay(1000);  // Small delay before the next reading
}
