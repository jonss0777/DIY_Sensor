/*
This is an extension of the NGENS Air Quality Sensor Project. It includes support for the  
DHT11 sensors. It also includes a website that can be hosted on github pages to visualize the collected data. 
It does this by leveraging Google Sheets and AppScripts as a server. 

Sensor Information: 
DHT11 Humidity and Temperature

  Info:
  - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
  - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
*/

#include <OneWire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <string>
#include <Wire.h>                
#include <WiFi101.h>

#define DHTPIN 12    
#define DHTTYPE DHT11  


bool Sensor_DHT11 = 1;
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
WiFiSSLClient client; 


int status = WL_IDLE_STATUS;
int samplingPeriod = 3; // in seconds

struct ServerInfo{
  String server_google_script;
  String server_google_usercontent;
};

struct WiFiInfo{
  String ssid;
  String passcode;
  String gsid;
};

struct SensorData {
  String temp_DHT11_sensor;
  String humidity_DHT11_sensor;
};

ServerInfo server_info;

WiFiInfo wifi_info;

SensorData sensor_data;


void setup() {
  Serial.begin(9600);
  while (!Serial);
  WiFi.setPins(8, 7, 4, 2);
  server_info.server_google_script = "script.google.com"; 
  server_info.server_google_usercontent = "script.googleusercontent.com"; 
  AP_getInfo();

  String header = "{\"command\":\"addHeader\", \"header\":[";
  if (Sensor_DHT11)
  {
    initializeDHT11();
    header.concat("\"temp_DHT11_sensor\",");
    header.concat("\"humidity_DHT11_sensor\"");
  }
  header.concat("]}");

  connectToWiFi();
  initializeClient(server_info.server_google_script);
  String response = httpPost(header, server_info.server_google_script);
  Serial.println("Sensor starts collecting data...");
}

void loop() {
  String payload = "{\"command\":\"addRow\", \"data\":[";
  bool firstItem = true;

  if (Sensor_DHT11) 
  {
    runDHT11(); 
     if (!firstItem) payload.concat(",");
    payload.concat("\"" + sensor_data.temp_DHT11_sensor + "\"");
    firstItem = false;
    
    if (!firstItem) payload.concat(",");
    payload.concat("\"" + sensor_data.humidity_DHT11_sensor + "\"");
    firstItem = false;
  }

  payload.concat("]}");

  // Are we still connected to WiFi
  String response;
  if (status != WL_CONNECTED){
    connectToWiFi();
  } else{
    // Connected to the correct script server
    initializeClient(server_info.server_google_script);
    response = httpPost(payload, server_info.server_google_script);
  }
  delay(1000*samplingPeriod);
 
}
