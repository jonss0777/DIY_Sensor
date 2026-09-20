# Temperature and Humidity Sensor

Host your local temperature and humidity sensor. Store the data in AWS timestream and visualiaze it. 


Components you will need:
 - Micro-controller  ESP32 
 - Sensor            DHT11 Keys Temperature and Humidity Sensor
 - Raspberry Pi      Mosquito Server
 - Services          AWS Time stream

### Architechture 
A[ESP32 Sensor] -->| Send data to topic in Mosquito Server

B[MQTT Server] -->| Send queued data to AWS timestream

D[AWS Timestream] --> | Store sensor data


### Benefits:
- Mosquito server works as a buffer to prevent data lost if AWS service is unreacheable.

- Writing directly to AWS for every single sensor reading can accumulate API request costs and can trigger rate limmits if scale to a donze of sensors. The Rasberry running the Mosquito server can buffer and bacth the sensor readings before sending them to AWS Timestream.

- Performing TLS handshakes, holding SSL certificates, and waiting for round-trip WAN latencies to AWS servers takes significant processing power and time on an ESP32.


### Set Up Mosquito Server in Rasberri Pi

**Debian 13(Trixie)**

Install Mosquito Service

```
sudo apt update

sudo apt install -y mosquitto mosquitto-clients

systemctl status mosquitto 
```

Configure Username and Password Authentication

```
sudo mosquitto_passwd -c /etc/mosquitto/passwd myusername
```

You'll be promted to type a password


To verify success run

```
 ls -l /etc/mosquitto/passwd
```

If there is code=exited status=13 error. You'll have to change the permission access to /etc/mosquitto/passwd

```
sudo chown -R mosquitto:mosquitto /etc/mosquitto/
sudo chmod 640 /etc/mosquitto/passwd

sudo systemctl restart mosquitto
```

Check if another service is using the same port 

```
sudo ss -tulpn | grep 1883
```

Test broker use a different terminal for each command

```
mosquitto_pub -h localhost -t "my/test/topic" -u "myusername" -P "yourpassword" -m "Hello World"
```

```
mosquitto_sub -h localhost -t "my/test/topic" -u "myusername" -P "yourpassword"
```



### Updates

- 9/19/2026 - Rewriting DTH.h from Arduino code to ESP32 framework code.

- 9/20/2026 - Set up Mosquito broker in Raspberry Pi.


