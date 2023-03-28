# Nanosec shifter
The shifter is used to further multiplex photometry and optogenetic channels. It takes a pulse train and splits it into 2-4 trains. For example, it takes a train of 50Hz pulses and turn into two interleaved 25 Hz trains (IIIIIIII -> ABABABAB, where Is are input pulses, As are output A pulses, and Bs are output B pulses). Each input pulse I is synchronous with an output pulse A or B. Since it doesn't change when the output pulses occur but rather controls whether it occurs or not at a given input pulse, it wouldn't create overlaps that do not exist before using shifter. The draw back is that each shifted channel pulses at a fraction of the input frequency (50Hz in = 2 x 25 Hz out = 3 x 16.67 Hz out = 4 x 12.5 Hz out). 

Use case 1: 465 nm and 405 nm photometry + 625 Chrimson stimulation. Using the nanosec optophotometry mode, you can use nanosec channel 1 and shifter to make interleaved 1A and 1B pulses (both at half frequency of the original channel 1) and control both 465 and 405 nm pulses. Nanosec channel 2 is free to be used to drive optogenetic stimulation pulses. 

The shifter channel count is set using the dip switch. The switch definitions and the truth table for each channel are below:
| Switch 1  | 2 | 3 | 4 |
| ------------- | ------------- | ------------- | ------------- |
| Ch2 LOW  | Ch2 HIGH  | Ch1 LOW  | Ch2 HIGH  |


| HIGH bit (H)  | LOW bit (L) | Output channels (H*2 + L + 1) |
| ------------- | ------------- | ------------- |
| 0  | 0  | 1  |
| 0  | 1  | 2  |
| 1  | 0  | 3  |
| 1  | 1  | Not onboard  |

## Hookup guide
Configure nanosec i2c output voltage to 5V if you are powering shifter from nanosec. I2c Repeater may be used as well (with V0 and V1 bridged).

![schematic](./Schematic_Nanosec%20Shifter_2022-09-27.png)

![Shifter footprint](./Shifter%20footprint.PNG)
