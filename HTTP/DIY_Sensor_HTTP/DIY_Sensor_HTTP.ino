#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <secrets.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11
// WiFi Credentials
// const char* ssid = "";
// const char* password = "";

// // AppScript Endpoint
// const char* endpoint = "";

WiFiClientSecure secureClient;
HTTPClient http;
int currPeriod = 2000;  // 2 seconds
DHT dht(DHTPIN, DHTTYPE);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (500)
char msg[MSG_BUFFER_SIZE];


// Google Certificate
//const char* google_root_ca = "";


String getSensorReading(){
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
snprintf(msg, MSG_BUFFER_SIZE, "{\"command\":\"addData\", \"data\":[%.2f, %.2f, %.2f]}", h, t, f);  return msg;
}


void connectToWIFI() {
  // Set WiFi to Station mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected to the WiFi network");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());  // Print local IP
}

void doPost(String payloadData) {
  http.begin(secureClient, endpoint);
  http.addHeader("Content-Type", "application/json");
  int code = http.POST(payloadData);
  String response = http.getString();
  Serial.println("POST response code: " + String(code));
  Serial.println("POST response: " + response);
  http.end();
}


void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;  // Wait for serial port to connect
  }
  Serial.println("Serial Connected!");
  connectToWIFI();
  secureClient.setCACert(google_root_ca);
  // Add Header
doPost("{\"command\": \"addHeader\", \"header\":[\"Humidity\", \"Temperature C\", \"Temperature F\"]}");}

void loop() {
  // Get sensor reading and Post sensor reading 
  doPost(getSensorReading());               
 
  // Wait 1 minute 
  delay(60000);
}