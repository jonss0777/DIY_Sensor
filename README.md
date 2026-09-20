# DIY Sensor

Components you will need:
 - Micro-controller  ESP32 
 - Sensor            DHT11 Keys Temperature and Humidity Sensor
 - Raspberry Pi      Mosquito Server
 - Services          AWS Time stream

### Architechture 
A[ESP32 Sensor] -->| Send data to topic in Mosquito Server

B[MQTT Client] -->| Mosquito Server

D[AWS Timestream] --> | Store sensor data


### Benefits:
- Mosquito server works as a buffer to prevent data lost if AWS service is unreacheable.

- Writing directly to AWS for every single sensor reading can accumulate API request costs and can trigger rate limmits if scale to a donze of sensors. The Rasberry running the Mosquito server can buffer and bacth the sensor readings before sending them to AWS Timestream.

- Performing TLS handshakes, holding SSL certificates, and waiting for round-trip WAN latencies to AWS servers takes significant processing power and time on an ESP32.


### Updates
- 2/24/2026 - Adding C to F button. Adding black and white mode. 

- 9/19/2026 - Rewriting DTH.h from Arduino code to ESP32 framework code.
