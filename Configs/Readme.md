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
![TCP](https://github.com/xzhang03/NidaqGUI/raw/master/Schemes/TCP.png)
TCP is done by pulses two LEDs in an interleaved manner. TCP operates in 20 ms cycles (50 Hz) by default.

Change this to true to enable tcp mode (must set optophotometry and same-color optophotometry to false).
```matlab
nicfg.tcp.enable = false;
```

Behavioral inter-trial interval (in seceonds, from trial start to trial start). Since there is no optogenetic stimulation in tcp mode. Behavior experiments are on their own schedule and this parameter sets how often the behavioral task occurs, e.g., 10 means one trial every 10 s. Without scheduler, the experiment will just repeat forever. With scheduler, the behavioral experiment won't start until schedule tells it to (see scheduler below).
```matlab
nicfg.tcp.behaviorcycle = 10;
```

These 2 values change how often the two pulses of tcp occur (sum together to TPeriod in the scheme above). It's in increments of 100 us, so 100 means 100 * 100 us = 10 ms. By default, pulse 1 happens happens 10 ms after pulse 1 happens, and pulse 2 happens 10 ms after pulse 1 happens. That's why 100/100 means interleaved 50 Hz pulsing in both channels. It is the sum of the two numbers that determines the length of a pulse1-pulse2 cycle (e.g., 100/100 means 20 ms cycle). Some of the photoemtry/behavioral timing mechanisms are done by counting cycles, so changing this number may affect them. I tried to account for cycle changes, but you may still need to let me know if some timings are off because of this.
```matlab
nicfg.tcp.pulsecycle1 = 100;
nicfg.tcp.pulsecycle2 = 100;
```

### 3. Optophotometry
![Optophotometry](https://github.com/xzhang03/NidaqGUI/raw/master/Schemes/Optophotometry.png)
Optophotometry is done by inserting opto pulses as soon as photometry pulse is done (but can be changed see below). Optophotometry operates in 20 ms cycles (50 Hz) by default.

Enable. Nothing is uploaded if false. If enabled, TCP and same-color optomphotometry must be both false.
```matlab
nicfg.optophotometry.enable = false;
```

Frequency modulator, meaning how many photometry pulses does it need before triggering an opto pulse. The photometry runs at 50 Hz (20 ms per cycle), so the opto actual frequency is 50 Hz/X, e.g., 5 means 10 Hz opto.
```matlab
nicfg.optophotometry.freqmod = 5; 
```

Number of pulses per train
```matlab
nicfg.optophotometry.trainlength = 10; 
```

How often does an opto train happen in seconds.
```matlab
nicfg.optophotometry.cycle = 30;
```

Opto pulse width. This corresponds to T2 in the image above. Notice that normally Optopulse gets turned on as soon as a photometry pulse is off (to allow as much time as possible for opto pulse to decay before taking a photometry time point), so this number is normally 14 or less given that photometry pulses are 6 ms each (i.e., max 70% duty cycle). You can however increase it to 20 if you set overlap to true below to get 100% duty cycle.
```matlab
nicfg.optophotometry.pulsewidth = 10;
```

Overlap mode. If you turn on overlap mode, opto pulse gets turned on as soon as the photometry pulse starts instead of when photometry pulse is turned off. This allows for 100% duty cycle of opto stim if you also set the pulse width to 20 ms (cycle is 20 ms). The disadvantage is that opto light may contaminate photometry recordings. If that's not a priority or if the opto light gets sent down a different fiber than the photometry light, use this option freely.
```matlab
nicfg.optophotometry.overlap = false;
```


These 2 values change how often the photometry pulse amd thet opto pulse occur (sum together to TPeriod in the scheme above). They are in increments of 100 us, so 60/140 means that opto light gets turned on 6 ms after photometry light is on, and photometry light is on 14 ms after opto light was on. It is the sum of the two numbers that determines TPeriod1 value above (e.g., 30/140 means 20 ms cycle). Some of the photoemtry/behavioral timing mechanisms are done by counting cycles, so changing this number may affect them. I tried to account for cycle changes, but you may still need to let me know if some timings are off because of this.
nicfg.optophotometry.pulsecycle1 = 60; 
nicfg.optophotometry.pulsecycle2 = 140; 

### 3. Same-color optophotometry
![Scoptophotometry](https://github.com/xzhang03/NidaqGUI/raw/master/Schemes/SCoptophotometry.png)
Same-color optophotometry is done by using an analog pulse (Ch2) to temporarily control the LED light intensity and perform opto stimulation. When opto stimulation is not turned on, the analog output is high-impedence, i.e., as if it's unplugged. The cycle (TPeriod) is 20 ms by default. This enable enables experiments that are not possible otherwise, but non-linear bleaching could be an issue.

Enable. Nothing is uploaded if false. If enabled, TCP and optomphotometry must be both false.
```matlab
nicfg.scoptophotometry.enable = false;
```

Frequency modulator, meaning how many photometry pulses does it need before triggering an opto pulse. The photometry runs at 50 Hz (20 ms per cycle), so the opto actual frequency is 50 Hz/X, e.g., 5 means 10 Hz opto.
```matlab
nicfg.scoptophotometry.freqmod = 5;
```

Number of pulses per train
```matlab
nicfg.scoptophotometry.trainlength = 10;
```

How often does an opto train happen in seconds.
```matlab
nicfg.scoptophotometry.cycle = 30;
```

Opto pulse width. This corresponds to T3 above. This step will alter the pulse width from the photometry length (6 ms default) to the opto pulse width, and change it back when photometry is back.
```matlab
nicfg.scoptophotometry.pulsewidth = 10;
```

### 4. Scheduler
Scheduler makes experimeriments simpler by defining 1) a pre-opto period, and 2) a pre-set number of opto trains. If scheduler is turned off, opto experiments (and behavior experiements, see below) are set to happen for an infinite number of trials on its own. When scheduler is on, there is a trial structure, no opto. or behavioral trials will happen until the pre-opto. period is over, and when the set number of trials are done, no more opto. or behavioral trials will happen anymore. This however does not turn off photometry pulses, which allows for post-opto. recordings. Please see below for details.



