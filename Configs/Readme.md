# All about Nanosec Config Files

## A. Basic configs
### 1. General
Where data will be stored (make sure this path exists):
```matlab
nicfg.BasePath = 'C:\Users\myuser\OneDrive\Documents\MATLAB\photometrydata'; 
```

Logging channel names for automated parsing or general notes (not essential)
```matlab
nicfg.ChannelNames  = {'PD1', 1, 'Ch1in', 2, 'camera', 3, 'ensure', 4, 'PD2', 5, 'lick', 6, 'Ch2in', 7, 'Buzz', 8}; 
nicfg.DigitalChannelNames = { };
```

Lab-specific server uploading (not essential)
```matlab
nicfg.serverupload = false;
nicfg.serveradd = {'SZ', '\\anastasia\data\photometry'};
```

Pre-filling mouse name when nanosec starts (not essential)
```matlab
nicfg.MouseName = 'SZ00';
```

### 2. Nanosec (basic, more below)
Serial port of nanosec. This number will be converted to "COM36".
```matlab
nicfg.ArduinoCOM = 36;
```

Nanosec variables that you don't often change. Generally it's fine to recording running no matter what and baumrate of 19200 is good enough.
```matlab
nicfg.RecordRunning = true;
nicfg.baumrate = 19200; 
```

### 3. Nidaq
Nidaq name, which you can see and change in nimax. It doesn't affect picodaq.
```matlab
nicfg.NidaqDevice = 'Dev2';
```

Number of nidaq channels, analog and digital. These numbers do not affect picodaq, which has fixed number of channels (4 analog + 16 digital), but if you set Nidaqchannels (analog) to 0, nanosec ui will not try to initialize any digitizer. You need a higher end (e.g., 6300) nidaq to be able to do simultaneous analog and digital recordings, so most likely digital channels will be set to 0.
```matlab
nicfg.NidaqChannels = 8;       
nicfg.NidaqDigitalChannels = 0; 
```

Nidaq variables that you don't change often. 2500 Hz recording is good enough for photometry and can be lowered for lower datarate applications. Monkeylogic library could be used if not having data acquisition toolbox. Leave digital string unchanged unless you are using different ports.
```matlab
nicfg.NidaqFrequency = 2500;
nicfg.useMLlibrary = false;
nicfg.DigitalString = 'Port0/Line';
```

Analog input modes. Could be a single string to apply the same mode to all channels or use a cell to specify channel-by-channel. Choices are 'Differential', 'SingleEnded', 'SingleEndedNonReferenced'. Please note that the higher number channels are generally single-ended only. This is Nidaq only.
```matlab
nicfg.AImode = 'SingleEnded';
```
or
```matlab 
nicfg.AImode = {0, 'Differential'; 1, 'Differential'; 2, 'Differential'; 3, 'Differential';....
                4, 'Differential'; 5, 'Differential'; 6, 'SingleEnded'; 7, 'SingleEnded';...
                14, 'SingleEnded'; 15, 'SingleEnded'};
```

### 4. picodaq
All the picodaq settings are paramterized in a single cell.
