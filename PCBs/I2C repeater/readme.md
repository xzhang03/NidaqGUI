I2C repeater is a device used to support more I2C devices and allow I2C devices to be placed further apart. One of the jumper pins: V0, V1, or VDC must be bridged for it to work. Connecting them does not affect the input logic voltags (from nanosec), which are isolated from everything else. However, please be careful about how you bridge them (see below). One advantage of this device is that it isolates the input logic voltage and support communication between different logic levels, and the other one is that this device should enable I2c communication over long distanec (theoretically many meters).

## IO ports
### Input side
I2C I0 and I1 are the input side. By default (no R1, R2, C1, C2), they are isolated from VDC and everything else. They are connected and whatever the voltage from one of these will be passed to the next one. Therefore, do not use both I0 and I1 if they are in different logic voltages (usually when nanosec i2c voltage is specified at 5V).

### Output side
I2C O0, O1, O2 are the output side. They are connected and whatever the voltage from one of these will be passed to the others. Their logic voltage depends on where the power comes from: V0, V1, or VDC.

## Input isolation
### R1, R2, C1, C2 (read before populate)
I0 and I1 are by default isolated from everything else. Populating R1, R2, C1, C2 will no-longer isolate the input SDA and SCL voltages from the other voltages. It is unclear at the moment of these components are required. If R1 and R2 are soldered on, then nanosec I2C output voltage must be set to be 3.3V on the nanosec board.

## Modes
### V0 mode
If you bridge V0, the output logic is the same voltage as the nanosec voltage level (3.3V 250mA from teensy or 5V, depending on what you choose on the nanosec PCB). There is however no voltage supplied on the output side. Do not use both I0 and I1 if nanosec i2c voltage is specified at 5V.

### V1 mode
If you bridge V1, the system power will be from one of the downstream devices. Please be sure that the input SCL and SDA logic voltages are both 3.3V before connecting to nanosec (i.e., do not populate R1, R2, C1, C2 unless you are sure the voltage here is precisely 3.3V).

### V0 + V1 mode (most common)
If you bridge both V0 and V1, you are using nanosec voltage (3.3V or 5V) to power the chip as well as any downstream devices. If you choose 3.3V on Nanosec, be careful to not draw too much current from the teensy power regulator. If you choose 5V, do not use both I0 and I1.

### VDC mode
If you bridge VDC, the system power will be coming from the DC jack. No power is however delivered to downstream devices.

### VDC + V1 mode (second most common)
If you bridge both VDC and V1, you are using DC jack voltage to power the chip as well as any downstream devices. Please make sure V0 is not bridged in this configuration.

![Schematic](./Schematic_I2C%20repeater_2023-01-03.png)
