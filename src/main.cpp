
/* Test comunication from Arduino via ESP-01 */
#include <Arduino.h>
#include <WiFiEsp.h>
#include <PubSubClient.h>

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include <SoftwareSerial.h>
  SoftwareSerial Serial1(4, 5); // RX, TX
#endif

#define BUILTIN_LED 13

//wifi
#include <WifiAuth.h>                       // define ssid and pass bellow
//char ssid[] = "mm";                       // your network SSID (name)
//char pass[] = "";                         // your network password
int status = WL_IDLE_STATUS;                // the Wifi radio's status

const char* mqtt_server = "7gya1l.messaging.internetofthings.ibmcloud.com";
const int mqtt_port = 1883;
const char* mqtt_clntid = "d:7gya1l:MQTTclient:atest1";
const char* mqtt_event = "iot-2/evt/event1/fmt/json";
const char* mqtt_user = "use-token-auth";
const char* mqtt_auth = "TUZNgXdi2fy?FIvTpm";

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

// Initialize the Ethernet client object
WiFiEspClient espClient;
PubSubClient client(mqtt_server,mqtt_port,callback, espClient);
long lastMsg = 0;
char msg[50];
long int value = 0;

void setup_wifi()
{
  WiFi.init(&Serial1);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }
  // you're connected now, so print out the data
  Serial.println("You're connected to the network");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// void callback(char* topic, byte* payload, unsigned int length) {
//   Serial.print("Message arrived [");
//   Serial.print(topic);
//   Serial.print("] ");
//   for (unsigned int i = 0; i < length; i++) {
//     Serial.print((char)payload[i]);
//   }
//   Serial.println();
//
//   // Switch on the LED if an 1 was received as first character
//   if ((char)payload[0] == '1') {
//     digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
//     // but actually the LED is on; this is because
//     // it is acive low on the ESP-01)
//   } else {
//     digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
//   }
//
// }

void setup(){
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  Serial1.begin(19203);  // ESP must be set to the same speed AT+UART_DEF=9600,8,1,0,1
  // initialize ESP module
  setup_wifi();
  if (client.connect(mqtt_clntid, mqtt_user, mqtt_auth)) {
    client.publish(mqtt_event,"{\"prop1\": \"Hello\",\"prop2\": \"world\",\"prop3\": \"ON\"}");
    //client.subscribe("inTopic");
  }
  //client.setServer(mqtt_server, mqtt_port);
  //client.setCallback(callback);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_clntid, mqtt_user, mqtt_auth)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      //client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {
  if (!client.connected()) {
     Serial.print("cln connected?");
     Serial.println(client.connected());
     reconnect();
  }
  client.loop();

  // long now = millis();
  // if (now - lastMsg > 2000) {
  //   lastMsg = now;
  //   ++value;
  //   snprintf (msg, 75, "hello world #%ld", value);
  //   Serial.print("Publish message: ");
  //   Serial.println(msg);
  //   client.publish("outTopic", msg);
  // }
}
