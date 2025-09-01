// Third Gen photometry box: Nanosec v3.5
// Handle almost everything you need for photometry and behavior
// Tested using Teensyduino with Arduino IDE 1.8.X
// Stephen Zhang

// ** Additional library not in Teensyduino **
// Adafruit PWM Servo Driver Library (Adafruit, v2.4.1) and parent libraries
// MCP23008 (Rob Tillaart, v0.1.4), must be before v2.0

// To lower cpu temperature, upload with 450 Mhz CPU speed selected

/* 1. Two color photometry
 * 2. Optophotometry (switch on)
 * 3. Same-color optophotometry (analogy input control intensity)
 * 4. Arbitrary photometry and optogenetic frequency
 * 5. Cam pulses with adjustable frequency
 * 6. Encoder at the same frequency as 4
 * 7. Preprogram experiment timescale: baseline -> opto -> postopto
 * 8. External input for scheduler (6)
 * 9. TTL pulses for other parts of the experiment
 * 10.Audio syncing
 * 11.Hardware random number generator for opto trials
 * 12.Online serial echo of trial and RNG infomation
 * 13.Multiple behavioral trial types switchable online
 * 14.Expanded IO (external PWM and DIO)
 * 
 */

// >>>>> File index <<<<<
// Here:   Modes, time, channel switches, debug
// 0_pins: Pin and encoder variables
// 0_vars: All other global variables

// 1_ch:   Turning ch1/ch2 LEDs on and off
// 1_mode: Mode switching functions
// 1_pind: Pin initilizatoin functions

// 2_cam:  Cam pulse functions
// 2_enc:  Encoder functions
// 2_extt: External TTL trigger functions
// 2_food: Food/reward delivery functions
// 2_i2c: I2C functions
// 2_RNG:  RNG functions
// 2_sche: Scheduler functions
// 2_ser:  Serial functions

// 3_perf: Performance/speed check
//

// =============== Version ===============
#define nsver "v3.56"

// =============== Hardware ===============
// Now running teensy 4.0 at 450 Mhz (2.2 ns step)
#if defined(__IMXRT1062__)
extern "C" uint32_t set_arm_clock(uint32_t frequency);
#define cpu_speed 450000000
#endif

// ================ PCB ================
#define PCB true
#define TeensyTester false//
 
// =============== Debug ===============
#define debugmode false // Master switch for all serial debugging
#define serialdebug false // This is the same as above but for MATLAB serial debugging
#define showpulses false // Extremely verbose. Dependent on debugmode or serialdebug to be true
#define showopto true // Dependent on debugmode or serialdebug to be true
#define showscheduler true // Dependent on debugmode or serialdebug to be true
#define showfoodttl true // Dependent on debugmode or serialdebug to be true
#define debugpins TeensyTester // This is set to follow TeensyTester. It is independent of debugmode/serialdebug
#define perfcheck false // Checking cycle time. Independent of debugmode/serialdebug
unsigned long ttest1 = 0;
bool ftest1 = false;

// =============== Modes ===============
// Photometry mode
bool optophotommode = false; // Green + red 
bool tcpmode = true; //
bool samecoloroptomode = false; // Green + blue (same led for photometry and opto);
bool useencoder = true;
bool usefoodpulses = false; //
bool syncaudio = false;
bool usescheduler = false; //false
bool manualscheduleoverride = false; // Only works when using scheduler (keep false if the opto input is left floating)
bool listenmode = false;
bool usecue = false; //
bool foodttlconditional = false; //
bool useRNG = false; // RNG for opto
bool randomiti = false; // Randomize ITI

// =============== Time ===============
// General time variables
unsigned long int tnow, tnowmillis;

// photometry time variables
unsigned long int t0; // When each cycle begins
unsigned long int t1; // Time in each cycle
unsigned int pulsewidth_1 = 6000; // in micro secs (ch1)
unsigned int pulsewidth_2 = 6000; // in micro secs (ch2)
unsigned long cycletime_photom_1; // in micro secs (Ch1)
unsigned long cycletime_photom_2; // in micro secs (Ch2)

// =============== Switches ===============
// Channel switches
bool ch1_on = false;
bool ch2_on = false;

// Food task switches
bool foodttlarmed = false;
bool cueon = false;

// Scheduler switches
bool inpreopto = true;
bool inopto = false;
bool inpostopto = false;
bool stimenabled = true;
bool schedulerrunning = false;

void setup() {
  // Set clock speed
  set_arm_clock(500000000);
  
  // Serial
  Serial.begin(19200);

  // Initialize RNG
  rng_init();

  // Initialize encoder
  enc_ini();

  // Initialize pins
  pin_ini();

  // Load up the variables
  modeswitch();

  // Initialize i2c
  i2c_init();
  
  if (debugmode){
    Serial.println("Experiment start.");  
    showpara();
  }

  // Scheduler flag initialize
  sche_ini();
  
  t0 = micros();
}

void loop() {
  tnow = micros();
  tnowmillis = tnow / 1000;

  // Performance check
  #if perfcheck
    perfc();
  #endif

  // Serial in
  if (Serial.available() >= 2){
    // Parse serial
    parseserial();
  }
  
  // Cam
  camerapulse();

  // Food ttl
  if (usefoodpulses && (foodttlarmed || cueon)){
    // Enter when needing to finish cueue too
    foodttl();
  }

  // Slow serial echo of trial and RNG ifo
  slowserialecho();
  
  // photometry
  t1 = tnow - t0;

  // ======================================== P1 ========================================
  // 1. Start a new cycle (once every 20 ms)
  if ((t1 >= (cycletime_photom_1 + cycletime_photom_2))){
    // Debug ch1 on to ch1 on
    // Serial.println(tnow - ttest1);
    // ttest1 = tnow;
    
    // This part happens once every 20 ms
    t0 = micros();

    // TCP or optophotometry mode. Turn on Ch1
    if (tcpmode || optophotommode){
      turn_ch1_on();
    }

    // Overflow
    // optophotometry mode or scoptophotometry mode, turn off stim if the stim period is the same as the cycle (finish the previous cycle)
    if (optophotommode && ch2_on){
      optophoto_ov();
    }
    else if (samecoloroptomode && ch1_on && (inopto || !schedulerrunning)){
      // Basically you can only get in here during the stim cycles, regular photometry pulsewidths are too short
      scoptophoto_ov();
    }
    
    // Manual stim
    if (manualscheduleoverride && usescheduler){
      externalttltrig();
    }

    // Doing the preopto period and the transition to inopto
    // Enable stim (now also true for tcp)
    sche_preopto();

    // Arm foodTTL in the food-then-opto mode (TCP/Opto/scopto). Scheduler or not. 
    preopto_rev_arm();

    // ***tcp mode behavioral task***
    if (tcpmode){
      tcp_p1();
    }
    
    
    // ***scopto mode***
    if (samecoloroptomode){
      scoptophoto_p1();
    }

    // ***Optophotometry***
    if (optophotommode){
      optophoto_p1();
    }
  }

  // ======================================== P2 ========================================
  // 2. First pulse ends, gap before second pulse
  else if ((t1 >= (pulsewidth_1)) && (t1 <= (cycletime_photom_1))){
    // Happens multiple times per cycle (twice actually for uno, might be getting close to the limit)
    // Serial.println(t1);
    
    // Always happen
    if (ch1_on){
      turn_ch1_off();
      
      // Same color opto
      if (samecoloroptomode){
        scoptophoto_p2();
      }
    }
  }

  // ======================================== P3 ========================================
  // 3. Second pulse starts
  else if ((t1 >= (cycletime_photom_1)) && (t1 < (cycletime_photom_1 + pulsewidth_2))){
    if (tcpmode){
      // tcp
      if (!ch2_on){
        turn_ch2_on();

        // debug ch2 on to ch2 on
//        Serial.println(tnow - ttest1);
//        ttest1 = tnow;
      }
    }
    // Opto photometry
    else if (optophotommode){
      optophoto_p3();
    }
  }

  // ======================================== P4 ========================================
  // 4. Second pulse ends. Wait for new cycle to start
  else if (t1 >= (cycletime_photom_1 + pulsewidth_2)){
    // tcp and the other thing
    if (ch2_on){
      // Serial.println(tnow - ttest1); Debug pulse width
      turn_ch2_off();

      if (optophotommode){
        optophoto_p4();
      }
    }
  }
  
// delayMicroseconds(step_size);
// Serial.println(micros() - tnow);
}
