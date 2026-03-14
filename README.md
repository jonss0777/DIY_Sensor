# DIY Sensor

Components you will need:
 - Micro-controller  ESP32 
 - Sensor            DHT11 Keys Temperature and Humidity Sensor
 - Services          AppScript

### Architechture HTTP

```mermaid
  flowchart TD
    A[Micro controller] -->|Sends post request to AppScript| B(App Script Webserver handles the request by storing the information in a sheet and seding a success response.)

    C[HTTP Client] -->|Send get request to AppScript|B
  
```


### Walkthrough
<img src='https://github.com/jonss0777/DIY_Sensor/blob/a51370c433fa4a4c2cd80e032c79ffe325e2ca9e/WebsiteGif.gif' title='Video Walkthrough' width='' alt='Video Walkthrough' />

### Updates
2/24/2026 - Adding C to F button. Adding black and white mode. 





