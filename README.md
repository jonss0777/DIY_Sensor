# Streaming sensor data into PostgreSQL


## Vaisala Stream API
```mermaid
graph LR;
  id1(VaisalaStreamAPI) --> |Cache until the for a date time is full|id2(NodeRED);   
  id2 --> |Send row with 8 datapoints|id3[(PostgreSQL)];
  


```




## AppScript Stream

```mermaid
graph LR;
  id1(AppScript) --> id2(NodeRED);
  id2 --> id3[(PostgreSQL)];

```








