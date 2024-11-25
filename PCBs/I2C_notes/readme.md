## Stephen I2c notes
### 1. I2c wiring
![image](TRRS%20i2c.png)
| Contact # | TRRS | Wire color* | I2c role |
| --------- | ---- | ----------- | -------- |
| 1         | T    | Red         | SDA      |   
| 2         | R    | Black       | SCL      |   
| 3         | R    | Yellow      | Power+   |   
| 4         | S    | Green       | Ground   |   

*Wire colors may not be standardized

Please ensure the plug is pushed all the way in, as misalignment can cause Power+ and Ground to be shorted together.

### 2. Arther's I2c wiring (not compatible, for reference only!)
| Contact # | TRRS | Wire color* | I2c role |
| --------- | ---- | ----------- | -------- |
| 1         | T    | Red         | Power+   |   
| 2         | R    | Black       | Ground   |   
| 3         | R    | Yellow      | SCL      |   
| 4         | S    | Green       | SDA      |   

#### Design for an Arther-to-Stephen adapter
| Arthur side | Jack position | Plug position | Stephen Side |
| ----------- | ------------- | ------------- | ------------ |
| Ground      | inner left    | 4S            | Ground       |
| Power+      | inner right   | 3R            | Power+       |
| SCL         | inner top     | 2R            | SCL          |
| SDA         | Outter shield | 1T            | SDA          |
