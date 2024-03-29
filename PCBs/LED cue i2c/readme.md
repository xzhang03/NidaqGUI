# RGB LED cue module (I2c, dimmable)
This is an I2C version of LED cue delivery. The PCA9685 uses I2C interface with a default address of 0x40, which is further programmable by bridging A5 (MSB) - A0 (LSB). Channels 0, 1, 2 correspond to Red, Green, and Blue LEDs respectively, with 12 bit (0-4095) PWM precision. Library: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library

## Hookup guide
### 1. RGB module only.
If the i2c LED module is used alone with nanosec, please use **3.3V** nanosec I2c voltage. Connect nanosec I2C port with PCA9685 port with a 4-channel audio cable.
### 2. (Recommended) Use with a DIO expander
I recommend using a [DIO expander module](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/DIO%20expander) to logging cue type and behavior outputs. If i2c LED module is used together with DIO expander, an [I2C repeater](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/I2C%20repeater) is also recommended. In this case, you can use either 3.3V or 5V nanosec i2c output voltage and use the table below for hookup guide. Bridge V0 and V1 on the repeater.

![Hookup scheme](https://github.com/xzhang03/NidaqGUI/blob/master/Schemes/Multi%20trialtype%20hookup%20guide.png)

| Purpose | Nanosec  | I2c repeater | I2c LED module | DIO expander |
| ------- | -------- | ------------ | -------------- | ------------ |
| I2c nanosec-repeater | I2C  | I2C I0 |  |  |
| Set Nanosec I2c voltage | Bridge I2c 5V or 3.3V  |  |  |  |
| Leave I1 empty if using 5V on nanosec | X | I2C I1  = Empty| X | X |
| Bridge V0 & V1 |   | Bridge V0 & V1 |  |  |
| I2c repeater-LED     |   | I2C O0  | I2C | |
| I2c repeater-DIO     |   | I2C O1  | | I2C |
| DIO Trial 1 cue indicator  |   |   | | GPIO4 |
| DIO Trial 2 cue indicator  |   |   | | GPIO5 |
| DIO Trial 3 cue indicator |   |   | | GPIO6 |
| DIO Trial 4 cue indicator  |   |   | | GPIO7 |
| DIO Trial 1 outcome pulse  |   |   | | Any of GPIO0-3 |
| DIO Trial 2 outcome pulse  |   |   | | Any of GPIO0-3 |
| DIO Trial 3 outcome pulse |   |   | | Any of GPIO0-3 |
| DIO Trial 4 outcome pulse  |   |   | | Any of GPIO0-3 |

[PCA9685 datasheet](https://www.nxp.com/docs/en/data-sheet/PCA9685.pdf)

![Schematic](./Schematic_LED%20cue%20I2C_2023-01-03.png)
