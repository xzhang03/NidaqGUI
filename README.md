# NidaqGUI
GUI for photometry (modified from Arthur's work)
Please find the original depository at https://github.com/asugden/nidaqgui

## Basic Idea
A Matlab UI is used to designate filename and start/stop, as well as to specify experiment modes and structures to a microcontroller (see below). The microcontroller actually takes care the actual operation of the experiment.

###### Functions:
  **Legacy mode**: UI specifies the start and the end of the camera-sync pulses, and camera pulse rate. Everything else is hard-coded.
  
  **Omnibox mode (v3)**: UI specifies the parameters in the following categories:
  
    1. Modes: Two-color photometry (e.g., 488 and 405 nm), optophotometry (e.g., 488 and 625 nm), same-color optophotometry (e.g., low-power photometry + high-power opto.)
    2. Timing of the modes
    3. Camera-pulse paramteres
    4. Buzzer for synchronizing high-sampling rate microphones
    5. Rotary encoder parameters (for tracking running)
    6. Scheduler (only optophotometry or scoptophotometry modes): Baseline -> Opto phase -> Post-opto phase
    7. Scheduler only: External TTL pulses for scheduler (each pulse trigger an opto train)
    8. Scheduler only: A Time-delayed TTL pulse output after each train (e.g., to trigger water delivery)
    9. Scheduler only: A conditional version time-delayed TTL pulse (e.g., has to lick during cue to get water delivery)
    
###### Hardware
  LED driver: I wrote the code for [PLEXON single channel driver](https://plexon.com/wp-content/uploads/2017/06/PlexBright-LD-1-Single-Channel-Driver-User-Guide.pdf), but any driver with **digital input** can do TCP and optophotometry modes. **Analog input** is required to do the same-color optophotometry mode.
  
  Microcontroller:
    Legacy mode: any microscontroller with serial communication will do (e.g., Arduino UNO, Trinket m0, etc).  
    Omnibox mode: I recommend [Teensy 4.0](https://www.pjrc.com/store/teensy40.html). It's fast, easy to program, and cheap. It also has many pins. **IT CANNOT HANDLE 5V LOGIC.** The drawback is the lack of true analog output for future ramp experiments.
    Same-color optophotometry mode only: a tri-state buffer such as [74AHCT125](https://www.adafruit.com/product/1787). I also use the buffer as a unidirectional logic level shifter since Teensy4.0 cannot take 5V logic inputs. An opamp is preferred for true analog buffer.
