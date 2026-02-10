
# DIY Sensor 
### Architechture HTTP

```mermaid
  flowchart TD
    subgraph Sensor
        A1[Sensor Collects Data]
    end

    subgraph Google Cloud
        B1[App Script Server receiveces a Post request from micro-controller] <--> B2[Google Sheets]
       
    end

    subgraph Hosted on ***
        C1[Response from AppScript WebApp] <--> C2[Display Data in UI for DIY Sensor]
    end

    [HTTP POST Request -> Display sensor data in UI]
```

Components you will need:
 - ESP32 
 - DHT11 Keys Temperature and Humidity Sensor
 - Services AppScript or HiveMQ


### Architechture MQTT

```mermaid
  flowchart TD
    subgraph Sensor
        A1[Sensor Collects Data]
    end

    subgraph HiveMQ Server
        B1[HiveMQ Server receiveces a published to "topic" from micro-controller] <--> B2[Google Sheets]
       
    end

    subgraph Hosted on Github
        C1[Response from ] <--> C2[Display Data in UI for DIY Sensor]
    end

    [Subscribe to "topic" -> Display live time Sensor Data]
```
   
Reference: 
 - [HiveMQ Guide](https://docs.hivemq.com/hivemq/latest/user-guide/index.html)




