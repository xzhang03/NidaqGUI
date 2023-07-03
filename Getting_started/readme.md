# I just received an Nanosec box. What do I do?

## 0. What did I get?
Below are items that you could have received. This guide mostly covers the nanosec box and tester. Please refer to the pages of the other components for details.

  1. A nanosec box (with teensy 4.0 in the center). Hardware details are [here](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/Nanosec)
  2. A nanosec tester. This is a gadget that is functionally equivalent to a nanosec box but doesn't have any BNC connectors or audio jacks. It is meant to test the config files and timings before deploying to your photometry rigs. It uses on-PCB LEDS to indicate when each digital pulse is sent out and uses buttons to mimic natural inputs (e.g., licking). Hardware details are [here](https://github.com/xzhang03/TeensyTester). Some pins are changed when compiling the code for a tester (see Section 4 below).
  3. Rotary encoder. Hardware details are [here](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/Rotary%20Encoder).
  4. Shifter. A box that enables multiplexing up to 6 channels of photometry/optogenetics control. Hardware details are [here](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/Shifter).
  5. DIO expander. A box that enables multiple trial types of behavior experiments. Hardware details are [here](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/DIO%20expander).
  6. RGB cue module. A 256-color module to enable LED as cues for behaviors. Hardware details are [here](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/LED%20cue%20i2c).
  7. I2C repeater. A module that enables longer cable lengths and component counts on the I2C bus. Hardware details are [here](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/I2C%20repeater).
  8. (Retired) [Buzzer](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/Buzzer).
  9. (Retired) [Non-i2c LED cue](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/LED%20cue).
  
  
## 1. How do I know my Nanosec box or tester is working?
### 1. Nanosec box
  1. Before testing, please make sure that, in the center of the PCB, "I2C=3V" and "BUFOUT=3V" are bridged. You can move the jumpers to 5V once you are familiar with the system. You can use **only 1 jumper per side**.
  2. When you plug in the micro USB on teensy, you should be able to record **6 ms, 50 Hz pulses** on both Ch1 and Ch2 BNCs. This is the default two-color photometry mode. Seeing the pulses does not mean your firmware vesion is up-to-date but it has some firmware there.
### 2. Nanosec tester
  1. When you plug in the micro USB on teensy, you should be able to see LED 2 and 3 blinking at **6 ms, 50 Hz pulses** each. This is the default two-color photometry mode. Seeing the pulses does not mean your firmware vesion is up-to-date but it has some firmware there.
  
## 2. Installing Nanosec software
  1. Clone the repo to your MATLAB path of interest. Do not add any folder to path yet.
  2. Navigate to the main folder, and run the following function. You can run section-by-section if you'd like.
	  ```MATLAB
	  nanosec_setup();
	  ```
  3. Please note the expected **firmware version (e.g., v3.5)**.
  4. Choose no on "Use PicoDAQ?" if you do not have one.
  5. If you see this message "Matlab is old. Please copy 'nanosecfun\old' to overwrite 'nanosecfun'." That means you are using an early version of matlab, which requires different functions. Please copy the files and overwrite. You can do this after finishing running this function. The folder "old" should never be in MATLAB paths.
  6. The program will add the appropriate folders to MATLAB paths. When you see "Nanosec settings saved", this step is succesfully completed.
  
## 3. COMs and firmwares
### 1. Get COMs
Each serial hardware has a COM that is assigned by the computer. It generally doesn't change if you unplug it and plug it back in. To get the COM port of your Nanosec and tester, plug them in and run:
```MATLAB
arduinoList()
```
If you see nothing, check that the USB cable is usable and plugged in correctly. You do not need proper firmware to be assigned a COM port. If you see multiple COMs, unplug your Nanosec/tester and run the command again. Whichever COM port that has disappeared is the correct one.

### 2. Get onboard firmware versions
Once you know the COM number (say "COM22"), run nanosecver(com) like below:
```matlab
nanosecver('COM22');
```
You should hear back a string of text such as
 >Nanosec firmware version: v3.50PA498 Mar 27 2023 15:38:25.

Here is how to parse it:
  1. 'v3.50' is the version, which should match the expected version above in Section 2.
  2. 'P' means PCB nanosec. If you use tester, it should say 'T'. 'P' and 'T' versions differ in Pin assignments and should match the PCBs.
  3. 'A' means non-debug firmware. If you see anything else, you will need to reflash the firmware (Section 4 below).
  4. '498' is the CPU speed, 498 MHz. Keeping this number below 500 MHz decreases the CPU temperature.
  5. The rest is the compilation date and time.
  
## 4. Reflash firmwares
If you need to reflash firmware, please see this [page](https://github.com/xzhang03/NidaqGUI/tree/master/Arduino/nanosec) for instructions. Note that you make sure that the '#define' flags match what you expect (e.g., choose Nanosec box vs tester), before you compile.

Below are the pin assignments in the tester mode (matching the numbers on the tester PCB):
  1. Pin 2: Ch1 pulse
  2. Pin 3: Ch2 pulse
  3. Pin 4: Tristate pin (only used for same-color optophotometry)
  4. Pin 5: Buzzer cue
  5. Pin 6: Food/reward pulses
  6. Pin 7: Pre-opto state indicator
  7. Pin 8: In-opto state indicator
  8. Pin 9: Post-opto state indicator
  9. Pin 20: Mimic input-pulse inputs for externally triggered opto
  10. Pin 21: Unused
  11. Pin 22: Mimic lick-pulse inputs for conditional experiments
  12. Pin 23: Unused
  
## 5. Doing the first experiment
If everything checks out above, run the following command to start an experiment:
```MATLAB
nanosec();
```
You should be greeted with a simple UI below:
![UI](./maingui.png)

