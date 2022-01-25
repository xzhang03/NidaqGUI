# NidaqGUI
GUI for photometry (modified from Arthur's work)
Please find the original depository at https://github.com/asugden/nidaqgui

## Basic Idea
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
    7. Scheduler only: External TTL pulses for scheduler (each pulse trigger an opto train)
    8. Stim.-delayed TTL output: A Time-delayed TTL pulse output after each train (e.g., to trigger water delivery)
    9. Conditional stim.-delayed TTL output: A conditional version time-delayed TTL pulse (e.g., has to lick during cue to get water delivery)
    
    To run Omnibox: 
    Microcontroller uses /Arduino/omniphotometrybox.ino
    Matlab uses nidaqguisz.m and nidaq_config_sz.m
    
## Hardware

  1. **LED driver** I wrote the code for [PLEXON single channel driver](https://plexon.com/wp-content/uploads/2017/06/PlexBright-LD-1-Single-Channel-Driver-User-Guide.pdf), but any driver with **digital input** can do TCP and optophotometry modes. **Analog input** is required to do the same-color optophotometry mode.
  2. **Microcontroller**:
    a. **Legacy mode**: any microscontroller with serial communication will do (e.g., Arduino UNO, Trinket m0, etc).  
    b. **Omnibox mode**: I recommend [Teensy 4.0](https://www.pjrc.com/store/teensy40.html). It's fast, easy to program, and cheap. It also has many pins. **IT CANNOT HANDLE 5V LOGIC.** The drawback is the lack of true analog output for future ramp experiments.
  3. **Same-color optophotometry mode only**: a tri-state buffer such as [74AHCT125](https://www.adafruit.com/product/1787). I also use the buffer as a unidirectional logic level shifter since Teensy4.0 cannot take 5V logic inputs. An opamp is preferred for true analog buffer.

## Modes

**Two-color photometry**
![TCP](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/TCP.png)
Two interleaved pulses, each to the digital inputs of the LED drivers. No scheduler associated with this mode. Parameters are changeable in arduino (T1, T2, T3, TPeriod). Intensities are controlled by the current limiting resistors of the LED drivers.

**Optophotometry**
![Optophotometry](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/Optophotometry.png)
One photometry pulse and one opto pulse. Photometry pulse width and cycle lengths are changeable in arduino (T1, TPeriod1). Opto parameters are changeable in Matlab through serial communicaiton (T2, TPeriod2), so can be changed on an experiment-by-experiment basis. TPeriod2 must be an integer multiplier of TPeriod1. Max T2 is [TPeriod1 - T1]. Opto train lengths and train periods are also adjustable in terms of number of pulses. The opto pulses immediately follow the offset of photometry pulse to allow max time following an opto pulse for LED to dim (LEDs don't dim instantly). Intensities are controlled by the current limiting resistors of the LED drivers.


**Same-color optophotometry**
![Scoptophotometry](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/SCoptophotometry.png)
The same digital output controls the timing of both photometry and opto (same color). Photometry parameters are changeable in arduino (T1, T2, TPeriod1). Opto parameters are changeable in Matlab throug serial communicaiton (T3, T4, TPeriod2). TPeriod2 must be an integer multiplier of TPeriod1. Opto train lengths and train periods are also adjustable in terms of number of pulses. The second output sets the intensity of the LED during the opto period, and when it's in the photometry period, the output level is not LOW but **OPEN**. OPEN means the driver uses its own current limiting resistor (potentiometer) to set the intensity of the photometry pulses and uses the microcontroller defined intensty only during opto stims.

## Camera/microphone synchronization pulses
Parameters in mMtlab and serial communicated to microcontrollers. Pulsing starts when user clicks START on the GUI and stops when user clicks STOP.

## Rotary encoder
Microcontroller sends encoder position on demand as requested by Matlab.

## Scheduler
It's a automated mode to schedule optogenetic stimulation. The general structure is:

 1. **Pre-optogenetic period**. Specifify time since the start of user clicking START on the Matlab GUI.
 2. **Optogenetic stimulation period**. Specify the number of trains, and the train details are defined in the modes above.
 3. **Post-optogenetic period**.

## Manually triggered scheduler (Listenmode; Scheduler mode required)
In this mode, the pre-stim. period is infinitely long until the external trigger is fed in. One trigger pulse gives one train.

## Stim.-delayed TTL output (Unconditional behavioral reward)
A different train is generated after the offset of an opto. Train delay, pulse width, pulse cycle, and train lengths are specified in Matlab. This can be used to e.g., unconditionally deliver reward after an optogenetic stimulation train.

## Conditional stim.-delayed TTL output (Conditional behavioral task)
The generation of stimulation-delayed train is condition to a TTL input (e.g., licking) during the response period. An audio cue may be used to signal the onset of the response period.

