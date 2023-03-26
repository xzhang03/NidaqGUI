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

These 2 values change how often the two pulses of tcp occur (TP1 and TP2, summing together to TPeriod in the scheme above). It's in increments of 100 us, so 100 means 100 * 100 us = 10 ms. By default, pulse 1 happens happens 10 ms after pulse 1 happens, and pulse 2 happens 10 ms after pulse 1 happens. That's why 100/100 means interleaved 50 Hz pulsing in both channels. It is the sum of the two numbers that determines the length of a pulse1-pulse2 cycle (e.g., 100/100 means 20 ms cycle). Some of the photoemtry/behavioral timing mechanisms are done by counting cycles, so changing this number may affect them. I tried to account for cycle changes, but you may still need to let me know if some timings are off because of this.
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

Opto pulse width. This corresponds to T2 in the image above. Notice that normally Optopulse gets turned on as soon as a photometry pulse is off (TP1 = T1, to allow as much time as possible for opto pulse to decay before taking a photometry time point), so this number is normally 14 or less given that photometry pulses are 6 ms each (i.e., max 70% duty cycle). You can however increase it to 20 if you set overlap to true below to get 100% duty cycle.
```matlab
nicfg.optophotometry.pulsewidth = 10;
```

Overlap mode. If you turn on overlap mode, opto pulse gets turned on as soon as the photometry pulse starts instead of when photometry pulse is turned off. This means that the actual pulse width is the sum of pulse width above and cycle 1 (default 6 ms). This allows for 100% duty cycle of opto stim if you also set the pulse width to 14 ms (cycle 1 is 6 ms so 20 ms pulse totday). The disadvantage is that opto light may contaminate photometry recordings. If that's not a priority or if the opto light gets sent down a different fiber than the photometry light, use this option freely.
```matlab
nicfg.optophotometry.overlap = false;
```

These 2 values change how often the photometry pulse amd thet opto pulse occur (TP1 and TP2, sum together to TPeriod in the scheme above). They are in increments of 100 us, so 60/140 means that opto light gets turned on 6 ms after photometry light is on, and photometry light is on 14 ms after opto light was on. It is the sum of the two numbers that determines TPeriod1 value above (e.g., 30/140 means 20 ms cycle). Some of the photometry/behavioral timing mechanisms are done by counting cycles, so changing this number may affect them. I tried to account for cycle changes, but you may still need to let me know if some timings are off because of this.
```matlab
nicfg.optophotometry.pulsecycle1 = 60; 
nicfg.optophotometry.pulsecycle2 = 140; 
```

### 3. Same-color optophotometry
![Scoptophotometry](https://github.com/xzhang03/NidaqGUI/raw/master/Schemes/SCoptophotometry.png)
Same-color optophotometry is done by using an analog pulse (Ch2) to temporarily control the LED light intensity and perform opto stimulation. When opto stimulation is not turned on, the analog output is high-impedence, i.e., as if it's unplugged. The cycle (TPeriod) is 20 ms by default. This enable enables experiments that are not possible otherwise, but non-linear bleaching could be an issue.

Enable. Nothing is uploaded if false. If enabled, TCP and optomphotometry must be both false.
```matlab
nicfg.scoptophotometry.enable = false;
```

Frequency modulator, meaning how many photometry pulses does it need before triggering an opto pulse. The photometry runs at 50 Hz (20 ms per cycle), so the opto actual frequency is 50 Hz/X, e.g., 5 means 10 Hz opto. In the scheme above, this corresponds to TPeriod2/TPeriod1 ratio.
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

Opto pulse width. This corresponds to T3 above. This step will alter the pulse width from the photometry length (6 ms default) to the opto pulse width, and change it back when photometry is back. The max value is 20 ms.
```matlab
nicfg.scoptophotometry.pulsewidth = 10;
```

### 4. Scheduler
Scheduler makes experimeriments more streamlined by defining 1) a pre-opto period, and 2) a pre-set number of opto trains. If scheduler is turned off, opto experiments (and behavior experiements, see below) are set to happen for an infinite number of trials on its own. When scheduler is on, there is a trial structure, no opto or behavioral trials will happen until the pre-opto period is over, and when the set number of trials are done, no more opto or behavioral trials will happen anymore. This however does not turn off photometry pulses, which allows for post-opto recordings. Please see below for details.

Enable. Nothing below is uploaded if false.
```matlab
nicfg.scheduler.enable = false;
```

Pre-opto (and pre-behavior) period is seconds.
```matlab
nicfg.scheduler.delay = 120;
```

Number of opto (and behavioral) trains
```matlab
nicfg.scheduler.ntrains = 10;
```

External pulse triggers. If this set true, extenal inputs could be used to trigger opto (and behavioral) train in additio to schedular trains. If the external trigger TTL BNC port is left open, do not turn this on as the input is floating.
```matlab
nicfg.scheduler.manualoverride = false; 
```

In listening mode, all scheduler trials are suppressed unless there is an external input. Turning this on will aumatically turn manualoveride on.
```matlab
nicfg.scheduler.listenmode = false;
```

This sets the polarity of external triggers to active high (true, default) or active low (false)
```matlab
nicfg.scheduler.listenpol = true;
```

RNG for opto experiments. Use this option if you don't want to do opto on every trial (usually true in the context of behavior). In the pass trials, the opto hapens as normal. In the no-pass trials, no opto pulses will be delivered. Pass chance is set between 0 (no chance) to 100 (always pass). The pass/fail is determined by teensy4.0 hardward RNG (proprietary), and is seeded when you press start. You can't pre-determine or pre-view the RNG.
```matlab
nicfg.scheduler.useRNG = false; 
nicfg.scheduler.passchance = 30; 
```

Control experiments. Use this option if you want to do a control session where every trial of opto is a no-pass. This option really just enables RNG and sets pass chance to 0. You can do that to gain the same effect.
```matlab
nicfg.scheduler.control = false;
```

Randomized inter-trial interval. If you want to randomized ITI, set this option to true and define lower and upper limits of randomization. randomITI_min and randomITI_max are both set in seconds. If you flag randomITI to true, nanosec will ignore the cycle or behavior cycle values above. The ITI randomization is determined by teensy4.0 hardward RNG (proprietary), and is seeded when you press start. You can't pre-determine or pre-view the RNG.
```matlab
nicfg.scheduler.randomITI = false; 
nicfg.scheduler.randomITI_min = 30; 
nicfg.scheduler.randomITI_max = 40; 
```

### 5. Behavior
Nanosec behavior is very customizable, which also comes with a little bit of a learning curve up front. The basic idea is to time lock a behavioral trial with an optogenetic trial. You could do opto before the task or after the task within a trial (hence the old name optodelayTTL). In the TCP mode, where there is no opto, behavioral trials run on its own schedule as if there is a pseudo opto experiment going on. 

**Multiple Trial types**: Nanosec behavioral system supports up to 4 trial types that are independent from each other. They share the same action input (e.g., lick TTL) but the cue output and the reward/punishment outputs are independently customized. Some of the parameters are a single value, meaning that they are shared between all trial types. Some are a 1x4 vector, meaning that they are independent per trial type 1, 2, 3, 4. You will see below on details of multi trial-types, but if you only use 1 trial type, set ntrialtypes to 1.

Master enable for behavior.
```matlab
nicfg.optodelayTTL.enable = false;
```

**Uncondtional Opto -> Behavior**
This is the basic building block of behavior tasks. The timing diagram is below.
![Timing](https://github.com/xzhang03/NidaqGUI/raw/master/Schemes/unconditional.png)

When does the first reward pulse start after opto train start. This is in 100-ms increments, so 20 means 2 s.
```matlab
nicfg.optodelayTTL.delay = 20;
```



 The number and frequency of the trial types are user defined in Matlab. Some of the experiment types will require additional hardward.
> 1. Traditional one cue type (e.g., buzzer) and one output type (e.g., food TTL). You don't need additional hardward for this except for a buzzer or a single-color LED.
> 2. Multiple reward output types (e.g., multiple TTL outputs to control different solenoids). In total, you have 5 options for reward outputs: 1 food TTL port on Nanosec and GPIO0-3 on an additional [DIO expander module](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/DIO%20expander). Which port to use for which trial is defined in the config file and multiple trial types could share the same port. The DIO expander module is connected to the Nanosec I2c port. You can use either Option 1 (no vreg no i2c repeater) or Option 2 (no vreg but with i2c repeater) in the hookup guide there. Please note that, if you use Option 1, make sure the I2c voltage on the Nanosec PCB is set to 3.3V. 
> 3. Multiple digital cue types (e.g., different TTL outputs to drive LEDs on/off at different colors). In total, you have 2 options for cue outputs: 1 buzzer port on Nanosec and a combination of GP4-7 on the [DIO expander module](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/DIO%20expander). If you use the DIO expander module here, you can either turn on GP4-7 one per trial type or different combinations per trial type. You can have the same cue or cue-combo for different trial types. The DIO expander module is connected to the Nanosec I2c port. You can use either Option 1 (no vreg no i2c repeater) or Option 2 (no vreg but with i2c repeater) in the hookup guide there. Please note that, if you use Option 1, make sure the I2c voltage on the Nanosec PCB is set to 3.3V. 
> 4. Multiple dimmable RGB cue types (e.g., bright red for Trial type 1 and dim blue+green for Trial type 2). In this mode, each trial type can have a RGB value of cue, with each color having 8 levels of intensities (512 colors in total). The 8 intensity levels are on a log scale: 0, 3, 11, 35, 114, 374, 1223, 3998 (max 4095) and is done through the [dimmable LED module](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/LED%20cue%20i2c). Because the dimmable LED module does not send pulses on which trial type is done per trial (i.e., it only gives the color), you will also likely want to use a [DIO expander module](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/DIO%20expander) to send collectable pulses on which trial is on and when. This shouldn't be an issue because you likely already use the DIO expander to deliver different output pulses on GPIO0-3 (see 2 above). Trial type 1 always uses GPIO4; Trial type 2 always uses GPIO5; Trial type 3 always uses GPIO6; Trial type 4 always uses GPIO7. Notice that GPIO4-7 have similar cue purposes in Options 3 and 4. If you use both the dimmable LED module and the DIO expander module, you will also want to use an [I2c repeater](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/I2C%20repeater) to safely isolate Nanosec's I2c logic level from everything else. A setup guide to use the dimmable RGB module, DIO expander, and I2c repeater rogether is the Option 2 of [Here](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/LED%20cue%20i2c). Please power down Nanosec before setting up the connections. The general idea is to 1) pass Nanosec i2c to repeater i2c input side, use repeater outputside to pass the voltage supply to downstream modules, and use the repeater outputside to split the i2c communication to downstreawm modules. 