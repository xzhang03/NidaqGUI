# All about Nanosec Config Files

## Basic configs
Where data will be stored (make sure this path exists):
```matlab
nicfg.BasePath = 'C:\Users\myuser\OneDrive\Documents\MATLAB\photometrydata'; 
```

Serial port of nanosec. This number will be converted to "COM36".
```matlab
nicfg.ArduinoCOM = 36;
```

Record running or not. Generally set to true.
```matlab
nicfg.RecordRunning = true;
```
