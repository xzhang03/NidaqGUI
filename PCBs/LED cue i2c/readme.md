This is an I2C version of LED cue delivery. The PCA9685 uses I2C interface with a default address of 0x40, which is further programmable by bridging A5 (MSB) - A0 (LSB). Channels 0, 1, 2 correspond to Red, Green, and Blue LEDs respectively, with 12 bit (0-4095) PWM precision. Library: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library

[PCA9685 datasheet](https://www.nxp.com/docs/en/data-sheet/PCA9685.pdf)

![Schematic](./Schematic_LED%20cue%20I2C_2023-01-03.png)
