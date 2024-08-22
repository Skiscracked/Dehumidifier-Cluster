#include <WiFi.h>               // Library for Wi-Fi connectivity
#include <PubSubClient.h>       // Library for MQTT communication
#include <DHT.h>                // Library for interfacing with the DHT sensor

// Wi-Fi credentials
const char* ssid = "EASTLINK861";           // Replace with your Wi-Fi SSID
const char* password = "44Okunola?1#";   // Replace with your Wi-Fi password

// MQTT broker information
const char* mqtt_server = "192.168.6.25 fd0c:3ecd:9225:1:46ea:e1c2:202b:9bdf";  // Replace with your Raspberry Pi's IP address

// DHT22 sensor setup
#define DHTPIN 2               // GPIO pin where the DHT22 is connected
#define DHTTYPE DHT22          // Define the sensor type (DHT22)
DHT dht(DHTPIN, DHTTYPE);      // Create a DHT sensor object

// Relay setup
#define RELAY_PIN 5            // GPIO pin connected to the relay module

// Wi-Fi and MQTT client setup
WiFiClient espClient;          // Wi-Fi client object
PubSubClient client(espClient); // MQTT client object using the Wi-Fi client

// Function: setup_wifi
// Description: Connects the ESP32/ESP8266 to the Wi-Fi network.
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Initiate Wi-Fi connection

  // Wait until the Wi-Fi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

// Function: reconnect
// Description: Reconnects to the MQTT broker if the connection is lost.
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client_Recroom")) {
      Serial.println("connected");
      // Once connected, you can subscribe to topics if needed
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Function: setup
// Description: Initializes the sensor, relay, and sets up Wi-Fi and MQTT connections.
void setup() {
  Serial.begin(115200);               // Start the serial monitor
  pinMode(RELAY_PIN, OUTPUT);         // Set the relay pin as output
  digitalWrite(RELAY_PIN, HIGH);       // Ensure the relay is off at startup(HIGH for low-level trigger)
  setup_wifi();                       // Connect to the Wi-Fi network
  client.setServer(mqtt_server, 1883); // Set the MQTT broker IP and port
  dht.begin();                        // Initialize the DHT22 sensor
}

// Function: loop
// Description: Continuously reads humidity, controls the relay based on thresholds, and sends data via MQTT.
void loop() {
  if (!client.connected()) {   // Reconnect to MQTT if the connection is lost
    reconnect();
  }
  client.loop();               // Ensure the MQTT client processes incoming messages

  float humidity = dht.readHumidity(); // Read humidity from the DHT22 sensor

  // Check if the reading is valid
  if (isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Control the relay based on humidity levels
  if (humidity > 63) {
    digitalWrite(RELAY_PIN, LOW);  // Turn on the relay (dehumidifier) if humidity > 60% (Made it 63% to allow room for any tolerance)
  } else if (humidity < 48) {
    digitalWrite(RELAY_PIN, HIGH);   // Turn off the relay (dehumidifier) if humidity < 50% (Made it 48% to allow room for any tolerance)
  }

  // Publish the humidity reading to the MQTT broker
  String payload = String(humidity);
  client.publish("home/Recroom/humidity", payload.c_str());

  delay(2000);  // Wait 2 seconds before the next reading
}
