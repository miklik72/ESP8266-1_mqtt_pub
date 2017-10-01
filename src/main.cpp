
/* Test comunication from Arduino via ESP-01 */
#include <Arduino.h>
#include <WiFiEsp.h>

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include <SoftwareSerial.h>
  SoftwareSerial Serial1(10, 11); // RX, TX
#endif

//wifi
#include <WifiAuth.h>
//char ssid[] = "mm";                       // your network SSID (name)
//char pass[] = "PointHlubcickaPoint";        // your network password
int status = WL_IDLE_STATUS;               // the Wifi radio's status

//web
char webserver[] = "www.miklik.cz";
//int webport = 443;
int webport = 80;

// Initialize the Ethernet client object
WiFiEspClient client;

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);  // ESP must be set to the same speed AT+UART_DEF=9600,8,1,0,1
  // initialize ESP module
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

  printWifiStatus();

  Serial.println();
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  if (client.connect(webserver, webport)) {
  //if (client.connectSSL(webserver, webport)) {
    Serial.println("Connected to server");
    // Make a HTTP request
    client.println("GET /test.txt HTTP/1.1");
    client.println("Host: www.miklik.cz");
    client.println("Connection: close");
    client.println();
  }
  else {
    Serial.println("Client not connected");
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();

    // do nothing forevermore
    while (true);
  }
}
