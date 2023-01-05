I2C repeater is a device used to support more I2C devices and allow I2C devices to be placed further apart. One of the jumper pins: V0, V1, or VDC must be bridged for it to work. Please do not bridge more than one at a time. I2C I0 and I1 are directly connected. I2C O0, O1, and O2 are connected.

### R1, R2, C1, C2
It is unclear at the moment of these components are required. If R1 and R2 are soldered on, then nanosec I2C output voltage must be set to be 3.3V on the nanosec board.

### V0 mode
If you bridge V0, the output voltage and logic are both 3.3V (inherited from nanosec, max 250 mA).

### V1 mode
If you bidge V1, the system power will be from one of the downstream devices. Please be sure that the input SCL and SDA logic voltages are both 3.3V before connecting to nanosec.

### VDC mode
If you bridge VDC, the system will be coming from the DC jack. Please make sure that the input SCL and SDA logic voltages are both 3.3V before connecting to nanosec.


![Schematic](./Schematic_I2C%20repeater_2023-01-03.png)
