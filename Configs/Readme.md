# All about Nanosec Config Files v3.5

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

### 4. Picodaq (Nidaq replacement)
All the picodaq settings are paramterized in a single cell like this. Picodaq has 4 analog channels and 16 digital channels, and is optimize for 2500 Hz sampling rate. 
```matlab
nicfg.picoDAQparams = {'daqcom', 'COM28', 'frequency', nicfg.NidaqFrequency};
```
* daqcom: Serial port for picodaq
* frequency: Data rate, usually 2500 Hz
* databuffer: Number of bytes to send at a time. Picodaq does not upload data continuously, but rather in packets to save processing power. Each data ponit is 6 bytes, so this number must be multiples of 6. By default, it's 7500 bytes, which is 1250 datapoints, meaning 0.5 s if the datarate is 2500 Hz.
* fda: Signal suppression before ADC. This is a defined by the resistors in the fully differential amplifier circuit. It should be 8.
* adcfreqmod: Picodaq adc works at 32 kHz at max, so we lower its sampling rate to increase signal-to-noise ratio. By default it's 2, meaning 32kHz / 2^2 = 8 kHz.
* adcsmooth: Addtional averaging of adc data, if ADC converts at 8 kHz, we can smooth by a factor of 2 to make it 4 kHz, which is still fast enough for 2500 Hz sampling rate.
* Please keep the rest of the parameters unchanged.

## B. Nanosec
This is where the extended nanosec settings are.

### 1. General
Master enable.
```matlab
nicfg.omnibox.enable = true;
```

### 2. Two-color photometry (TCP)
Change this to true to enable tcp mode (must set optophotometry and same-color optophotometry to false).
```matlab
nicfg.tcp.enable = false;
```

Behavioral inter-trial interval (in seceonds, from trial start to trial start). Since there is no optogenetic stimulation in tcp mode. Behavior experiments are on their own schedule and this parameter sets how often the behavioral task occurs, e.g., 10 means one trial every 10 s. Without scheduler, the experiment will just repeat forever. With scheduler, the behavioral experiment won't start until schedule tells it to (see scheduler below).
```matlab
nicfg.tcp.behaviorcycle = 10;
```

These 2 values change how often the two pulses of tcp occur. It's in increments of 100 us, so 100 means 100 * 100 us = 10 ms. By default, pulse 1 happens happens 10 ms after pulse 1 happens, and pulse 2 happens 10 ms after pulse 1 happens. That's why 100/100 means interleaved 50 Hz pulsing in both channels. It is the sum of the two numbers that determines the length of a pulse1-pulse2 cycle (e.g., 100/100 means 20 ms cycle). Some of the photoemtry/behavioral timing mechanisms are done by counting cycles, so changing this number may affect them. I tried to account for cycle changes, but you may still need to let me know if some timings are off because of this.
```matlab
nicfg.tcp.pulsecycle1 = 100;
nicfg.tcp.pulsecycle2 = 100;
```
