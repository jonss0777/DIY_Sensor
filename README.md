# DIY Sensor

Components you will need:
 - ESP32 
 - DHT11 Keys Temperature and Humidity Sensor
 - Services AppScript or HiveMQ

Wiring:


### Architechture HTTP

```mermaid
  flowchart TD
    A[ESP32 Sensor] -->|Send data to topic in Mosquito| B()
    C[MQTT Client] -->|Mosquito Server|B()
    D[AWS Timestream] --> |Store sensor data| C()
  
```

// Updates
- 2/24/2026 - Adding C to F button. Adding black and white mode. 

- 9/19/2026 - Rewriting DTH.h from Arduino code to ESP32 framework code.
