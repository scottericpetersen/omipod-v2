/*
 *  This is work-in-progress proof-of-concept code for a many-to-many via broker network topology. Here we assume:
 *  1. Each pod will use this code (Arduino). You must edit the String podname line to indicate a different pod name fomatted as "/pod1", "/pod2", etc
 *  2. The broker.py code (Python3) is running on a computer whose IP is entered below in the udpAddress line and that must contain the /pod(s) entered here.
 *  3. The client.scd code is running (SuperCollider) correctly (see the setup in the client.scd document) 
 *
 */
#include <WiFi.h>
#include <WiFiUdp.h>

unsigned long prevTime = 0;
unsigned long curTime;
const long del = 250;  // every 250 milliseconds, send values


// -- SETUP -- MUST BE EDITED FOR NETWORK IP AND POD NUMBER

// -- Edit the podname variable and be sure that name appears in the broker.py document in the list of pods. If necessary, add it there.
String podname = "/pod1";

// WiFi network name and password. For a network that does not require a password, leave an empty string.
const char* networkName = "yale wireless";  // ESP32 Devkits cannot connect to 5G wireless... must use 2 GHz
const char* networkPswd = "";

// -- IP address to send UDP data to: either use the ip address of the server or a network broadcast address
//const char * udpAddress = "10.0.0.137"; // double check the IP address of the machine running the Python script
const char* udpAddress = "192.168.1.2";  // double check the IP address of the machine running the Python script
const int udpPort = 5001;                // double check the port too :)

// -- END SETUP --

// Boolean to store connection status
boolean connected = false;

//The udp library class
WiFiUDP udp;

byte command[27] = { 0x20, 0x00, 0x00, 0x00, 0x16, 0x02, 0x62, 0x3A, 0xD5, 0xED, 0xA3, 0x01, 0xAE, 0x08, 0x2D, 0x46, 0x61, 0x41, 0xA7, 0xF6, 0xDC, 0xAF, 0xD3, 0xE6, 0x00, 0x00, 0x1E };

void setup() {
  // Initilize hardware serial:
  Serial.begin(115200);

  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
}

void loop() {

  curTime = millis();

  if (curTime - prevTime >= del) {
    prevTime = curTime;
    writeUDP(curTime);
  }
}

void connectToWiFi(const char* ssid, const char* pwd) {
  delay(5);
  Serial.println("Connecting to WiFi network: " + String(ssid));
  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:  // updated to reflect current IDE V2 WiFiEvent events
      //When connected set
      Serial.print("WiFi connected! IP address: ");
      Serial.println(WiFi.localIP());
      //initializes the UDP state
      //This initializes the transfer buffer
      udp.begin(WiFi.localIP(), udpPort);
      connected = true;
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:  // updated to reflect current IDE V2 WiFiEvent events
      Serial.println("no WiFi connection");
      connected = false;
      break;
  }
}

void writeUDP(int time) {

  int s_light = analogRead(4);
  int s_other = analogRead(5);
  String send_data = podname + " " + time + " " + String(s_light) + " " + String(s_other);
  // only send data when connected -- could this cause the delay?
  if (connected) {
    // Serial.println("Sent: " + send_data);  // Uncomment to monitor what is being sent
    udp.beginPacket(udpAddress, udpPort);
    udp.write((const uint8_t*)send_data.c_str(), send_data.length());
    udp.endPacket();
  }
}