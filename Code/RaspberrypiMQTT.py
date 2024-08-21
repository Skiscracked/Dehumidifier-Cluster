import paho.mqtt.client as mqtt  # Import the MQTT client library

# Function: on_connect
# Description: Handles the event when the client connects to the MQTT broker.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("home/#")  # Subscribe to all topics under "home/"

# Function: on_message
# Description: Handles incoming messages and processes the data.
def on_message(client, userdata, msg):
    print(f"Topic: {msg.topic} - Message: {str(msg.payload.decode())}")

# Set up the MQTT client
client = mqtt.Client()
client.on_connect = on_connect  # Define the callback function for connection
client.on_message = on_message  # Define the callback function for incoming messages

client.connect("localhost", 1883, 60)  # Connect to the MQTT broker on the Raspberry Pi

# Keep the script running to listen for incoming messages
client.loop_forever()
