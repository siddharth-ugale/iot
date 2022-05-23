/*
  Publishing in the callback

  - connects to an MQTT server
  - subscribes to the topic "inTopic"
  - when a message is received, republishes it to "outTopic"

  This example shows how to publish messages within the
  callback function. The callback function header needs to
  be declared before the PubSubClient constructor and the
  actual callback defined afterwards.
  This ensures the client reference in the callback function
  is valid.

*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

const char* ssid = "Alex";
const char* password = "12345678";
const char* mqtt_server = "192.168.43.101";
int slot1, slot2, slot3, slot4, flag;

int pins[4] = {D5, D6, D7};

// Update these with values suitable for your network.
WiFiClient espClient;


PubSubClient client(espClient);

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);
  } else {
    digitalWrite(BUILTIN_LED, HIGH);
  }



}


void setup()
{
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.begin(115200);
  pinMode(D3, INPUT);

  //setting input pins
  for (int i = 0; i < 3; i++) {
    pinMode(pins[i], INPUT_PULLUP);
  }

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, 0);
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("lol", "Connected successfully");
      // ... and resubscribe
      client.subscribe("wemos");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(500);
    }
  }
}
void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  static int pinvalues[4];
  for (int i = 0 ; i < 3; i++ )
  {
    if ( pinvalues[i] != digitalRead(pins[i]))
    {
      pinvalues[i] = digitalRead(pins[i]);
      String data = "{ \"light1\":";
      data += pinvalues[0] ;
      data += ",\"light2\":";
      data +=  pinvalues[1];
      data += ", \"light3\" :";
      data += pinvalues[2];
      data += "}";
  char message[100];
  data.toCharArray(message, 100);
  Serial.println(message);
  client.publish("mytopic", message);
    }
  }
  client.loop();

}
