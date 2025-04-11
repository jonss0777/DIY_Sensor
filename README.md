# Streaming sensor data into PostgreSQL


## Vaisala Stream API
```mermaid
  flowchart TD
    subgraph Process A
        A1[Vaisala] --> A2[Do something in A]
    end

    subgraph Process B
        B1[Node Red] --> B2[Cache the data until a row is full]
        
    end



    A2 --> B1

```






## AppScript Stream

```mermaid

```
```








