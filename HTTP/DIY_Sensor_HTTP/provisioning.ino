const char webpage_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>
    DIY Air Quality Sensor Provisioning
  </title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<style>
  body {
    display: flex;
    justify-content: center;
    /* Centers horizontally */
    align-items: center;
    /* Centers vertically */
    height: 100vh;
    background-color: #cccccc;
    font-family: Arial, Helvetica, Sans-Serif;
    font-size: 27px;
    color: #000088;
  }
  #main_container {
    font-size: 16;
    background-color: #ffffff;
    padding: 20px;
  }
</style>
<body>
  <div id="main_container">
    <h1>DIY Air Quality Sensor</h1>
    <form action="/get">
      ssid: <input type="text" name="SSID"><br>
      passcode: <input type="password" name="passcode"><br>
      gsid: <input type="text" name="GSID"><br>
      <input type="submit" value="Submit">
    </form>
  </div>
</body>
</html>)rawliteral";


/*!
*   @brief Decodes url encoded values 
*   @details Solves the case where $ or other uncommon characters are used in a wifi password 
*/
String urlDecode(String input) {
  String decoded = "";
  char temp[] = "0x00"; // to hold the hex value

  for (unsigned int i = 0; i < input.length(); i++) {
    if (input[i] == '%') { // If a '%' is found, decode the following two hex characters
      if (i + 2 < input.length()) {
        temp[2] = input[i + 1];
        temp[3] = input[i + 2];
        decoded += (char) strtol(temp, NULL, 16); // Convert hex to character
        i += 2;
      }
    } 
    else if (input[i] == '+') { decoded += ' ';} 
    else { decoded += input[i]; }
  }
  return decoded;
}


/*!
*   @brief  Prints information about the WiFi, the micro controller is currently, connected to. 
*/

void printWiFiStatus() {
  Serial.print(F("SSID: "));
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print(F("IP Address: "));
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print(F("signal strength (RSSI):"));
  Serial.print(rssi);
  Serial.println(F(" dBm"));

  
}

/*!
*   Prints the micro controllers mac address
*   @param mac     - an array containing the microcontrollers mac address
*/

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16)
      Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i > 0)
      Serial.print(":");
  }
  Serial.println();
}

/**
*   Makes AP and, when client connected, serves the 
*   web page with entry fields. The fields are 
*   returned in the parameters.
*   
*   @param ssid     - a String to place the ssid
*   @param passcode - a String to place the passcode
*   @param gsid     - a String to place the gsid
*/
void AP_getInfo() {
  WiFiServer server(80);
  WiFiClient client;
  Serial.println(F("Access Point Web Server"));

  status = WiFi.status();
  if (status == WL_NO_SHIELD) {
    Serial.println(F("WiFi shield not present"));
    while (true);
  }

  makeMACssidAP("DIY_Air_Quality_Sensor");
  delay(1000);
  printWiFiStatus();

  while (true) {  
   
    if (status != WiFi.status()) { 
      status = WiFi.status();
      if (status == WL_AP_CONNECTED) {
        byte remoteMac[6];
        Serial.print(F("Device connected to AP, MAC address: "));
        WiFi.APClientMacAddress(remoteMac);
        printMacAddress(remoteMac);

        Serial.println(F("Starting server"));
        server.begin();
        // print where to go in a browser:
        IPAddress ip = WiFi.localIP();
        Serial.print(F("To provide provisioning info, open a browser at http://"));
        Serial.println(ip);
        Serial.println(ip);
      } else {
        // a device has disconnected from the AP, and we are back in listening mode
        Serial.println(F("Device disconnected from AP"));
        
        client.stop();
      }
    }

    client = server.available();  

    if (client) {                   
      Serial.println(F("new client")); 
      String currentLine = "";      
      while (client.connected()) {  

        if (client.available()) { 
          char c = client.read();  
          Serial.write(c);         

          if (c == '\n') {  
           
            if (currentLine.length() == 0) {
              client.println(F("HTTP/1.1 200 OK"));
              client.println(F("Content-type:text/html"));
              client.println();
              client.print(webpage_html);  
              client.println();
              break;                   
            } else {                                   
              if (currentLine.startsWith("GET /get?")) {  
              
                wifi_info.ssid = urlDecode(currentLine.substring(currentLine.indexOf("SSID=") + 5, currentLine.indexOf("passcode=") - 1));
                wifi_info.passcode = urlDecode(currentLine.substring(currentLine.indexOf("passcode=") + 9, currentLine.indexOf("GSID=") - 1));
                wifi_info.gsid = urlDecode(currentLine.substring(currentLine.indexOf("GSID=") + 5, currentLine.indexOf(" HTTP")));

                client.stop();
                Serial.println("client disconnected\n");
                WiFi.end();
                delay(5000);
                status = WiFi.status();
                //storeinfo(wifi_info.ssid, wifi_info.passcode, wifi_info.gsid);
                return; 
              }
              currentLine = ""; 
            }
          } else if (c != '\r') {  
            currentLine += c;     
          }
        }  
      }    
      client.stop();
      Serial.println(F("Client disconnected"));

    }  
  }    
}

/**
*   Create an AP with a unique ssid formed with a string
*   and the last 2 hex digits of the board MAC address.
*   By default the local IP address of will be 192.168.1.1
*   you can override it with the following:
*   WiFi.config(IPAddress(10, 0, 0, 1));
*
*   @param startString a string to preface the ssid
*/
void makeMACssidAP(String startString) {

  byte localMac[6];

  Serial.print(F("Device MAC address: "));
  WiFi.macAddress(localMac);
  printMacAddress(localMac);

  char myHexString[3];
  sprintf(myHexString, "%02X%02X", localMac[1], localMac[0]);
  // EDIT LOCAL SERVER NAME BY CHANGING SSID
  String ssid = startString + String((char *)myHexString);

  Serial.print(F("Creating access point: "));
  Serial.println(ssid);
  status = WiFi.beginAP(ssid.c_str());

  if (status != WL_AP_LISTENING) {
    Serial.println(F("Creating access point failed"));
    while (true);
  }
}