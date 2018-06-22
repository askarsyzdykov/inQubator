/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com
*********/

// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"
#include <EEPROM.h>

// Uncomment one of the lines below for whatever DHT sensor type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Replace with your network details
const char* ssid = "ssid";
const char* password = "password";

int serverPortAddr = 0;
int serverPort = 0;

// DHT Sensor
const int DHTPin = 14;
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE, 15);

// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);

  serverPort = EEPROMReadInt(serverPortAddr);
  Serial.print("ServerPort = ");
  Serial.println(serverPort);
  if (serverPort == 0) {
    EEPROMWriteInt(serverPortAddr, 45);
    //EEPROM.write(serverPortAddr, 40005);
    delay(1000);
    //serverPort = EEPROMReadInt(serverPortAddr);
    Serial.print("New value for ServerPort = ");
    Serial.println(EEPROMReadInt(serverPortAddr), HEX);
  }

  dht.begin();

  // Connecting to WiFi network
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

  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
}

// runs over and over again
void loop() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  Serial.print("Humidity: ");
  Serial.println(h);
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  Serial.print("Temperature: ");
  Serial.println(t);

  String data = "IncubatorId=1025&Temperature=" + String(h) + "&Humidity=" + String(t);
  //
  HTTPClient http;

  http.begin("http://put.your.server.url.here");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Content-Length", String(data.length()));
  int httpCode = http.POST(data);
  //http.writeToStream(&Serial);
  if (httpCode > 0) {
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  //

  delay(60000);
}

void EEPROMWriteInt(int address, int value) {
  byte two = (value & 0xFF);
  byte one = ((value >> 8) & 0xFF);

  EEPROM.write(address, two);
  EEPROM.write(address + 1, one);
}

int EEPROMReadInt(int address) {
  long two = EEPROM.read(address);
  long one = EEPROM.read(address + 1);

  return ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
}
