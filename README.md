# Nanosec Photometry (original name: NidaqGUI)
GUI for photometry (modified from Arthur's work)
Please find the original depository at https://github.com/asugden/nidaqgui
Renaming in progress.

## A. Basic Idea
A Matlab UI is used to designate filename and start/stop, as well as to specify experiment modes and structures to a microcontroller (see below). The microcontroller takes care the actual operation of the experiment.

###### Functions:
  **Legacy**: UI specifies the start and the end of the camera-sync pulses, and camera pulse rate. Everything else is hard-coded. This is the simple version
    
    To run legacy: 
    Microcontroller uses (just an example) /Arduino/quad_encoder_selftrigger_pulsing_only.ino
    Matlab uses nidaqgui.m and nidaq_config.m
                   
  
  **Omnibox (v3)**: UI specifies the parameters in the following categories:
  
    1. Modes: Two-color photometry (e.g., 488 and 405 nm), optophotometry (e.g., 488 and 625 nm), same-color optophotometry (e.g., low-power photometry + high-power opto.)
    2. Timing of the modes
    3. Camera-pulse paramteres
    4. Buzzer for synchronizing high-sampling rate microphones
    5. Rotary encoder parameters (for tracking running)
    6. Scheduler (only optophotometry or scoptophotometry modes): Baseline -> Opto phase -> Post-opto phase
    7. Listen mode (passive scheduler): External TTL pulses for scheduler (each pulse trigger an opto train)
    8. Stim.-delayed TTL output: A Time-delayed TTL pulse output after each train (e.g., to trigger water delivery)
    9. Conditional stim.-delayed TTL output: A conditional version time-delayed TTL pulse (e.g., has to lick during cue to get water delivery)
    10.Hardware strong RNG for randomizing opto-trials (scheduler only and does not apply to listen mode)
    11.Harware randomized inter-trial interval (scheduler only and does not apply to listen mode)
    
    To run Omnibox: 
    Microcontroller uses /Arduino/omniphotometrybox.ino
    Matlab uses nidaqguisz.m and nidaq_config_sz.m
    
PCB for Omniphotometrybox: [EasyEDA](https://github.com/xzhang03/NidaqGUI/blob/master/PCB_omni/EasyEDA_PCB_Omnibox%20v3.json), [Scheme](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/Schematic_Omnibox%20v3_2022-04-08.png), and [Gerber files](https://github.com/xzhang03/NidaqGUI/blob/master/PCB_omni/Gerber_PCB_Omnibox%20v3.zip). Parts numbers on Digikey: WM5514-ND (BNC), CP-435107RS-ND (4-cond Audio/I2C jack).

PCB for IR encoder: [EasyEDA](https://github.com/xzhang03/NidaqGUI/blob/master/PCB_omni/EasyEDA_PCB_IR%20rotary%20encoder%20v1.json), [Scheme](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/Schematic_IR%20rotary%20encoder%20v1_2022-04-13.png), and [Gerber files](https://github.com/xzhang03/NidaqGUI/blob/master/PCB_omni/Gerber_PCB_IR%20rotary%20encoder%20v1_2022-04-13.zip).

PCB for buzzer: [EasyEDA](https://github.com/xzhang03/NidaqGUI/blob/master/PCB_omni/EasyEDA_PCB_Buzzer_2022-04-13.json), [Scheme](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/Schematic_Buzzer_2022-04-13.png), and [Gerber files](https://github.com/xzhang03/NidaqGUI/blob/master/PCB_omni/Gerber_PCB_Buzzer_2022-04-13.zip).

Containment for PCB box: [Github](https://github.com/xzhang03/Half_breadboard_box/blob/main/half%20breadboard%20box%20PCB.ai).

Breadboard for Omniphotometrybox: [Scheme](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/omnibox_half_breadboard.png).
    
## B. Hardware

  1. **LED driver**: I wrote the code for [PLEXON single channel driver](https://plexon.com/wp-content/uploads/2017/06/PlexBright-LD-1-Single-Channel-Driver-User-Guide.pdf), but any driver with **digital input** can do TCP and optophotometry modes. **Analog input** is required to do the same-color optophotometry mode.
  2. **Microcontroller**:
    a. **Legacy mode**: any microscontroller with serial communication will do (e.g., Arduino UNO, Trinket m0, etc).  
    b. **Omnibox mode**: I recommend [Teensy 4.0](https://www.pjrc.com/store/teensy40.html). It's fast, easy to program, and cheap. It also has many pins. **IT CANNOT HANDLE 5V LOGIC.** The drawback is the lack of true analog output for future ramp experiments.
  3. **Same-color optophotometry mode only**: a tri-state buffer such as [74AHCT125](https://www.adafruit.com/product/1787). I also use the buffer as a unidirectional logic level shifter since Teensy4.0 cannot take 5V logic inputs. An opamp is preferred for true analog buffer.
  
## How to order
1. Use one of the PCB printing services (e.g., [JLCPCB](https://jlcpcb.com/), [PCBWAY](https://www.pcbway.com/), [OSHPARK](https://oshpark.com/), [etc](https://pcbshopper.com/)). Upload the zip Gerber files from [here](https://github.com/xzhang03/NidaqGUI/tree/master/PCB_omni). The sizes are pre-set so just choose your quantity, color, etc... You are good to go with PCBs
2. Upload the [bill of materials excel files](https://github.com/xzhang03/NidaqGUI/tree/master/PCB_omni) to [Digikey](https://www.digikey.com/en/mylists/). The quantities are for 1 unit. For resisters, trimmers, and header pins, you are better off using bulk kits from Amazon/Ebay. Please feel free to ask me about replacements.

## C. Modes

**Two-color photometry**
![TCP](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/TCP.png)
Two interleaved pulses, each to the digital inputs of the LED drivers. No scheduler associated with this mode. Parameters are changeable in arduino (T1, T2, T3, TPeriod). Intensities are controlled by the current limiting resistors of the LED drivers.

**Optophotometry**
![Optophotometry](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/Optophotometry.png)
One photometry pulse and one opto pulse. Photometry pulse width and cycle lengths are changeable in arduino (T1, TPeriod1). Opto parameters are changeable in Matlab through serial communicaiton (T2, TPeriod2), so can be changed on an experiment-by-experiment basis. TPeriod2 must be an integer multiplier of TPeriod1. Max T2 is [TPeriod1 - T1]. Opto train lengths and train periods are also adjustable in terms of number of pulses. The opto pulses immediately follow the offset of photometry pulse to allow max time following an opto pulse for LED to dim (LEDs don't dim instantly). Intensities are controlled by the current limiting resistors of the LED drivers.

***Pure optogenetics***
You can use channel 2 for pure optogenetic stimulations. Pulse widths are adjustable to a max value of TPeriod1. In this mode, TPeriod1 is adjustable at a resolution of 0.1 ms to a max value of 51 ms. Please use caution and follow the following rules.

1. The final pulse frequency is determined by TPeriod1 as well as by frequency modulator (a value that sets TPeriod2/TPeriod1). 
  >For example, 10 Hz stimulation can be written as TPeriod1 = 20 ms and freqmod = 5 (stim comes on once every 5 x 20 = 100 ms). 
  >15 Hz stimulation can be written as TPeriod1 = 11.1 ms and freqmod = 6 (stim comes on once every 6 x 11.1 = 66.6 ms). 
  >20 Hz stimulation can be written as TPeriod1 = 10 ms and freqmod = 5 (stim comes on once every 5 x 10 = 50 ms). 
  >30 Hz stimulation can be written as TPeriod1 = 11.1 ms and freqmod = 3 (stim comes on once every 3 x 11.1 = 33.3 ms). 
3. TPeriod1 is operationally defined as the sum of PulseCycle1 and PulseCycle2. For normal usage, you can set PulseCycle1 to 0 and PulseCycle2 to whatever value that can be used for the stimulation frequency of choice (see 1 for examples). For maximum cycle time, set both values to 25.5 ms to obtain TPeriod1 = 51 ms.
4. Changing TPeriod1 will also affect the train cycle, which by default assumes TPeriod1 = 20 ms. If you double TPeriod1, you should half the traincycle value that is uploaded to microcontroller in order for the final train cycle time (in real time units) to remain the same.

**Same-color optophotometry**
![Scoptophotometry](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/SCoptophotometry.png)
The same digital output controls the timing of both photometry and opto (same color). Photometry parameters are changeable in arduino (T1, T2, TPeriod1). Opto parameters are changeable in Matlab throug serial communicaiton (T3, T4, TPeriod2). TPeriod2 must be an integer multiplier of TPeriod1. Opto train lengths and train periods are also adjustable in terms of number of pulses. The second output sets the intensity of the LED during the opto period, and when it's in the photometry period, the output level is not LOW but **OPEN**. OPEN means the driver uses its own current limiting resistor (potentiometer) to set the intensity of the photometry pulses and uses the microcontroller defined intensty only during opto stims.

## D. Camera/microphone synchronization pulses
Parameters in mMtlab and serial communicated to microcontrollers. Pulsing starts when user clicks START on the GUI and stops when user clicks STOP.

## E. Rotary encoder
Microcontroller sends encoder position periodically to Matlab. Total numbers of positions sent and received are both saved.

## F. Scheduler
It's a automated mode to schedule optogenetic stimulation. The general structure is:

 1. **Pre-optogenetic period**. Specifify time since the start of user clicking START on the Matlab GUI.
 2. **Optogenetic stimulation period**. Specify the number of trains, and the train details are defined in the modes above.
    RNG can be implemented to randomize whether opto stim is given or not. Inter-trial interval can also be hardware randomized within the specified range.
 4. **Post-optogenetic period**.

## G. Manually triggered scheduler (Listenmode; Scheduler mode required)
In this mode, the pre-stim. period is infinitely long until the external trigger is fed in. One trigger pulse gives one train, and the number of triggers can be as many as needed.

## H. Food TTL output (Unconditional behavioral reward)
A food pulse train is generated in timelock with optogenetic stims or periodically in pure photometry experiments. Train delay, pulse width, pulse cycle, and train lengths are specified in Matlab with reference to opto starts of each trial. This can be used to e.g., unconditionally deliver reward after an optogenetic stimulation train.

### 1. Unconditional Opto-then-food
This is the timing of the unconditional reward delivery system, where a food pulse trains is sent out every time after opto stimulation. For two-color photometry experiments, food pulses are sent out in reference to a virtual opto train (no actual opto stimulations are done). Cue pulses are optional.

![unconditional_opto-then-food](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/unconditional.png)

### 2. Unconditional Food-then-opto
This is the timing of unconditional reward delivery system in which food is delivered before opto pulses. Essentially, the timing reference point is now moved forward to X seconds before an opto train instead of at the onset of the opto train. As a result, all timings are moved forward by the same amount.

![unconditiona_food-then-opto](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/unconditional%20foodthenopto.png)


## I. Conditional stim.-delayed TTL output (Conditional behavioral task)
The generation of stimulation-delayed train is condition to a TTL input (e.g., licking) during the response period. An audio cue or an LED cue may be used to signal the onset of the response period.

### 1. Conditional Opto-then-food
Timing of conditional reward delivery system. A cue window is specified at a delay after opto train onset (also works in TCP). An action window is specified with independent delay and duration. If a TTL pulse is received during this action window (e.g., the mouse licked), food train is delivered at the earliest point within the food-train window. If a lick happens during the action window but before the food window, food delivery will be delayed until when the food window opens. If a lick happens outside the action window or after the food window closes, no food will be delivered.

![conditional_opto-then-food](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/conditional.png)

### 2. Conditional Food-then-opto
Same as before, but food is set to be delivered before opto. Now everything is moved earlier by the lead time.
![conditiona_food-then-opto](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/conditional%20foodthenopto.png)
