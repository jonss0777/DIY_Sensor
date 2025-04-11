# Streaming sensor data into PostgreSQL


## Vaisala Stream API
```mermaid
graph LR;
  id1(VaisalaStreamAPI) --> id2(NodeRED);   A@{ shape: braces, label: "Comment" }
  id2 --> id3[(PostgreSQL)];  A@{ shape: braces, label: "Comment" }


```




## AppScript Stream

```mermaid
graph LR;
  id1(AppScript) --> id2(NodeRED);
  id2 --> id3[(PostgreSQL)];

```








