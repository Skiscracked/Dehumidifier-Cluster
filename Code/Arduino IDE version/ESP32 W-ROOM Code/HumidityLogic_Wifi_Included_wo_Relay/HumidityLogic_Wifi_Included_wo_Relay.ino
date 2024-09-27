#include <WiFi.h>               // Library for Wi-Fi connectivity
#include <PubSubClient.h>       // Library for MQTT communication
#include <Adafruit_Sensor.h>    // Libraries for interfacing with the DHT sensor
#include <Adafruit_AHTX0.h>                

// Create an AHT20 sensor object
Adafruit_AHTX0 aht;

// Wi-Fi credentials
const char* ssid = "ARRIS-355B";           // Replace with your Wi-Fi SSID
const char* password = "BSY776600733";      // Replace with your Wi-Fi password

// MQTT broker information
const char* mqtt_server = "192.168.0.20";   // Replace with your Raspberry Pi's IP address


// Relay setup
#define RELAY_PIN 4            // GPIO pin connected to the relay module

// Wi-Fi and MQTT client setup
WiFiClient espClient;          // Wi-Fi client object
PubSubClient client(espClient); // MQTT client object using the Wi-Fi client

// Light sleep duration (5 minutes in microseconds)
const uint64_t SLEEP_INTERVAL = 300000000;

void setup_wifi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting to Wi-Fi...");
    WiFi.begin(ssid, password);  // Reconnect to Wi-Fi

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("Wi-Fi reconnected");
  } else {
    Serial.println("Wi-Fi already connected");
  }
}

// Reconnects to the MQTT broker if the connection is lost
void reconnect() {
  // Check if the client is connected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect to the broker
    if (client.connect("ESP32Client_Sksroom")) {
      Serial.println("Connected to MQTT broker");
      client.subscribe("home/#");  // Subscribe to relevant topic
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Trying again in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);               // Start the serial monitor
  pinMode(RELAY_PIN, OUTPUT);         // Set the relay pin as output
  digitalWrite(RELAY_PIN, HIGH);      // Ensure the relay is off at startup (HIGH for low-level trigger)
  setup_wifi();                       // Connect to the Wi-Fi network
  client.setServer(mqtt_server, 1883); // Set the MQTT broker IP and port

   // Initialize the AHT20 sensor
  if (!aht.begin()) {
    Serial.println("Could not find AHT20 sensor! Check wiring.");
    while (1);  // Halt the program if the sensor is not found
  }
  Serial.println("AHT20 sensor found!");

  WiFi.setSleep(false); // Don't disable wifi upon sleep
  // Set up the light sleep wake-up interval
  esp_sleep_enable_timer_wakeup(SLEEP_INTERVAL);
}

void loop() {
  // Reconnect to Wi-Fi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }
  // Reconnect to MQTT if the connection is lost
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // Ensure the MQTT client processes incoming messages
  delay(100);  // Small delay to allow time for connection to stabilize

  sensors_event_t humidity, temp;

  // Get the humidity and temperature data from the AHT20 sensor
  aht.getEvent(&humidity, &temp);


  // Check if the reading is valid
  if (isnan(humidity.relative_humidity)) {
    Serial.println("Failed to read from AHT sensor!");
    return;
  }

  // Control the relay based on humidity levels
  String relayStatus = "OFF";  // Default relay status
  if (humidity.relative_humidity > 63) {
    digitalWrite(RELAY_PIN, LOW);  // Turn on the relay (dehumidifier) if humidity > 63%
    relayStatus = "ON";
  } else if (humidity.relative_humidity < 48) {
    digitalWrite(RELAY_PIN, HIGH); // Turn off the relay (dehumidifier) if humidity < 48%
    relayStatus = "OFF";
  }

  // Publish the humidity and relay status as a single message to the MQTT broker
  String payload = "Humidity: " + String(humidity.relative_humidity) + " %, Relay: " + relayStatus;
  client.publish("home/Sksroom/humidity", payload.c_str());
  client.loop();
  delay(200);  // Ensure MQTT publishing happens before sleep


  // Print humidity and relay status
  Serial.print("Humidity: ");
  Serial.print(humidity.relative_humidity);
  Serial.print(" %, Relay Status: ");
  Serial.println(relayStatus);

  // Go into light sleep for 5 minutes
Serial.println("Entering light sleep for 5 minutes...");
delay(1000);  // Small delay to allow all operations to finish
esp_light_sleep_start();  // Enter light sleep mode
}
