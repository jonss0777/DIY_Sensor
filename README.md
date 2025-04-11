# Streaming sensor data into PostgreSQL


## Vaisala Stream API
```mermaid
graph LR;
  id1(VaisalaStreamAPI) --> |Process data|id2(NodeRED);   A@{ shape: braces, label: "Comment" }
  id2 --> id3[(PostgreSQL)];
  


```




## AppScript Stream

```mermaid
graph LR;
  id1(AppScript) --> id2(NodeRED);
  id2 --> id3[(PostgreSQL)];

```








