# Streaming sensor data into PostgreSQL


## Vaisala Stream API
```mermaid
graph TD;
  id1(VaisalaStreamAPI) --> |Cache until a datetime row is full|id2(NodeRED);   
  id2 --> |Send row with 8 datapoints|id3[(PostgreSQL)];
```






## AppScript Stream

```mermaid
graph TD;
  id1(AppScript) --> |Process data|id2(NodeRED);
  id1 --> |Two|id4(Google Sheet);
  id2 --> |One|id3[(PostgreSQL)];
  
```
```








