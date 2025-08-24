# DIY Sensor Architechture

```mermaid
  flowchart TD
    subgraph Sensor
        A1[Sensor Collects Data]
    end

    subgraph Google Cloud
        B1[App Script receiveces a Post request from micro-controller/  from UI] <--> B2[Google Sheets]
       
    end

    subgraph Hosted on Github
        C1[Response from AppScript API] <--> C2[Display Data in UI for DIY Sensor]
    end

    A1 --> |HTTP POST Request|B1
    B1 <--> |HTTP POST Request|C1

```

Components you will need:
 - Adafruit M0 feather micro controller with WiFi shield 
 - DHT11 Keys Temperature and Humidity Sensor


Reference: 
 - [Google Sheets API](https://developers.google.com/workspace/sheets/api/guides/concepts)
 - [Google AppScript API](https://developers.google.com/apps-script/api/reference/rest)
 - [Mozilla HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP)
