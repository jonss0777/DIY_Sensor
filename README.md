# Streaming sensor data into PostgreSQL

## Vaisala Stream API
```mermaid
  flowchart TD
    subgraph Vaisala
        A1[Measurement Stream API]

    end

    subgraph Local
        B1[Node-RED] --> B2[PostgreSQL]

    end

    A1 -->|HTTP GET Response |B1


```






## Google Cloud Stream

```mermaid
  flowchart TD
    subgraph Sensor
        A1[Sensor Collects Data]
    end

    subgraph Google Cloud
        B1[App Script Receiveces Request] --> B2[Google Sheets]
    end

    subgraph Local
        C1[Node-RED] --> C2[PostgreSQL]
    end

    A1 --> |HTTP POST Request|B1
    B1 --> |HTTP POST Request|C1

```

### References:
  + [Meassurement Stream API](https://api-catalog.eu.platform.xweather.com/docs/Measurement%20stream/stream-api)
  + [Undestanding Scopes Node Red](https://nodered.org/docs/user-guide/context)
  + [Google Sheets Doc](https://developers.google.com/apps-script/reference/spreadsheet/sheet)











