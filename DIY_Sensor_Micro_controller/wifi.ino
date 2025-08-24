/*!
* @brief Makes an http get request and handles the response message by calling handleResponse()
* @param
**/
String httpGet(String server, String url) {
  client.println("GET " + url + " HTTP/1.1");
  client.println("Host: " + String(server));
  client.println("Connection: close");
  client.println();

  String response = "";
  unsigned long lastRead = millis();
  const unsigned long timeout = 5000; // 5 seconds

  while (client.connected() || client.available()) {
    if (client.available()) {
      char c = client.read();
      response += c;
      lastRead = millis(); // reset timer on data received
    }
    if (millis() - lastRead > timeout) { Serial.println("Response timeout. Exiting read loop."); break; }
  }
  return response;
}


/*!
* @brief Makes an http post request and handles the response message by calling handleResponse()
* @param
**/
String httpPost(String payload, String server) {
  client.println(String("POST /macros/s/") + String(wifi_info.gsid) + String("/exec HTTP/1.1"));
  client.println("Host: "+ String(server));
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.print("Content-Length: ");
  client.println(payload.length());
  client.println();
  client.print(payload);
  String response = "";

  // This approach was selected because it allowed the sensor to escape instances where a package never arrives completely.
  // This approach also gives the sensor enough time to receive the message. (Still needs to be tested for instances where the internet connection is really poor and the
  // request-response cycle between the server and the sensor takes longer than 3 seconds.)

  unsigned long lastRead = millis();
  const unsigned long timeout = 5000; // 5 seconds

  while (client.connected() || client.available()) {
    if (client.available()) {
      char c = client.read();
      response += c;
      lastRead = millis(); // reset timer on data received
    }
    if (millis() - lastRead > timeout) { Serial.println("Response timeout. Exiting read loop."); break; }
  }  
  return response;
}

/*!
* @brief 
* @param
**/
void handleResponse(String response) {
  Serial.print(response);
  if (response.indexOf("200 OK") != -1) {
    // payload format
    // payload:val1,val2,val3\n
    int pIndex = response.indexOf("payload:");

    /*!  
    *  TODO: Create logic to access more than one val
    *
    */
    if (pIndex != -1) {
      Serial.println("payload:");
      Serial.println(response.substring(pIndex));
      int start = pIndex + 8; // skip payload:
      int end = response.indexOf('\n', start); 
      String prateStr = response.substring(start, end);
      samplingPeriod = prateStr.toInt();
      Serial.print("Sampling Period: ");
      Serial.println(samplingPeriod);
    }
  }
  else if (response.indexOf("302 Moved Temporarily") != -1) {
  
    int locIndex = response.indexOf("Location: ");
    if (locIndex != -1) {
      int endIndex = response.indexOf('\n', locIndex);
      if (endIndex == -1) endIndex = response.length();  
      String locationURL = response.substring(locIndex + 9, endIndex);
      locationURL.trim(); 
      int pathIndex = locationURL.indexOf(".com");
      if (pathIndex == -1) {
         Serial.println("Invalid URL format");
        return;
      }
      Serial.println("Location URL: ");
      Serial.println(locationURL);
      initializeClient(server_info.server_google_usercontent);
      httpGet(server_info.server_google_usercontent, locationURL.substring(pathIndex + 4));
    } else {
      Serial.println("No Location header found.");
    }
  }
  else{
    Serial.println("Unrecognized response code");
  }
}


void connectToWiFi(){
  // Check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(wifi_info.ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(wifi_info.ssid, wifi_info.passcode);

    // wait 5 seconds for connection:
    delay(5000);
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");
  Serial.println(wifi_info.ssid);
} 


/*!
* @brief Starts SSL connection to server
* @param 
*/
void initializeClient(String server) {
   delay(200);
  if (client.connectSSL(server.c_str(), 443)) {     
    Serial.println("Connected to ");
    Serial.println(server);
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
    Serial.println();
  }
  else {
    Serial.print("Not connected to ");
    Serial.println(server);
  }
}