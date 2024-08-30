#include <Wire.h>               // Library for I2C communication
#include <WiFi.h>               // Library for Wi-Fi connectivity
#include <PubSubClient.h>       // Library for MQTT communication
#include <Adafruit_AHTX0.h>     // Library for the DHT20 sensor
#include <HardwareSerial.h>

Adafruit_AHTX0 dht20;           // Initialize the DHT20 sensor object

// Wi-Fi credentials
const char* ssid = "EASTLINK861";           // Replace with your Wi-Fi SSID
const char* password = "44Okunola?1#";      // Replace with your Wi-Fi password

// MQTT broker information
const char* mqtt_server = "192.168.6.25";   // Replace with your Raspberry Pi's IP address

// Relay setup
#define RELAY_PIN 6            // GPIO pin connected to the relay module

// Wi-Fi and MQTT client setup
WiFiClient espClient;
PubSubClient client(espClient);

HardwareSerial mySerial(1); // Use Serial1 or another UART

// Function: setup_wifi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Initiate Wi-Fi connection

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

// Function: reconnect
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client_Sksroom")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

// Function: setup
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);  // Wait until the serial connection is established
  }
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // Ensure the relay is off at startup
  setup_wifi();                   // Connect to the Wi-Fi network
  client.setServer(mqtt_server, 1883);

  // Set up I2C communication with custom pins
  Wire.begin(8, 9); // SDA on GPIO 8, SCL on GPIO 9

  if (!dht20.begin()) {
    Serial.println("Failed to start DHT20 sensor!");
    while (1);  // Halt if the sensor doesn't initialize
  }

  // Initialize the secondary serial port (must be done in setup)
  mySerial.begin(115200, SERIAL_8N1, 43, 44);  // Replace 43 and 44 with your actual TX and RX GPIO numbers
}

// Function: loop
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  sensors_event_t humidityEvent, tempEvent;
  dht20.getEvent(&humidityEvent, &tempEvent);  // Read data from the DHT20 sensor

  Serial.println("Starting humidity monitoring...");  // Add at the beginning of loop()
  Serial.print("Humidity: ");
  Serial.println(humidityEvent.relative_humidity);

  if (humidityEvent.relative_humidity > 63) {
    digitalWrite(RELAY_PIN, LOW);  // Turn on the relay (dehumidifier) if humidity > 63%
  } else if (humidityEvent.relative_humidity < 48) {
    digitalWrite(RELAY_PIN, HIGH); // Turn off the relay (dehumidifier) if humidity < 48%
  }

  String payload = String(humidityEvent.relative_humidity);
  client.publish("home/Sksroom/humidity", payload.c_str());

  delay(2000);  // Wait 2 seconds before the next reading
}
