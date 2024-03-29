# Code

## Structure
Starting at version v3.3, this code is split up into parts. The main code is nanosec.ino. The rest are split as below. If you open any of them in arduino IDE, the rest will show up as tabs in alphabetical order. Arduino will concatenate them before compiling (hence the numbers). The splitting is done lazily right now and will be updated into proper .cpp and .h files in the future<sup>TM</sup>.

* nanosec:	Modes, time, channel switches, debug
* 0_pins:	Pin and encoder variables
* 0_vars:	All other global variables
* 1_ch:   Turning ch1/ch2 LEDs on and off
* 1_i2c:  I2C settings
* 1_mode: Mode switching functions
* 1_opho: Optophotometry
* 1_pind: Pin initilizatoin functions
* 1_scop: Same-color optophotometry
* 1_tcp:  Two-color photometry
* 2_cam:  Cam pulse functions
* 2_enc:  Encoder
* 2_extt: External TTL trigger functions
* 2_food: Food/reward delivery functions
* 2_RNG:  RNG functions
* 2_sche: Scheduler functions
* 2_sem:  Serial menu
* 2_ser:  Serial functions
* 3_perf: Performance/speed check

## Upload
Please use arduino IDE to upload the code (need [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html) for Arduino Version before 2.0). This code is written on Teensy 4.0 but Teensy 4.1 is workable.

## Additional libraries (not in teensyduino)
1. Adafruit PWM Servo Driver Library (Adafruit, v2.4.1) and parent libraries
2. MCP23008 (Rob Tillaart, v0.1.4), must be v2.0 or before

## CPU clock speed
The code now runs Teensy 4.0 at 450 MHz. Dropping CPU clock from 600 MHz to 450 MHz lowers CPU voltage and makes the hardware run cooler (source: PJRC forum). You can change this value.

```C
#if defined(__IMXRT1062__)
extern "C" uint32_t set_arm_clock(uint32_t frequency);
#define cpu_speed 450000000
#endif
```

## Modes
There are different usage modes that the compiler will take into considerations to assign pins.

### 1. Normal PCB mode.
99% of the time you will run the code in normal PCB mode. Make sure the these flags are set as is.
```C
#define PCB true
#define TeensyTester false
#define debugmode false // Master switch for all serial debugging
#define serialdebug false // This is the same as above but for MATLAB serial debugging
#define debugpins TeensyTester // This is set to follow TeensyTester. It is independent of debugmode/serialdebug
#define perfcheck false // Checking cycle time. Independent of debugmode/serialdebug
```

### 2. Old hand-soldered boards
There aren't many of these boards around, but if you happen to have them, please set PCB to be false.
```C
#define PCB false
#define TeensyTester false
#define debugmode false // Master switch for all serial debugging
#define serialdebug false // This is the same as above but for MATLAB serial debugging
#define debugpins TeensyTester // This is set to follow TeensyTester. It is independent of debugmode/serialdebug
#define perfcheck false // Checking cycle time. Independent of debugmode/serialdebug
```

### 3. Teensy tester
If you have the associated teensy tester, you can pop your Teensy in and use the onboard LEDs and buttons to test out your experiment configs. This tester uses slightly different pinouts as the normal PCB version and uses extra LEDS to show scheduler info, so you will need to flag the tester mode.
```C
#define PCB true
#define TeensyTester true
#define debugmode false // Master switch for all serial debugging
#define serialdebug false // This is the same as above but for MATLAB serial debugging
#define debugpins TeensyTester // This is set to follow TeensyTester. It is independent of debugmode/serialdebug
#define perfcheck false // Checking cycle time. Independent of debugmode/serialdebug
```

### 4. Debug flags
You can use the debug flags to ender debug mode if you feel advanturous. :)

#### debugmode allows you to use Arduino terminal to debug and see debug messages. serialdebug does the same thing but uses Matlab (and presumably other softwares) as a debug interface. You flag one of them true to enter debug mode.
```C
#define debugmode false // Master switch for all serial debugging
#define serialdebug true // This is the same as above but for MATLAB serial debugging
```

#### If you flag either debugmode or serialdebug as true, you will receive debug info of the following modules as you flag them true. They only function of you are in debug mode. They show pulse info (use with care), opto train info, schedular info, and behavioral task info.
```C
#define showpulses false // Extremely verbose. Dependent on debugmode or serialdebug to be true
#define showopto true // Dependent on debugmode or serialdebug to be true
#define showscheduler true // Dependent on debugmode or serialdebug to be true
#define showfoodttl true // Dependent on debugmode or serialdebug to be true
```

#### If you use TeensyTester, the debugpins are automatically flagged as true. You can force them true or false as well. These pins show where you are in scheduler. They do not need to be in debugmode to work.
```C
#define debugpins TeensyTester // This is set to follow TeensyTester. It is independent of debugmode/serialdebug
```

#### You can do a performance check to see how fast each cycle of code is running. You don't need to be in debug mode but the results will be read out by software (e.g., MATLAB) in the serial port.
```C
#define perfcheck true // Checking cycle time. Independent of debugmode/serialdebug
```
