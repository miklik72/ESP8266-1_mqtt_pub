
/* Test comunication from Arduino via ESP-01 */
#include <Arduino.h>
#include <WiFiEsp.h>
#include <PubSubClient.h>
#include <DHT.h>

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include <SoftwareSerial.h>
  SoftwareSerial Serial1(4, 5); // RX, TX
#endif

#define BUILTIN_LED 13
#define BUTTON_PIN 6
#define DHTTYPE DHT11   // DHT 11
#define DHTPIN 7

DHT dht;

//wifi
#include <WifiAuth.h>                       // define ssid and pass bellow
//char ssid[] = "mm";                       // your network SSID (name)
//char pass[] = "";                         // your network password
int status = WL_IDLE_STATUS;                // the Wifi radio's status

const char* mqtt_server = "7gya1l.messaging.internetofthings.ibmcloud.com";
const int mqtt_port = 1883;
const char* mqtt_clntid = "d:7gya1l:MQTTtest:Arduino001";
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
unsigned long last_time = millis();
unsigned long repeat_time = 300000;
//unsigned long repeat_time = 15000;
bool button_state = false;

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
  Serial.println("Start");
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  dht.setup(DHTPIN);
  Serial.begin(115200);
  Serial1.begin(19203);  // ESP must be set to the same speed AT+UART_DEF=9600,8,1,0,1
  // initialize ESP module
  setup_wifi();
  if (client.connect(mqtt_clntid, mqtt_user, mqtt_auth)) {
    Serial.print("cln connected?");
    Serial.println(client.connected());
    //client.publish(mqtt_event,"{\"temp1\": \"22.0\",\"button1\": \"OFF\"}");
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
  } else
  {
    if(! digitalRead(BUTTON_PIN) && ! button_state)
    {
      button_state = true;
      Serial.print("button1: ");
      Serial.println(button_state);
      //client.publish(mqtt_event,"{\"button1\": 1}");
    }
    if (last_time + repeat_time < millis() || button_state)
    {
      Serial.println(last_time);
      Serial.println(last_time + repeat_time);
      last_time = millis();
      float h = dht.getHumidity();
      float t = dht.getTemperature();
      //String str_temp1 = "{\"temp1\":" + String(t) + "}";
      //String str_humi1 = "{\"humi1\":" + String(h) + "}";
      //int len_temp1 = str_temp1.length()+1;
      //int len_humi1 = str_humi1.length()+1;
      String str_mqtt = "{\"temp1\":" + String(t) + " ,\"humi1\":" + String(h) + " ,\"button1\":" + String(button_state) + "}";
      int len_mqtt_pub = str_mqtt.length()+1;
      char mqtt_pub[len_mqtt_pub];
      str_mqtt.toCharArray(mqtt_pub,len_mqtt_pub);
      //char mqtt_temp1[len_temp1];
      //str_temp1.toCharArray(mqtt_temp1,len_temp1);
      //char mqtt_humi1[len_humi1];
      //str_humi1.toCharArray(mqtt_humi1,len_humi1);
      // Serial.println("temp");
      // Serial.println(t);
      // Serial.println(len_temp1);
      // Serial.println(str_temp1);
      // Serial.println("humi");
      // Serial.println(len_humi1);
      // Serial.println(h);
      // Serial.println(str_humi1);
      Serial.println("mqtt");
      Serial.println(len_mqtt_pub);
      Serial.println(t);
      Serial.println(h);
      Serial.println(str_mqtt);

      //client.publish(mqtt_event,mqtt_temp1);
      //client.publish(mqtt_event,mqtt_humi1);
      client.publish(mqtt_event,mqtt_pub);
      //client.publish(mqtt_event,"{\"aaa\": 21, \"bbb\": \"23.00\", \"ccc\": 99.0}");
      //client.publish(mqtt_event,str_temp1);
      //client.publish(mqtt_event,str_humi1);
      Serial.println(last_time);
      //Serial.println("{\"temp1\": \"22.0\"}");
      //Serial.println(digitalRead(BUTTON_PIN));
    }
    if(digitalRead(BUTTON_PIN)) button_state = false;
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
