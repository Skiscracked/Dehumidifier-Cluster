#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>

// Create an AHT20 object
Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud

  // Initialize the sensor
  if (!aht.begin()) {
    Serial.println("Could not find AHT20 sensor. Check wiring!");
    while (1);  // Halt the program if the sensor is not found
  }
  Serial.println("AHT20 sensor found!");
}

void loop() {
  sensors_event_t humidity, temp;
  
  // Get humidity and temperature events
  aht.getEvent(&humidity, &temp); 
  
  // Print the results
  Serial.print("Humidity: ");
  Serial.print(humidity.relative_humidity);
  Serial.println(" %");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" *C");

  delay(2000);  // Wait 2 seconds before the next reading
}
