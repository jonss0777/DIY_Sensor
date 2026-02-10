
# DIY Sensor 
### Architechture HTTP

```mermaid
  flowchart TD
    A[Micro controller] -->|Sends publish to topic| B(HiveMQ Server receiveces a published to "topic" from micro-controller)
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
        B1[HiveMQ Server receiveces a published to topic from micro-controller] <--> B2[Google Sheets]
       
    end

    subgraph Hosted on Github
        C1[Response from ] <--> C2[Display Data in UI for DIY Sensor]
    end

    [Subscribe to topic] -> Display live time Sensor Data]
```
   
Reference: 
 - [HiveMQ Guide](https://docs.hivemq.com/hivemq/latest/user-guide/index.html)




