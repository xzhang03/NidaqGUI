# DIO expander
This board expands the digital capability of nanosec. It uses the I2C DIO chip MCP 23008, which uses I2C at a default address of (0x20). More bits can be written on the address with the 3-bit DIP switch. Because the digital IO is commanded through I2C, it's not quite as fast but probably still has micro-seconds precision.

## Hookup guide
## No vreg mode without I2c repeater (common)
Bridge the NOVREG pads to use voltage inputs from nanosec. Please note that the nanosec output voltage is **3.3V** before doing this as the logic level of MCP23008 is whatever its input voltage is. Then you can connect nanosec I2C with the DIO expander I2C with a 4-channel audio cable. No need to solder on the Vreg, power barrel, C1, and C2 if you use the no vreg mode.

## No vreg mode with I2c repeater (common)
Another safer option is to use the [I2C repeater](https://github.com/xzhang03/NidaqGUI/tree/master/PCBs/I2C%20repeater) with V0 and V1 both bridged. In that mode, you can connect nanosec I2c port with the repeater I0 or I1, and connect repeater O0, O1, or O2 with the DIO expander I2c. In that mode, you can use either 3.3V or 5V on nanosec i2c power supply voltage. Do not use both I0 and I1 if you choose 5V on the nanosec I2c power.

## Vreg mode
Bridge the Vreg pads to use the voltage regulator. You will also need to populate Vreg, DC barrel, C1, and C2 if you do this. This also means that you are sending the DC voltage on the I2C bus - be careful about if nanosec or other components can work with that. Usually, you will want to use an I2c repeater in this scenario with V1 bridged.

Library: https://github.com/adafruit/Adafruit-MCP23008-library

Datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/MCP23008-MCP23S08-Data-Sheet-20001919F.pdf

![Schematic](./Schematic_Nanosec%20DIO_2023-01-03.png)
