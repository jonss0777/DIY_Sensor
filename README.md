# DIY Sensor Architechture


<details>
<summary>DIY Sensor Data Pipeline</summary>

```mermaid
  flowchart TD
    subgraph Sensor
        A1[Sensor Collects Data]
    end

    subgraph Google Cloud
        B1[App Script Receiveces Request] --> B2[Google Sheets]
    end

    subgraph Local
        C1[Fetch from AppScript] --> C2[UI DIY Sensor]
    end

    A1 --> |HTTP POST Request|B1
    B1 --> |HTTP POST Request|C1

```
</details>
