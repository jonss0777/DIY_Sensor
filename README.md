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

    A1 -->|Data Stream| B1


```






## AppScript Stream

```mermaid
  flowchart TD
    subgraph Sensor
        A1[Sensor Collects Data]
    end

    subgraph AppScript
        B1[App Script Receivces Request]
    end

    subgraph Local
        C1[Node-RED] --> B2[PostgreSQL]
    end

    A1 --> B1
    B1 --> C1

```

### References:
  + [Meassurement Stream API](https://api-catalog.eu.platform.xweather.com/docs/Measurement%20stream/stream-api)











