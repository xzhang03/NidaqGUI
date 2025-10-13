// =============== Pins ===============
const byte ch1_pin = 2; // try to leave 0 1 open. 
const byte ch2_pin = 3; // 405 nm or red opto
const byte AOpin = 3; // Use same pin as ch2 until true analog outputs are used in the future
const byte tristatepin = 4; // Use to control tristate transceivers (AO (used as digital here), 0, or disconnected). Default active low
#if TeensyTester
  const byte cam_pin = 25; // Cam pulses
#else
  const byte cam_pin = 21; // Cam pulses
#endif

#if TeensyTester
  const byte switchpin = 20; // Use if not in PCB mode
  const byte foodTTLpin = 6; // output TTL to trigger food etc
  const byte audiopin = 5; // Pin for audio signal
#elif PCB
  const byte switchpin = 10; // External toggle to start a train (default active high). Usually used in listenmode
  const byte foodTTLpin = 15; // output TTL to trigger food etc
  const byte audiopin = 6; // Pin for audio signal
#else
  const byte switchpin = 17; // Use if not in PCB mode
  const byte foodTTLpin = 9; // output TTL to trigger food etc
  const byte audiopin = 6; // Pin for audio signal
#endif
const byte foodTTLinput = 22; // input TTL for conditional food pulses (active high, 3.3 V only!!)
const byte led_pin = 13; // onboard led


const byte i2csda = 18; // Reserve for future i2c
const byte i2cscl = 19; // Reserve for future i2c

// ============= debugpins =============
const byte serialpin = 24; // Parity signal for serial pin
#if TeensyTester
  const byte schedulerpin = 26; // On when scheduler is used
  const byte preoptopin = 7; // preopto
  const byte inoptopin = 8; // preopto
  const byte postoptopin = 9; // preopto
#else
  const byte schedulerpin = 5; // On when scheduler is used
  const byte preoptopin = 7; // preopto
  const byte inoptopin = 8; // preopto
  const byte postoptopin = 9; // preopto
#endif
bool serialpinon = false;

// ============== Encoder ==============
//#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
#if PCB
  // PCB
  Encoder myEnc(17,16); // pick your pins, reverse for sign flip
#else
  // Proto
  Encoder myEnc(18,19); // pick your pins, reverse for sign flip
#endif
