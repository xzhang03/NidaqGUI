# All about Nanosec Config Files v3.5

## A. Basic configs
### 1. General
Where data will be stored (make sure this path exists):
```matlab
nicfg.BasePath = 'C:\Users\myuser\OneDrive\Documents\MATLAB\photometrydata'; 
```
---
Logging channel names for automated parsing or general notes (not essential)
```matlab
nicfg.ChannelNames  = {'PD1', 1, 'Ch1in', 2, 'camera', 3, 'ensure', 4, 'PD2', 5, 'lick', 6, 'Ch2in', 7, 'Buzz', 8}; 
nicfg.DigitalChannelNames = { };
```
---
Lab-specific server uploading (not essential)
```matlab
nicfg.serverupload = false;
nicfg.serveradd = {'SZ', '\\anastasia\data\photometry'};
```
---
Pre-filling mouse name when nanosec starts (not essential)
```matlab
nicfg.MouseName = 'SZ00';
```
---

### 2. Nanosec (basic, more below)
Serial port of nanosec. This number will be converted to "COM36".
```matlab
nicfg.ArduinoCOM = 36;
```
---
Nanosec variables that you don't often change. Generally it's fine to recording running no matter what and baumrate of 19200 is good enough.
```matlab
nicfg.RecordRunning = true;
nicfg.baumrate = 19200; 
```
---

### 3. Nidaq
Nidaq name, which you can see and change in nimax. It doesn't affect picodaq.
```matlab
nicfg.NidaqDevice = 'Dev2';
```
---
Number of nidaq channels, analog and digital. These numbers do not affect picodaq, which has fixed number of channels (4 analog + 16 digital), but if you set Nidaqchannels (analog) to 0, nanosec ui will not try to initialize any digitizer. You need a higher end (e.g., 6300) nidaq to be able to do simultaneous analog and digital recordings, so most likely digital channels will be set to 0.
```matlab
nicfg.NidaqChannels = 8;       
nicfg.NidaqDigitalChannels = 0; 
```
---
Nidaq variables that you don't change often. 2500 Hz recording is good enough for photometry and can be lowered for lower datarate applications. Monkeylogic library could be used if not having data acquisition toolbox. Leave digital string unchanged unless you are using different ports.
```matlab
nicfg.NidaqFrequency = 2500;
nicfg.useMLlibrary = false;
nicfg.DigitalString = 'Port0/Line';
```
---
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
---

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
---

## B. Nanosec
This is where the extended nanosec settings are.

### 1. General
Master enable.
```matlab
nicfg.omnibox.enable = true;
```
---

### 2. Two-color photometry (TCP)
![TCP](https://github.com/xzhang03/NidaqGUI/raw/master/Schemes/TCP.png)
TCP is done by pulses two LEDs in an interleaved manner. TCP operates in 20 ms cycles (50 Hz) by default.

Change this to true to enable tcp mode (must set optophotometry and same-color optophotometry to false).
```matlab
nicfg.tcp.enable = false;
```
---
Behavioral inter-trial interval (in seceonds, from trial start to trial start). Since there is no optogenetic stimulation in tcp mode. Behavior experiments are on their own schedule and this parameter sets how often the behavioral task occurs, e.g., 10 means one trial every 10 s. Without scheduler, the experiment will just repeat forever. With scheduler, the behavioral experiment won't start until schedule tells it to (see scheduler below).
```matlab
nicfg.tcp.behaviorcycle = 10;
```
---
These 2 values change how often the two pulses of tcp occur (TP1 and TP2, summing together to TPeriod in the scheme above). It's in increments of 100 us, so 100 means 100 * 100 us = 10 ms. By default, pulse 1 happens happens 10 ms after pulse 1 happens, and pulse 2 happens 10 ms after pulse 1 happens. That's why 100/100 means interleaved 50 Hz pulsing in both channels. It is the sum of the two numbers that determines the length of a pulse1-pulse2 cycle (e.g., 100/100 means 20 ms cycle). Some of the photoemtry/behavioral timing mechanisms are done by counting cycles, so changing this number may affect them. I tried to account for cycle changes, but you may still need to let me know if some timings are off because of this.
```matlab
nicfg.tcp.pulsecycle1 = 100;
nicfg.tcp.pulsecycle2 = 100;
```
---

### 3. Optophotometry
![Optophotometry](https://github.com/xzhang03/NidaqGUI/raw/master/Schemes/Optophotometry.png)
Optophotometry is done by inserting opto pulses as soon as photometry pulse is done (but can be changed see below). Optophotometry operates in 20 ms cycles (50 Hz) by default.

Enable. Nothing is uploaded if false. If enabled, TCP and same-color optomphotometry must be both false.
```matlab
nicfg.optophotometry.enable = false;
```
---
Frequency modulator, meaning how many photometry pulses does it need before triggering an opto pulse. The photometry runs at 50 Hz (20 ms per cycle), so the opto actual frequency is 50 Hz/X, e.g., 5 means 10 Hz opto.
```matlab
nicfg.optophotometry.freqmod = 5; 
```
---
Number of pulses per train
```matlab
nicfg.optophotometry.trainlength = 10; 
```
---
How often does an opto train happen in seconds.
```matlab
nicfg.optophotometry.cycle = 30;
```
---
Opto pulse width. This corresponds to T2 in the image above. Notice that normally Optopulse gets turned on as soon as a photometry pulse is off (TP1 = T1, to allow as much time as possible for opto pulse to decay before taking a photometry time point), so this number is normally 14 or less given that photometry pulses are 6 ms each (i.e., max 70% duty cycle). You can however increase it to 20 if you set overlap to true below to get 100% duty cycle.
```matlab
nicfg.optophotometry.pulsewidth = 10;
```
---
Overlap mode. If you turn on overlap mode, opto pulse gets turned on as soon as the photometry pulse starts instead of when photometry pulse is turned off. This means that the actual pulse width is the sum of pulse width above and cycle 1 (default 6 ms). This allows for 100% duty cycle of opto stim if you also set the pulse width to 14 ms (cycle 1 is 6 ms so 20 ms pulse totday). The disadvantage is that opto light may contaminate photometry recordings. If that's not a priority or if the opto light gets sent down a different fiber than the photometry light, use this option freely.
```matlab
nicfg.optophotometry.overlap = false;
```
---
These 2 values change how often the photometry pulse amd thet opto pulse occur (TP1 and TP2, sum together to TPeriod in the scheme above). They are in increments of 100 us, so 65/135 means that opto light gets turned on 6.5 ms after photometry light is on, and photometry light is on 13.5 ms after opto light was on. It is the sum of the two numbers that determines TPeriod1 value above (e.g., 65/135 means 20 ms cycle). Some of the photometry/behavioral timing mechanisms are done by counting cycles, so changing this number may affect them. I tried to account for cycle changes, but you may still need to let me know if some timings are off because of this.
```matlab
nicfg.optophotometry.pulsecycle1 = 65; 
nicfg.optophotometry.pulsecycle2 = 135; 
```
---

### 4. Same-color optophotometry
![Scoptophotometry](https://github.com/xzhang03/NidaqGUI/raw/master/Schemes/SCoptophotometry.png)
Same-color optophotometry is done by using an analog pulse (Ch2) to temporarily control the LED light intensity and perform opto stimulation. When opto stimulation is not turned on, the analog output is high-impedence, i.e., as if it's unplugged. The cycle (TPeriod) is 20 ms by default. This enable enables experiments that are not possible otherwise, but non-linear bleaching could be an issue.

Enable. Nothing is uploaded if false. If enabled, TCP and optomphotometry must be both false.
```matlab
nicfg.scoptophotometry.enable = false;
```
---
Frequency modulator, meaning how many photometry pulses does it need before triggering an opto pulse. The photometry runs at 50 Hz (20 ms per cycle), so the opto actual frequency is 50 Hz/X, e.g., 5 means 10 Hz opto. In the scheme above, this corresponds to TPeriod2/TPeriod1 ratio.
```matlab
nicfg.scoptophotometry.freqmod = 5;
```
---
Number of pulses per train
```matlab
nicfg.scoptophotometry.trainlength = 10;
```
---
How often does an opto train happen in seconds.
```matlab
nicfg.scoptophotometry.cycle = 30;
```
---
Opto pulse width. This corresponds to T3 above. This step will alter the pulse width from the photometry length (6 ms default) to the opto pulse width, and change it back when photometry is back. The max value is 20 ms.
```matlab
nicfg.scoptophotometry.pulsewidth = 10;
```
---

### 5. Scheduler
Scheduler makes experimeriments more streamlined by defining 1) a pre-opto period, and 2) a pre-set number of opto trains. If scheduler is turned off, opto experiments (and behavior experiements, see below) are set to happen for an infinite number of trials on its own. When scheduler is on, there is a trial structure, no opto or behavioral trials will happen until the pre-opto period is over, and when the set number of trials are done, no more opto or behavioral trials will happen anymore. This however does not turn off photometry pulses, which allows for post-opto recordings. Please see below for details.

Enable. Nothing below is uploaded if false.
```matlab
nicfg.scheduler.enable = false;
```
---
Pre-opto (and pre-behavior) period is seconds.
```matlab
nicfg.scheduler.delay = 120;
```
---
Number of opto (and behavioral) trains
```matlab
nicfg.scheduler.ntrains = 10;
```
---
External pulse triggers. If this set true, extenal inputs could be used to trigger opto (and behavioral) train in additio to schedular trains. If the external trigger TTL BNC port is left open, do not turn this on as the input is floating.
```matlab
nicfg.scheduler.manualoverride = false; 
```
---
In listening mode, all scheduler trials are suppressed unless there is an external input. Turning this on will aumatically turn manualoveride on.
```matlab
nicfg.scheduler.listenmode = false;
```
---
This sets the polarity of external triggers to active high (true, default) or active low (false)
```matlab
nicfg.scheduler.listenpol = true;
```
---
RNG for opto experiments. Use this option if you don't want to do opto on every trial (usually true in the context of behavior). In the pass trials, the opto hapens as normal. In the no-pass trials, no opto pulses will be delivered. Pass chance is set between 0 (no chance) to 100 (always pass). The pass/fail is determined by teensy4.0 hardward RNG (proprietary), and is seeded when you press start. You can't pre-determine or pre-view the RNG.
```matlab
nicfg.scheduler.useRNG = false; 
nicfg.scheduler.passchance = 30; 
```
---
Control experiments. Use this option if you want to do a control session where every trial of opto is a no-pass. This option really just enables RNG and sets pass chance to 0. You can do that to gain the same effect.
```matlab
nicfg.scheduler.control = false;
```
---
Randomized inter-trial interval. If you want to randomized ITI, set this option to true and define lower and upper limits of randomization. randomITI_min and randomITI_max are both set in seconds. If you flag randomITI to true, nanosec will ignore the cycle or behavior cycle values above. The ITI randomization is determined by teensy4.0 hardward RNG (proprietary), and is seeded when you press start. You can't pre-determine or pre-view the RNG.
```matlab
nicfg.scheduler.randomITI = false; 
nicfg.scheduler.randomITI_min = 30; 
nicfg.scheduler.randomITI_max = 40; 
```
---

### 6. Behavior
Nanosec behavior is very customizable, which also comes with a little bit of a learning curve up front. The basic idea is to time lock a behavioral trial with an optogenetic trial. You could do opto before the task or after the task within a trial (hence the old name optodelayTTL). In the TCP mode, where there is no opto, behavioral trials run on its own schedule as if there is a pseudo opto experiment going on. 

**Multiple Trial types**: Nanosec behavioral system supports up to 4 trial types that are independent from each other. They share the same action input (e.g., lick TTL) but the cue output and the reward/punishment outputs are independently customized. Some of the parameters are a single value, meaning that they are shared between all trial types. Some are a 1x4 vector, meaning that they are independent per trial type 1, 2, 3, 4. You will see below on details of multi trial-types, but if you only use 1 trial type, set ntrialtypes to 1.

#### 6.1 Master enable for behavior.
```matlab
nicfg.optodelayTTL.enable = false;
```
---

#### 6.2 Uncondtional: Opto -> Behavior
This is the basic building block of behavior tasks. The timing diagram is below.

![Timing](https://github.com/xzhang03/NidaqGUI/raw/master/Schemes/unconditional.png)

---

When does the first reward pulse start after opto train start. This is in 100-ms increments, so 20 means 2 s. This corresponds to the Food-window delay in the scheme above.
```matlab
nicfg.optodelayTTL.delay = 20;
```
---
These values define the pulse width, pulse cycle (from onset to onset), and pulse number. Pulse width and cycle are in 10-ms increments, so 15 means 150 ms.
```matlab
nicfg.optodelayTTL.pulsewidth = [15 15 15 15];
nicfg.optodelayTTL.cycle = [30 30 30 30]; 
nicfg.optodelayTTL.trainlength = [5 5 5 5]; 
```
---
Cue enable. Turn this on if you want cue (doesn't mean it's conditional).
```matlab
nicfg.optodelayTTL.cueenable = false;
```
---
When does the cue start after opto train start. This is in 100-ms increments, so 20 means 2 s. This corresponds to the Cue delay in the scheme above.
```matlab
nicfg.optodelayTTL.cuedelay = 20;
```
---
Cue duration. This is in 100-ms increments so 10 means 1 s.
```matlab
nicfg.optodelayTTL.cuedur = [10 10 10 10];
```
---

#### 6.3 Conditional: Opto -> Behavior
Conditional tasks add an action window, during which a pulse is needed (e.g., licking) to trigger reward pulses. If triggered, reward pulses start as soon as the reward delay (see above) is done. The same action pulses outside the action window do not trigger rewards. The timing diagram is below.

![Timing](https://github.com/xzhang03/NidaqGUI/raw/master/Schemes/conditional.png)

---

Set conditional on or off per trial type.
```matlab
nicfg.optodelayTTL.conditional = [false, false, false, false];
```
---
When does the action window start after opto onset. This is in 100-ms increments, so 20 means 2 s. This corresponds to Action delay in the scheme above.
```matlab
nicfg.optodelayTTL.actiondelay = 20;
```
---
The width of action window in 100-ms increments, so 50 means 5 s. The corresponds to Action duration in the scheme above.
```matlab
nicfg.optodelayTTL.actiondur = 50;
```
---
The width of reward-delivery window in 100-ms increments. This is another way to define when the action is too late. This corresponds to Food-window duration in the scheme above. This window only limits when the reward pulse can occur. If a train starts, it will finish.
```matlab
nicfg.optodelayTTL.deliverydur = [50 50 50 50]; 
```
---

#### 6.4 Uncondtional and Conditional: Behavior -> Opto
By default, behavior follows opto but you could do it the other way around but moving the behavioral reference point into a spot that X seconds leading opto onset instead of at opto onset. This method doesn't change the cue, action, and reward delays - just moves them earlier by the same amount. The actual delays therefore depends both on the delays above as well as the lead value. The timing diagrams are below.

**Unconditional behavior -> opto**

![Timing](https://github.com/xzhang03/NidaqGUI/raw/master/Schemes/unconditional%20foodthenopto.png)

**Conditional behavior -> opto**

![Timing](https://github.com/xzhang03/NidaqGUI/raw/master/Schemes/conditional%20foodthenopto.png)

---

Enable behavior-then-opto by setting this false. By default, it's opto-then-behavior, in which case the lead value has no use.
```matlab
nicfg.optodelayTTL.optothenTTL = true;
```
---
The lead value in seconds. Only effective if optothenTTL is set to false.
```matlab
nicfg.optodelayTTL.lead = 4;
```
---

### 7. Multi trialtypes
Nanosec supports up to 4 trial types. Given the IO limitation of Nanosec PCB, additional cue and reward types need to be actualized with additional hardware. Please power off Nanosec before making these hardware adjustments.

#### 7.1 Hardware modes
1. Traditional one cue type (e.g., buzzer) and one output type (e.g., food TTL). 
2. Multiple reward output types (e.g., multiple TTL outputs to control different solenoids). 
   - In total, you have 5 options for reward outputs: 1 food TTL port on Nanosec and GPIO0-3 on an additional [DIO expander module](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/DIO%20expander). 
   - Hook up guide is [here](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/DIO%20expander). You can use either Option 1 (no vreg no i2c repeater) or Option 2 (no vreg but with i2c repeater) in the hookup guide there. Please note that, if you use Option 1, make sure the I2c voltage on the Nanosec PCB is set to 3.3V. 
3. Multiple digital cue types (e.g., different TTL outputs to drive LEDs on/off at different colors). 
   - In total, you have 2 options for cue outputs: 1 buzzer port on Nanosec and a combination of GP4-7 on the [DIO expander module](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/DIO%20expander). If you use the DIO expander module here, you can either turn on GPIO4-7 one port per trial type or one combination per trial type. 
   - Hook up guide is [here](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/DIO%20expander). You can use either Option 1 (no vreg no i2c repeater) or Option 2 (no vreg but with i2c repeater) in the hookup guide there. Please note that, if you use Option 1, make sure the I2c voltage on the Nanosec PCB is set to 3.3V. 
4. (Most common) Multiple dimmable RGB cue types and output ports (e.g., bright red for Trial type 1 and dim blue+green for Trial type 2). 
   - Each trial type can have a RGB value of cue, with each primary color having 8 levels of intensities (512 colors in total). This is done through the [dimmable LED module](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/LED%20cue%20i2c). 
   - Because the dimmable LED module does not send pulses on which trial type is done per trial (i.e., it only gives the color), you will also likely want to use a [DIO expander module](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/DIO%20expander) to send collectable pulses on which trial is on and when. This shouldn't be an issue because you likely already use the DIO expander to deliver different output pulses on GPIO0-3 (see 2 above). Trial type 1 always uses GPIO4; Trial type 2 always uses GPIO5; Trial type 3 always uses GPIO6; Trial type 4 always uses GPIO7. 
   - If you use both the dimmable LED module and the DIO expander module, you will also want to use an [I2c repeater](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/I2C%20repeater) as an intermediate to safely isolate Nanosec's I2c logic level from everything else. 
   - A setup guide to use the dimmable RGB module, DIO expander, and I2c repeater rogether is the **Option 2** of [Here](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/LED%20cue%20i2c). 
   
   ![Hookup image](https://github.com/xzhang03/NidaqGUI/raw/master/Schemes/Multi%20trialtype%20hookup%20guide.png)

#### 7.2 Matlab configs
 
These two values together define the number of trialtypes and occurence frequencies. If you set ntrialtypes as 1, only Trialtype 1 is done so only the first number of all the behavioral definition vectors is used. The frequency values are expressed as integer weights (just as in MonkeyLogic). A frequency vector of [3 3 3 3] means that all trials occur at equal frequency, and is the same as [1 1 1 1] or [10 10 10 10]. A frequency vector of [8 6 4 2] means 40%, 30%, 20%, 10% occurence of the 4 trial types (assuming trial types is set to 4). In general, it's a good idea to keep the frequency weights low. If you set the frequency vector to [3 1 3 3] and ntrialtypes to 2, only Trialtype 1 and 2 will be done, and they are 3:1 in frequency. If you set the frequency to [3 0 3 3] and ntrialtypes to 4, only Trialtypes 1, 3, 4 will be done, and 2 has no chance of occuring.
```matlab
nicfg.optodelayTTL.ntrialtypes = 1; 
nicfg.optodelayTTL.trialfreq = [3 0 0 0];
```
Below is an example distribution when asking Nanosec to do 4 trial types with equal frequency (ntrialtypes = 4 and trialfreq = [3 3 3 3]).
![Random](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/Trialtype_rand.PNG?raw=true)

---
This defines the cue type of Trialtype 1. 'Buzzer' is the buzzer port on Nanosec PCB. 'DIO' means combinations of digital pulse cues (Mode 3 in Section 7.1). 'PWMRGB' means the cue comes from the dimmable LED module (Mode 4 in Section 7.1).
```matlab
nicfg.optodelayTTL.type1.cuetype = 'Buzzer';
```
---
This defines the DIO or RGB cues. If cuetype is Buzzer, this input is ignored. If cuetype is DIO, then this value should be set as a binary 1x4 vector that specifies which ports of DIO GPIO4-7 will be turned on as cue (e.g., [0 1 1 0] means that GPIO5 and GPIO6 is turned on as cue). If cue type is PWMRGB, then this value should be a 1x3 vector with each value between 0 and 7 to specify RGB color brightness (e.g., [0 2 7] means no red, dim green, and bright blue). The 8 intensity levels are on a log scale: 0, 3, 11, 35, 114, 374, 1223, 3998 (max 4095), and they should appear linear to the eye. You can have the same cue or cue-combo for different trial types. 
```matlab
nicfg.optodelayTTL.type1.RGB = [0 0 0];
```
---
Where does the reward pulse come from. This could be 'Native', meaning the food BNC port from Nanosec, or it could be DIO, meaning from GPIO0-3 on DIO expander.
```matlab
nicfg.optodelayTTL.type1.rewardtype = 'Native';
```
---
Which port on DIO will be used for reward (GPIO0, 1, 2, 3). Multiple trial types could share the same reward port. This value is ignored if reward type is set to 'Native'.
```matlab
nicfg.optodelayTTL.type1.DIOport = 0;
```
---
These settings are for Trialtype 2.
```matlab
nicfg.optodelayTTL.type2.cuetype = 'PWMRGB';
nicfg.optodelayTTL.type2.RGB = [7 0 0];
nicfg.optodelayTTL.type2.rewardtype = 'DIO';
nicfg.optodelayTTL.type2.DIOport = 0;
```
---
These settings are for Trialtype 3.
```matlab
nicfg.optodelayTTL.type3.cuetype = 'PWMRGB';
nicfg.optodelayTTL.type3.RGB = [7 0 0];
nicfg.optodelayTTL.type3.rewardtype = 'DIO';
nicfg.optodelayTTL.type3.DIOport = 0;
```
---
These settings are for Trialtype 4.
```matlab
nicfg.optodelayTTL.type4.cuetype = 'PWMRGB';
nicfg.optodelayTTL.type4.RGB = [7 0 0];
nicfg.optodelayTTL.type4.rewardtype = 'DIO';
nicfg.optodelayTTL.type4.DIOport = 0;
```
---

### 8. Encoder
Enable running encoder recordings (generally ON even if no encoder). Please see module info [here](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/Rotary%20Encoder).
```matlab
nicfg.encoder.enable = true;
```
---
Turn on to use Nanosec hardware timers to control encoder sampling, which yields more accurate results.
```matlab
nicfg.encoder.autoecho = true;
```
---

### 9. Online Scheduler and RNG echo
Online info showing trial number, opto RNG info, and trial type info if turned on. These info comes from Nanosec so it should be accurate.
```matlab
nicfg.onlineecho.enable = true;
```
---
How often does online echo occur in 100-ms increments, so 10 means one echo per 1 s. Please be ware that decreasing this value by a lot will make updates much more frequent, but it will add Matlab stress and potentially lag.
```matlab
nicfg.onlineecho.periodicity = 10;
```
---

### 10. Audio sync
Use this option to turn on buzzer at the photometry camera rate (default 30 Hz), which allows for audio/visual synchronizing. It cannot be used together with buzzer cue.
```matlab
nicfg.audiosync.enable = false;
```
---
Buzzer pitch in 100 Hz increments, so 20 means 2 kHz.
```matlab
nicfg.audiosync.freq = 20;
```


