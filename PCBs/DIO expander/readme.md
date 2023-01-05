This board expands the digital capability of nanosec. It uses the I2C DIO chip MCP 23008, which uses I2C at a default address of (0x20). More bits can be written on the address with the 3-bit DIP switch. Because the digital IO is commanded through I2C, it's not quite as fast but probably still has micro-seconds precision.

## No vreg mode
Bridge the NOVREG pads to use voltage inputs from nanosec. Please before that the nanosec output voltage is 3.3V before doing this as the logic level of MCP23008 is whatever its input voltage is. No need to solder on the Vreg, power barrel, C1, and C2 if you do this.

## Vreg mode
Bridge the Vreg pads to use the voltage regulator. You will also need to populate Vreg, DC barrel, C1, and C2 if you do this. This also means that you are sending the DC voltage on the I2C bus - be careful about if nanosec or other components can work with that.

Library: https://github.com/adafruit/Adafruit-MCP23008-library

Datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/MCP23008-MCP23S08-Data-Sheet-20001919F.pdf

![Schematic](./Schematic_Nanosec%20DIO_2023-01-03.png)
