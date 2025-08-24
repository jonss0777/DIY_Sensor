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
        C1[Node-RED] --> C2[PostgreSQL]
    end

    A1 --> |HTTP POST Request|B1
    B1 --> |HTTP POST Request|C1

```
</details>
