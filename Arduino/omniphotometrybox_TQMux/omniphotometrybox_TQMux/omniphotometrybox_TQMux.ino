// Multiplexer for using Nanosec (omniphotometrybox) with Thorlabs TCube

#define debugmode true

// photometry analogout = A0
// opto analogout = A1
// SCL pin = SCL
// SDA pin = SDA

const byte LEDsw = 11; // LED switch. This turns off pulsing mode (active high)
const byte LEDout = 10; // Output to drive an LED indicator
const byte SW1p = 9; // Push button, active high
const byte SW1b = 7; // Knob direction B, active high
const byte SW1a = 5; // Knob direction A, active high

// DAC
unsigned int a0out = 255; // 12 bit
unsigned int a1out = 4096; // 12 bit

// UI
const byte inc0 = 1;
const byte inc1 = 100;
bool editing = false; // editing = not running
bool asel = false; // false = A0, true = A1


void setup() {
  // put your setup code here, to run once:
  pinMode(LEDsw, INPUT);
  pinMode(LEDout, OUTPUT);
  pinMode(SW1p, INPUT);
  pinMode(SW1b, INPUT);
  pinMode(SW1a, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

}
