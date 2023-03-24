I2C repeater is a device used to support more I2C devices and allow I2C devices to be placed further apart. One of the jumper pins: V0, V1, or VDC must be bridged for it to work. Please do not bridge more than one at a time. I2C I0 and I1 are directly connected. I2C O0, O1, and O2 are connected.

### R1, R2, C1, C2
It is unclear at the moment of these components are required. If R1 and R2 are soldered on, then nanosec I2C output voltage must be set to be 3.3V on the nanosec board.

### V0 mode
If you bridge V0, the output logic is both 3.3V (inherited from nanosec, max 250 mA). There is however no voltage supplied on the output side.

### V1 mode
If you bridge V1, the system power will be from one of the downstream devices. Please be sure that the input SCL and SDA logic voltages are both 3.3V before connecting to nanosec.

### V0 + V1 mode
If you bridge both V0 and V1, you are using nanosec voltage to power the chip as well as any downstream devices. Be careful to not draw too much current from the nanosec power supply

### VDC mode
If you bridge VDC, the system will be coming from the DC jack. Please make sure that the input SCL and SDA logic voltages are both 3.3V before connecting to nanosec.

### VDC + V1 mode
If you bridge both VDC and V1, you are using DC jack voltage to power the chip as well as any downstream devices. Please make sure that the input SCL and SDA logic voltages are both 3.3V before connecting to nanosec

![Schematic](./Schematic_I2C%20repeater_2023-01-03.png)
