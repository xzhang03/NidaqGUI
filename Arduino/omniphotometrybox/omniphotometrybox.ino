// Third Gen photometry box: Nanosec v3.2
// Handle almost everything you need for photometry
// Needs a decent number of pins
// Stephen Zhang

/* 1. Two color photometry
 * 2. Optophotometry (switch on)
 * 3. Same-color optophotometry (analogy input control intensity)
 * 4. Arbitrary photometry and optogenetic frequency
 * 5. Cam pulses with adjustable frequency
 * 6. Encoder at the same frequency as 4
 * 7. Preprogram experiment timescale: baseline -> opto -> postopto
 * 8. External input for scheduler (6)
 * 9. TTL pulses for other parts of the experiment
 * 10. Audio syncing
 * 11.Hardware random number generator for opto trials
 * 12.Online serial echo of trial and RNG infomation
 * 
 */

// ================ PCB ================
#define PCB true
#define TeensyTester false //
 
// =============== Debug ===============
#define debugmode false // Master switch for all serial debugging
#define serialdebug false // This is the same as above but for MATLAB serial debugging
#define showpulses false // Extremely verbose
#define showopto true
#define showscheduler true
#define showfoodttl true
#define debugpins false // This is independent of debugmode

unsigned long ttest1 = 0;
bool ftest1 = false;

// ============ Performance check ============
#define perfcheck false
#if perfcheck
  unsigned long int tnow_pc;
  unsigned long int cycles_pc = 1000000;
  bool pc_going = false;
#endif
unsigned long int ix_pc = 0;
long tpc;


// ============== Encoder ==============
//#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
#if PCB
  // PCB
  Encoder myEnc(16,17); // pick your pins, reverse for sign flip
#else
  // Proto
  Encoder myEnc(18,19); // pick your pins, reverse for sign flip
#endif

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
bool usebuzzcue = false; //
bool foodttlconditional = false; //
bool useRNG = false; // RNG for opto
bool randomiti = false; // Randomize ITI


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
  const byte schedulerpin = 7; // On when scheduler is used
  const byte preoptopin = 8; // preopto
  const byte inoptopin = 11; // preopto
  const byte postoptopin = 12; // preopto
#endif
bool serialpinon = false;

// =============== Time ===============
// General time variables
unsigned long int tnow, tnowmillis;

// ====== Camera, audio, and encoder ======
// time variables for camera
unsigned long int pulsetime = 0;
unsigned int step_size = 10; // in micros

// cam trig variables
bool pulsing = false;
byte freq = 30; // Pulse at 30 Hz (default)
unsigned int ontime = 2000; // On for 2 ms
unsigned long cycletime = 1000000 / freq;
bool onoff = false;
long pos = 0;

// Audio
unsigned int audiofreq = 2000;

// ============== Serial ==============
byte m, n;

// number of pulses
long pcount = 0;
byte echo[4] = {0, 0, 0, 0};
bool autoencserial = true; // Auto serial relay encoder info
bool echoencoderc = true; // Echo encoder counter at the end
bool autoechotrialinfo = false; // Auto echo scheduler and RNG info on a slow clock
unsigned long int cycletime_slow = 1000; // slow echo time in ms
unsigned long int echotime_slow;

// ============ Photometry ============
// photometry time variables
unsigned long int t0; // When each cycle begins
unsigned long int t1; // Time in each cycle
unsigned int pulsewidth_1 = 6000; // in micro secs (ch1)
unsigned int pulsewidth_2 = 6000; // in micro secs (ch2)
unsigned long cycletime_photom_1; // in micro secs (Ch1)
unsigned long cycletime_photom_2; // in micro secs (Ch2)
float fps = 50; // Points per second (just say fps colloquially)

// tcp photometry time variables
const int pulsewidth_1_tcp = 6000; // in micro secs (ch2)
const int pulsewidth_2_tcp = 6000; // in micro secs (ch2)
unsigned long cycletime_photom_1_tcp = 10000; // in micro secs (Ch1)
unsigned long cycletime_photom_2_tcp = 10000; // in micro secs (Ch2)

// ============ Opto ============
// Opto varaibles
byte opto_per = 5; // Number of photometry pulses per opto pulse (A). Can be: 1, 2, 5, 10, 25, 50. Pulse freq is 50 / A
byte train_length = 10; // Number of opto pulses per train (B). Duration is  B / (50 / A).
unsigned long train_cycle = 30 * 50; // First number is in seconds. How often does the train come on.
int pulsewidth_2_opto = 10000; // in micro secs (ch2)
unsigned long cycletime_photom_1_opto = 6500; // in micro secs (Ch1)
unsigned long cycletime_photom_2_opto = 13500; // in micro secs (Ch2)
bool optophotooverlap = false; // When optophotooverlap is on, opto pulse and photometry pulse start at the same time, otherwise, opto pulse starts after cycl1time_photom_1_opto.

// Same color opto variables
byte scopto_per = 5; // Number of photometry pulses per opto pulse (AO). Can be: 1, 2, 5, 10, 25, 50. Pulse freq is 50 / AO
byte sctrain_length = 10; // Number of photometry pulses per stim period (BO). Duration is  BO / (50 / AO).
unsigned long sctrain_cycle = 30 * 50; // First number is in seconds. How often does the train come on. 
unsigned int pulsewidth_1_scopto = 10000; // in micro secs (ch1)
const unsigned long cycletime_photom_1_scopto = 20000; // in micro secs (Ch1)
const unsigned long cycletime_photom_2_scopto = 0; // in micro secs (Ch2). Irrelevant
bool tristatepinpol = false; // Polarity for the tristatepin (1 = active high, 0 = active low). Default active low.

// TCP (no real opto for behavioral purpose only)
unsigned long tcptrain_cycle = 30 * 50; // First number is in seconds. How often does the train come on.

// ============ Scheduler ============
unsigned int preoptotime = 120; //in seconds (max is high because unsigned int is 32 bit for teensy)
unsigned int npreoptopulse = preoptotime * 50; // preopto pulse number
unsigned int ipreoptopulse = 0; // preopto pulse number
unsigned int ntrain = 10; // Number of trains
byte itrain = 0; // Current number of trains
bool inpreopto = true;
bool inopto = false;
bool inpostopto = false;
bool stimenabled = true;
bool schedulerrunning = false;
bool manualon = false;
bool listenpol = true; // Polarity for listen mode false = active low, true = active high. Either way, it has to be 3.3V logic

// ============ Hardware RNG ============
// RNG is only used in scheduler mode (and not in listening mode)
// RNG is used to control whether a train of stimulation (optophotometry or scoptophotometry) passes
// RNG does not affect the food TTL
#include <Entropy.h>
#define maxrngind 256 // Max 256 trials
const byte maxrng = 100; // Max value RNG = (0 - X)
byte threshrng = 30; // Below this is pass (0 - thresh)
byte rngvec[maxrngind]; // Initialize array
bool trainpass = true;

// Randomize ITI
byte rng_cycle_min = 30; // Min cycle when ITI is randomized (in seconds, assuming 20 ms pulse cycle)
byte rng_cycle_max = 40; // Max cycle when ITI is randomized (in seconds, assuming 20 ms pulse cycle)
byte rngvec_ITI[maxrngind]; // Initialize array for RNG ITI

// ============ Food TTL ============
// Food TTL (basically sync'ed with opto)
unsigned int nfoodpulsedelay = 2000; // Time after opto pulse train onset in ms (applies to both opto-then-food and food-then-opto)
unsigned int foodpulse_ontime = 150; // in ms
unsigned int foodpulse_cycletime = 300; // in ms
unsigned int nfoodpulsedelay_complement = 4 * 50; // Pulses before pulse train onset in ms (applies to food-then-opto). Note this shifts early the time0 of when foodttl arming happens
byte foodpulses = 5; // Number of food ttl pulses per stim period.
byte foodpulses_left = 0; // Try counting down this time. Probably easier to debug
unsigned long tfood0, tfood1;
bool optothenfood = true; // Set false is foodthenopto
bool inputttl = false;
bool foodttlarmed = false;
bool foodttlwait = false; // In waiting for delivery
bool foodttlon = false;

// ======== Food TTL Conditional ========
// Cue: Opto start => buzzer delay => buzzer duration
// Action: Opto start => action delay => action window
// Delivery: Opto start => delivery delay => delivery
// Example: 
/*  1. Opto start
 *  2. 2s
 *  3. Cue start, Action window start, potential delivery start.
 *  4. 1s
 *  5. Cue stop
 *  6. 4s
 *  7. Action window stop
 *  8. potential delivery stop.
 */

bool foodttlcuewait = false;
bool foodttlactionwait = false;
unsigned int buzzdelay = 2000; //
unsigned int buzzdur = 1000; // Time for buzzer cue 
unsigned int actiondelay = 2000;
unsigned int actiondur = 5000;
unsigned int deliverydur = 5000;
bool cueon = false;
bool actionperiodon = false;


// ============== I2c ==============
bool usei2c = false;

// ============ Switches ============
// photometry and optophotometry switches
bool ch1_on = false;
bool ch2_on = false;
bool opto = false;
bool train = false;
bool lastpulseopto = false; // Last pulse coming up

// ============ Counters ============
// Counters
byte pmt_counter_for_opto = 0;
byte opto_counter = 0;
unsigned long counter_for_train = 0; // Number of cycles
unsigned long counter_for_train_complement = 0;

void setup() {
  // Serial
  Serial.begin(19200);

  // Initialize RNG
  Entropy.Initialize();
  
  if (useencoder){
    myEnc.write(0);
  }

  // Essential pin
  pinMode(cam_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(ch1_pin, OUTPUT);
  pinMode(ch2_pin, OUTPUT);
  pinMode(tristatepin, OUTPUT);
  pinMode(foodTTLpin, OUTPUT);
  pinMode(audiopin, OUTPUT);

  if (listenpol){
    pinMode(switchpin, INPUT_PULLDOWN); // active high
  }
  else {
    pinMode(switchpin, INPUT_PULLUP); // active low
  }
  
  pinMode(foodTTLinput, INPUT_PULLDOWN);
  
  digitalWrite(cam_pin, LOW);
  digitalWrite(led_pin, LOW);
  digitalWrite(ch1_pin, LOW);
  digitalWrite(ch2_pin, LOW);
  digitalWrite(foodTTLpin, LOW);

  // Debug pin
  pinMode(serialpin, OUTPUT); // Parity signal for serial pin
  pinMode(schedulerpin, OUTPUT); // On when scheduler is used
  pinMode(preoptopin, OUTPUT); // preopto
  pinMode(inoptopin, OUTPUT); // preopto
  pinMode(postoptopin, OUTPUT); // preopto

  digitalWrite(serialpin, LOW);
  digitalWrite(schedulerpin, LOW);
  digitalWrite(preoptopin, LOW);
  digitalWrite(inoptopin, LOW);
  digitalWrite(postoptopin, LOW);

  // Load up the variables
  modeswitch();
  
  if (debugmode){
    Serial.println("Experiment start.");  
    showpara();
  }

  // If scheduler is used. disable stims
  stimenabled = !usescheduler;
  schedulerrunning = false;
  inpreopto = true;
  inopto = false;
  inpostopto = false;
  if (debugpins && usescheduler){
    digitalWrite(schedulerpin, HIGH);
    digitalWrite(preoptopin, HIGH);
  }
  
  t0 = micros();
}

void loop() {
  tnow = micros();
  tnowmillis = tnow / 1000;

  // Performance check code (comment out if not doing this)
  /*
  if (pulsing & perfcheck) {
    if ((ipreoptopulse == 0) && !pc_going){
      // Very first cycle during pulsing
      ix_pc = 0;
      tnow_pc = tnow;
      pc_going = true;
    }
    else if ((ix_pc < cycles_pc) && pc_going){
      ix_pc++;
    }
    else if (pc_going){
      tpc = tnow - tnow_pc;
      pc_going = false;
    }
  }
  */
  
  if (Serial.available() >= 2){
    // Read 2 bytes
    m = Serial.read();
    n = Serial.read();
    
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
  
  // 1. Start a new cycle (once every 20 ms)
  if ((t1 >= (cycletime_photom_1 + cycletime_photom_2))){
    // Debug ch1 on to ch1 on
    // Serial.println(tnow - ttest1);
    // ttest1 = tnow;
    
    // This part happens once every 20 ms
    t0 = micros();

    // TCP or optophotometry mode. Turn on Ch1
    if (tcpmode || optophotommode){
      digitalWrite(ch1_pin, HIGH);
      ch1_on = true;
      // Serial.println(t1); Confirm only once every 20 ms
    }

    // Overflow
    // optophotometry mode or scoptophotometry mode, turn off stim if the stim period is the same as the cycle (finish the previous cycle)
    if (optophotommode && ch2_on){
      digitalWrite(ch2_pin, LOW);
      ch2_on = false; 


      if ((opto_counter >= train_length) && optophotommode && train){
        // reset opto counter
        train = false;
        
        if ((debugmode || serialdebug) && showopto){
          Serial.println("Train off");
        }
      }
      
      // Scheduler disable
      if ((itrain >= ntrain) && stimenabled && inopto && schedulerrunning){
        schedulerdisable();
      }
    }
    else if (samecoloroptomode && ch1_on && (inopto || !schedulerrunning)){
      // Basically you can only get in here during the stim cycles, regular photometry pulsewidths are too short
      digitalWrite(ch1_pin, LOW);
      ch1_on = false;

      if (lastpulseopto){
        lastpulseopto = false;

        // Return to normal pulse width after the train is off. Only at last pulse
        pulsewidth_1 = pulsewidth_1_tcp;
        digitalWrite(AOpin, LOW);
        digitalWrite(tristatepin, !tristatepinpol); // !false = active low = off

        // Scheduler disable
        if ((itrain >= ntrain) && stimenabled && inopto && schedulerrunning){
          schedulerdisable();
        }
      }
    }
    

    // Manual stim
    if (manualscheduleoverride && usescheduler){
      externalttltrig();
    }


    // Doing the preopto period and the transition to inopto
    // Enable stim (now also true for tcp)
    if ((ipreoptopulse >= npreoptopulse) && (!stimenabled) && inpreopto && schedulerrunning){
      stimenabled = true;
      inpreopto = false;
      inopto = true;

      if (debugpins){
        digitalWrite(preoptopin, LOW);
        digitalWrite(inoptopin, HIGH);
        digitalWrite(postoptopin, LOW);
      }
      
      if ((debugmode || serialdebug) && showscheduler){
        Serial.println("Stim is enabled. Entering opto phase.");
      }
    }
    // Just advance
    else if (schedulerrunning && inpreopto && (!stimenabled) && (!listenmode)){
      ipreoptopulse++;

      // For food -> opto experiments, counting down to when the first foodttlarm comes out (first trains starts at 0);
      counter_for_train_complement = npreoptopulse - ipreoptopulse;
      
      if ((debugmode || serialdebug) && showscheduler){
        if ((ipreoptopulse % 50) == 0){
            Serial.print("Preopto pulse #");
            Serial.print(ipreoptopulse);
            Serial.print("/");
            Serial.println(npreoptopulse);
        }
      }
    }

    // Arm foodTTL in the food-then-opto mode (TCP/Opto/scopto). Scheduler or not. 
    if ((!optothenfood) && (usefoodpulses) && (pulsing) && (counter_for_train_complement == nfoodpulsedelay_complement)){
      // Only applies to food-then-opto mode here
      armfoodttl(); // Arm food ttl
    }
    /*
    else if ((debugmode) && (showfoodttl) && (!optothenfood) && (usefoodpulses) && (pulsing)){
      if ((counter_for_train_complement % 50) == 0){
        Serial.print(counter_for_train_complement);
        Serial.println(" pulses until food arming.");
      }
    }
    */

    // ***tcp mode behavioral task***
    if (tcpmode){
      // Advance counters
      // Scheduler allows train and opto counters to advance
      if (stimenabled){
        counter_for_train_complement = tcptrain_cycle - counter_for_train; // Counting 1499-0. 0 when train starts
        counter_for_train++; // Counting 1-1500, on 1 food task is on
      }
      
      if ((debugmode || serialdebug) && showpulses){
        Serial.print("Photometry ");
        Serial.println(counter_for_train);
      }

      if ((counter_for_train == 1) && stimenabled){
        // Once per train
        if (usefoodpulses && optothenfood && pulsing){
          // Only applies to opto-then-food here
          armfoodttl(); // Arm food ttl
        }

        if (schedulerrunning){
          if (randomiti){
            // Get randomized ITI. This affects scopto.
            tcptrain_cycle = rngvec_ITI[itrain] * fps;
          }
          
          itrain++; // Advance train count
  
          if ((debugmode || serialdebug) && showscheduler){
            Serial.print("Scheduler train #");
            Serial.print(itrain);
            Serial.print("/");
            Serial.println(ntrain);
  
            if (randomiti){
              Serial.print("RNG says next cycle is (s): ");
              Serial.println(tcptrain_cycle / fps);
            }
          }
        }
      }

      if (counter_for_train >= tcptrain_cycle){
        counter_for_train = 0;
      }

      // Scheduler disable
      if ((itrain >= ntrain) && stimenabled && inopto && schedulerrunning){
        schedulerdisable();
      }
    }
    
    
    // ***scopto mode***
    if (samecoloroptomode){
      // Advance counters
      // Scheduler allows train and opto counters to advance
      if (stimenabled){
        counter_for_train_complement = sctrain_cycle - counter_for_train; // Counting 1499-0. 0 when train starts
        pmt_counter_for_opto++; // Counting 1-5, on 1 opto is on
        counter_for_train++; // Counting 1-1500, on 1 train is on
      }
      
      if ((debugmode || serialdebug) && showpulses){
        Serial.print("Photometry ");
        Serial.print(counter_for_train);
        Serial.print(" - ");
        Serial.println(pmt_counter_for_opto);
      }

      // Beginning of each train cycle. Turn train mode on.
      // Only if stim is enabled though
      if ((counter_for_train == 1) && stimenabled){
        // Debug train on to train on
//        Serial.println(tnow - ttest1);s
//        ttest1 = tnow;
        
        train = true;
        opto_counter = 0;
                
        if (usescheduler){
          // In scheduler and current train is on
          // This affects scopto
          if (useRNG){
            // Figure out if current train passes
            trainpass = rngvec[itrain] < threshrng;
          }

          if (randomiti){
            // Get randomized ITI. This affects scopto.
            sctrain_cycle = rngvec_ITI[itrain] * fps;
          }
          
          itrain++; // Advance train count

          if ((debugmode || serialdebug) && showscheduler){
            Serial.print("Scheduler train #");
            Serial.print(itrain);
            Serial.print("/");
            Serial.println(ntrain);

            if (useRNG){
              Serial.print("RNG says train pass = ");
              Serial.println(trainpass);
            }

            if (randomiti){
              Serial.print("RNG says next cycle is (s): ");
              Serial.println(sctrain_cycle / fps);
            }
          }
        }

        // RNG (only in scheduler mode and useRNG is on can RNG be considered)
        if ((!usescheduler) || (!useRNG) || (trainpass)){
          // schedule mode is off or not using RNG or train passed anyway
          // In other words, this step is skipped when schedulemode = 1 && useRNG = 1 && and trainpass = 0
          // If train pass we set analog output high
          pulsewidth_1 = pulsewidth_1_scopto; // Change pulse width
          digitalWrite(AOpin, HIGH);
          digitalWrite(tristatepin, tristatepinpol); // false = active low = on.
//          Serial.println("OPTO actually ON");
        }
        
        if (usefoodpulses && optothenfood && pulsing){
          // Only applies to opto-then-food here
          armfoodttl(); // Arm food ttl
        }
        
        if ((debugmode || serialdebug) && showopto){
          Serial.println("Train on");
          Serial.println("Analog on");
          Serial.println("Transceiver on");
          Serial.println("Ch1 pulse width set to scopto");
        }
      }

      // Turn on opto mode
      if (train){
        if (pmt_counter_for_opto==1){
          // Debug opto on to opto on
//          Serial.println(tnow - ttest1);
//          ttest1 = tnow;
          
          opto = true;
        }
      }
      else {
        // When no train just regular photometry
        digitalWrite(ch1_pin, HIGH);
        ch1_on = true;
      }

      // Turn on light
      if (opto){
//        Serial.println("OPTO actually ON");
        digitalWrite(ch1_pin, HIGH);
        
        ch1_on = true;
        opto_counter++;
        opto = false;

        // debug ch1 on to ch1 off for opto
        if ((debugmode || serialdebug) && showopto){
          ttest1 = tnow;
          ftest1 = true;
        }
      }

      // Reset counter
      // reset pmt counter
      if (pmt_counter_for_opto >= scopto_per){
        pmt_counter_for_opto = 0;
      }
      
      // reset opto counter
      if ((opto_counter >= sctrain_length) && train){
        train = false;
        lastpulseopto = true;
      }
    
      // Serial.println(counter_for_train);
      // reset train counter
      if (counter_for_train >= sctrain_cycle){
        counter_for_train = 0;
      }
    }

    // ***Optophotometry***
    if (optophotommode){
      // Advance counters (not using scheduler or stim is enabled)
      if (stimenabled){
        counter_for_train_complement = train_cycle - counter_for_train; // Counting 1499-0. 0 when train starts
        pmt_counter_for_opto++; // Counting 1-5, on 1 opto is on
        counter_for_train++; // Counting 1-1500, on 1 train is on
      }
      
      if ((debugmode || serialdebug) && showpulses){
        Serial.print("Photometry ");
        Serial.print(counter_for_train);
        Serial.print(" - ");
        Serial.println(pmt_counter_for_opto);
      }

      // Beginning of each train cycle. Turn train mode on (if stim is enabled).
      if ((counter_for_train == 1) && stimenabled){
        // Debug train on to train on
//        Serial.println(tnow - ttest1);
//        ttest1 = tnow;
        
        train = true;
        opto_counter = 0;

        if (usescheduler){
          // In scheduler and RNG mode is on
          // This affects optophotometry only
          if (useRNG){
            trainpass = rngvec[itrain] < threshrng;
          }

          if (randomiti){
            // Get randomized ITI. This affects scopto.
            train_cycle = rngvec_ITI[itrain] * fps;
          }
          
          itrain++; // Advance train count

          if ((debugmode || serialdebug) && showscheduler){
            Serial.print("Scheduler train #");
            Serial.print(itrain);
            Serial.print("/");
            Serial.println(ntrain);

            if (useRNG){
              Serial.print("RNG says train pass = ");
              Serial.println(trainpass);
            }

            if (randomiti){
              Serial.print("RNG says next cycle is (s): ");
              Serial.println(train_cycle / fps);
            }
          }
        }

        if (usefoodpulses && optothenfood && pulsing){
          // Only applies to opto-then-food here
          armfoodttl(); // Arm food ttl
        }
        
        if ((debugmode || serialdebug) && showopto){
          Serial.println("Train on");
        }
      }
    
      // Turn on opto mode
      if (train){
        if (pmt_counter_for_opto==1){
          // Debug opto on to opto on
//          Serial.println(tnow - ttest1);
//          ttest1 = tnow;
          
          opto = true;
        }
      }

      // reset pmt counter
      if (pmt_counter_for_opto >= opto_per){
        pmt_counter_for_opto = 0;
      }
      
      // Serial.println(counter_for_train);
      // reset train counter
      if (counter_for_train >= train_cycle){
        counter_for_train = 0;
      }

      // Overlap mode (sending opto pulse now)
      if ((optophotooverlap) && (opto) && (!ch2_on)){
        // RNG (only in scheduler mode and useRNG is on can RNG be considered)
        if ((!usescheduler) || (!useRNG) || (trainpass)){
          // schedule mode is off or not using RNG or train passed anyway
          // In other words, this step is skipped when schedulemode = 1 && useRNG = 1 && and trainpass = 0
          digitalWrite(ch2_pin, HIGH);
        }
        ch2_on = true;
        opto_counter++;
        opto = false;
        if ((debugmode || serialdebug) && showopto){
          ttest1 = tnow;
          ftest1 = true;
        }
      }
    }
  }
  
  // 2. First pulse ends, gap before second pulse
  else if ((t1 >= (pulsewidth_1)) && (t1 < (cycletime_photom_1))){
    // Happens multiple times per cycle (twice actually for uno, might be getting close to the limit)
    // Serial.println(t1);
    
    // Always happen
    if (ch1_on){
      digitalWrite(ch1_pin, LOW);
      ch1_on = false;

      // debug ch1 on to ch1 off for opto
      if ((debugmode || serialdebug) && showopto){
        if (ftest1){
          Serial.print("Opto ");
          Serial.print(opto_counter);
          Serial.print(". Pulse width (us): ");
          Serial.print(tnow - ttest1);
          Serial.print(". At (ms): ");
          Serial.println(tnowmillis);
          ftest1 = false;
        }
      }
      
      // Same color opto
      if (lastpulseopto && samecoloroptomode){
        lastpulseopto = false;

        // Return to normal pulse width after the train is off. Only at last pulse
        pulsewidth_1 = pulsewidth_1_tcp;
        digitalWrite(AOpin, LOW);
        digitalWrite(tristatepin, !tristatepinpol); // !false = active low = off

        if ((debugmode || serialdebug) && showopto){
          Serial.println("Train off");
        }
        
        if ((debugmode || serialdebug) && showopto){
          Serial.println("Ch1 pulse width returned to tcp");
          Serial.println("Analog off");
          Serial.println("Transceiver off");
        }
        
        // Scheduler disable
        if ((itrain >= ntrain) && stimenabled && inopto && schedulerrunning){
          schedulerdisable();
        }   
      }
    }
  }
  
  // 3. Second pulse starts
  else if ((t1 >= (cycletime_photom_1)) && (t1 < (cycletime_photom_1 + pulsewidth_2))){
    if (tcpmode){
      // tcp
      if (!ch2_on){
        digitalWrite(ch2_pin, HIGH);
        ch2_on = true;

        // debug ch2 on to ch2 on
//        Serial.println(tnow - ttest1);
//        ttest1 = tnow;
      }
    }
    // Opto photometry
    else if (optophotommode && (!optophotooverlap)){
      // optophotometry and not-overlap mode (overlapping means the opto pulse is sent earlier with photometry pulse)
      if ((opto) && (!ch2_on)){
        // RNG (only in scheduler mode and useRNG is on can RNG be considered)
        if ((!usescheduler) || (!useRNG) || (trainpass)){
          // schedule mode is off or not using RNG or train passed anyway
          // In other words, this step is skipped when schedulemode = 1 && useRNG = 1 && and trainpass = 0
          digitalWrite(ch2_pin, HIGH);
//          Serial.println("OPTO actually ON");
        }
        ch2_on = true;
        opto_counter++;
        opto = false;
        if ((debugmode || serialdebug) && showopto){
          ttest1 = tnow;
          ftest1 = true;
        }
      }
    }
  }
  
  // 4. Second pulse ends. Wait for new cycle to start
  else if (t1 >= (cycletime_photom_1 + pulsewidth_2)){
    // tcp and the other thing
    if (ch2_on){
      // Serial.println(tnow - ttest1); Debug pulse width
      digitalWrite(ch2_pin, LOW);
      ch2_on = false;

      // debug ch2 on to ch2 off
      if ((debugmode || serialdebug) && showopto){
        if (ftest1 && train){
          Serial.print("Opto ");
          Serial.print(opto_counter);
          Serial.print(". Pulse width (us): ");
          Serial.print(tnow - ttest1);
          Serial.print(". At (ms): ");
          Serial.println(tnowmillis);
          ftest1 = false;
        }
      }

      
      if ((opto_counter >= train_length) && optophotommode && train){
        // reset opto counter
        train = false;
        
        if ((debugmode || serialdebug) && showopto){
          Serial.println("Train off");
        }

        // Scheduler disable
        if ((itrain >= ntrain) && stimenabled && inopto && schedulerrunning){
          schedulerdisable();
        }   
      }
    }
  }
  
delayMicroseconds(step_size);
// Serial.println(micros() - tnow);
}

// Parse serial
void parseserial(){
  if (debugmode){
    m = m - '0';
    n = n - '0';
  }

  // m table
  // ========== Operational ==========
  // 2: new cycle time (1000000/n, n in Hz)
  // 1: start pulse
  // 0: stop pulse
  // 5: Quad encoder position
  // 9: Show all parameters
  // 253: reboot (n = 104)
  // 255: status update (n = variable)
  
  // ========== Opto & photom ==========
  // 3: TCP mode (n = 0 TCP, n = 1 optophotometry, n = 2 samecolor optophotometry)
  // 6: Change opto frequency (opto_per = n, f = 50/n)
  // 7: Change opto pulse per train (train_length = n)
  // 8: Change opto train cycle (train_cycle = n * 50, n in seconds)
  // 59: Opto-photometry overlap (n = 1 yes, 0 no)
  // 10: Change scopto frequency (scopto_per = n, f = 50/n)
  // 11: Change scopto train cycle (train_cycle = n * 50)
  // 12: Change scopto pulse per train (train_length = n)
  // 13: Change scopto pulse width (n * 1000 us)
  // 14: Change Opto pulse width (n * 1000 us)
  // 29: Tristate pin polarity (1 = active high, 0 = active low);
  // 47: Change tcp behavioral train cycle (train_cycle = n * 50)
  // 36: Cycle 1 for optophotometry (only changed for pure optogenetic experiments; cycle = n * 100 us)
  // 37: Cycle 2 for optophotometry (only changed for pure optogenetic experiments; cycle = n * 100 us)
  // 55: Add Cycle 1 for optophotometry (cycle = cycle + n * 256 * 100 us);
  // 56: Add Cycle 2 for optophotometry (cycle = cycle + n * 256 * 100 us);
  // 53: Cycle 1 for TCP (only changed for pure TCP experiments; cycle = n * 100 us)
  // 54: Cycle 2 for TCP (only changed for pure TCP experiments; cycle = n * 100 us)
  // 57: Add Cycle 1 for TCP (cycle = cycle + n * 256 * 100 us);
  // 58: Add Cycle 2 for TCP (cycle = cycle + n * 256 * 100 us);
  
  
  // ============ Scheduler ============
  // 15: Use scheduler (n = 1 yes, 0 no) 
  // 4: Set delay (delay_cycle = n * 50, n = 1 means 1 second). Only relevant when scheduler is on
  // 52: Adding delay (n * 256 * 50, n = 1 means 256 second). Only relevant when scheduler is on
  // 16: Number of trains (n trains)
  // 46: Adding trains (trains = trains + n * 256)
  // 17: Enable manual scheduler override
  // 27: Listening mode on or off (n = 1 yes, 0 no). Will turn on manual override.
  // 28: Listen mode polarity (1 = active high, 0 = active low)
  
  // ============ Hardware RNG ============
  // 38: use RNG or not (n = 1 yes, 0 no)
  // 39: Pass chance in percent (30 = 30% pass chance)
  // 40: RNG ITI or not (n = 1 yes, 0 no)
  // 41: min randomized ITI (n * 1 s)
  // 42: max randomized ITI (n * 2 s)

  // ============ Food TTL ============
  // 24: Use Food TTL or not (n = 1 yes, 0 no) 
  // 18: Delay time after opto (n * 100 ms)
  // 50: Adding delay time after opto (n * 256 * 100 ms)
  // 19: Pulse on time (n * 10 ms)
  // 20: Pulse cycle time (n * 10 ms)
  // 21: Pulses per train (n)
  // 30: Use buzzer cue or not (n = 1 yes, 0 no)
  // 31: Buzzer delay (n * 100 ms)
  // 32: Buzzer duration (n * 100 ms)
  // 48: Opto then Food (n = 1 yes, 0 no)
  // 49: Lead time before opto (n s)
  // 51: Adding lead time before opto (+ n * 256 s) 

  // ====== Food TTL Conditional ======
  // 22: Conditional or not (n = 1 yes, 0 no) 
  // 33: Action period delay (n * 100 ms)
  // 34: Action period duration (n * 100 ms)
  // 35: Delivery period duration (n * 100 ms);
  
  // ============= Encoder =============
  // 23: Encoder useage (n = 1 yes, 0 no) 
  // 43: Auto encoder serial transmission (n = 1 yes, 0 no)

  // ============ Audio sync ============
  // 25: Audio sync (n = 1 yes, 0 no)
  // 26: Audio sync tone frequency (n * 100 Hz)

  // ========== Trial and RNG Echo ==========
  // 44: Auto trial and rng echo (n = 1 yes, 0 no)
  // 45: Auto trial and rng echo periodicity (n * 100 ms)
  
  
  switch (m){
    // Initialize echo
    
    case 255:
      echo[0] = 0;
      echo[1] = 0;
      echo[2] = 0;
      echo[3] = 0;
    
     // 255: status update (n = variable)
     switch (n){
      case 0:
        // Echo back scheduler info
        echo[3] = 1;
        
        // Scheduler
        if (!usescheduler){
          echo[2] = 1; // Echo back [0 0 1 1] as no scheduler
        }
        else if (inpreopto){
          echo[2] = 2; // Echo back [0 0 2 1] as preopto
        }
        else if (inpostopto){
          echo[2] = 3;; // Echo back [0 0 3 1] as postopto
        }
        else {
          echo[0] = itrain;
          echo[1] = ntrain; // first byte shows n train, second byte shows itrain
        }
        break;

      case 38:
        // Echo back rng info
        echo[3] = 2;
        echo[2] = useRNG; // Echo back [X X 0 2] as no RNG
        echo[1] = usescheduler; // Echo back [X 0 X 2] as no RNG
        echo[0] = trainpass;
        break;

      case 39:
        // Echo back pass chance
        echo[3] = 3;
        echo[0] = threshrng;
        break;
     }
     Serial.write(echo, 4);
     break;
     
    case 253:
      // Reboot
      if (n == 104){
        SCB_AIRCR = 0x05FA0004;
      }
      break;
      
    case 0:
      // End pulsing
      pulsing = false;
      train = false;

      if (samecoloroptomode){
        // return analog pin to high impedence and reset pulse width
        pulsewidth_1 = pulsewidth_1_tcp;
        digitalWrite(AOpin, LOW);
        digitalWrite(tristatepin, !tristatepinpol); // !false = active low = off
      }
      
      if (echoencoderc){
        Serial.write((byte *) &pcount, 4);
      }
      if (usescheduler){
        stimenabled = false;
        schedulerrunning = false;
      }
      if (usebuzzcue){
        noTone(audiopin);
      }
      if (perfcheck){
        // Tag
        echo[0] = 128;
        echo[1] = 128;
        echo[2] = 128;
        echo[3] = 128;
        Serial.write(echo, 4);
        Serial.write((byte *) &ix_pc, 4);
        Serial.write((byte *) &tpc, 4);
      }
      
      if (debugmode){
        Serial.println("Cam pulse stop.");
      }
      break;
      
    case 1:
      // Start pulsing
      pulsing = true;
      pulsetime = tnow;
      echotime_slow = tnowmillis;

      // Train/opto related variables (relavant with or without scheduler)
      pmt_counter_for_opto = 0;
      opto_counter = 0;
      counter_for_train = 0; // Number of cycles

      // Food TTL related variables (relavant with or without scheduler)
      foodpulses_left = 0;
      inputttl = !foodttlconditional;
      foodttlarmed = false;
      foodttlwait = false;
      foodttlon = false;

      if (foodttlconditional){
        foodttlcuewait = false;
        foodttlactionwait = false;
        cueon = false;
        actionperiodon = false;
      }
      
      if (debugmode){
        Serial.println("Cam pulse start.");
      }
      if (useencoder){
        myEnc.write(0);    // zero the position
        pos = 0;
        pcount = 0;  
      }

      // Scheduler reset
      if (usescheduler){
        ipreoptopulse = 0;
        itrain = 0;
        inpreopto = true;
        inopto = false;
        inpostopto = false;
        stimenabled = false;
        schedulerrunning = true;
        counter_for_train_complement = npreoptopulse;
        
        // Opto RNG
        if (useRNG){
          // Generate array
          rng(rngvec, maxrng, 0, maxrngind);
        }

        // RNG ITI
        if (randomiti){
          // Generate array
          rng(rngvec_ITI, rng_cycle_max , rng_cycle_min, maxrngind);
        }
        
        if (debugpins){
          digitalWrite(schedulerpin, HIGH);
          digitalWrite(preoptopin, HIGH);
          digitalWrite(inoptopin, LOW);
          digitalWrite(postoptopin, LOW);
        }
      }
      else{
        stimenabled = true;
        inpreopto = false;

        // Complement counter (no scheduler)
        if (tcpmode){
          counter_for_train_complement = tcptrain_cycle;
        }
        else if (samecoloroptomode){
          counter_for_train_complement = sctrain_cycle;
        }
        else if(optophotommode){
          counter_for_train_complement = train_cycle;
        }
        
        
        if (debugpins){
          digitalWrite(schedulerpin, LOW);
          digitalWrite(preoptopin, LOW);
          digitalWrite(inoptopin, LOW);
          digitalWrite(postoptopin, LOW);
        }      
      }
      break;

    case 2:
      // Set frequency
      freq = n;
      cycletime = 1000000 / n;

      if (debugmode){
        Serial.print("New cam frequency (Hz): ");
        Serial.println(n);
        Serial.print("New cam cycle time (ms): ");
        Serial.println(cycletime/1000);
      }
      break;

    case 3:
      // tcp mode
      if (n == 0){
        // tcp
        optophotommode = false;
        tcpmode = true;
        samecoloroptomode = false;
      }
      else if (n == 1){
        // optophotometry
        optophotommode = true;
        tcpmode = false;
        samecoloroptomode = false;
      }
      else if (n == 2){
        // same color optophotometry
        optophotommode = false;
        tcpmode = false;
        samecoloroptomode = true;
      }
      modeswitch();
      break;

    case 4:
      // 4: Set delay (delay_cycle = n * 50, n = 1 means 1 seconds). Only relevant when scheduler is on
      preoptotime = n; // in seconds (max is high because npreoptopulse is unsigned int which is 32 bits)
      npreoptopulse = preoptotime * fps; // preopto pulse number
      ipreoptopulse = 0;
      itrain = 0;
      break;
        
    case 5:
      // Give position
      if (useencoder){
        pos = myEnc.read();
        //      Serial.println(pos);      
        Serial.write((byte *) &pos, 4);
      }
      else{
        pos = 0;
        //      Serial.println(pos);      
        Serial.write((byte *) &pos, 4);
      }
      break;
      
    case 6:
      // 6: Change opto frequency (opto_per = n)
      opto_per = n;

      if (debugmode){
        Serial.print("New opto freq (Hz): ");
        Serial.println(fps / opto_per);
      }
      break;
      
    case 7:
      // 7: Change opto pulse per train (train_length = n)
      train_length = n;
      if (debugmode){
        Serial.print("New opto pulses per train: ");
        Serial.println(train_length);
      }
      break;
      
    case 8:
      // 8: Change opto train cycle (train_cycle = n * 50)
      train_cycle = n * fps;
      if (debugmode){
        Serial.print("New opto train cycle (s): ");
        Serial.println(train_cycle / fps);
      }
      break;

    case 9:
      // List parameters
      showpara();
      break;

    case 10:
      // 10: Change scopto frequency (scopto_per = n, f = 50/n)
      scopto_per = n;

      if (debugmode){
        Serial.print("New scopto freq (Hz): ");
        Serial.println(fps / opto_per);
      }
      break;

    case 11:
      // 11: Change scopto train cycle (train_cycle = n * 50)
      sctrain_cycle = n * fps;
      if (debugmode){
        Serial.print("New scopto train cycle (s): ");
        Serial.println(sctrain_cycle / fps);
      }
      break;

    case 12:
      // 12: Change scopto pulse per train (train_length = n)
      sctrain_length  = n;
      if (debugmode){
        Serial.print("New scopto pulses per train: ");
        Serial.println(sctrain_length);
      }
      break;

    case 13:
      // 13: Change scopto pulse width (n * 1000)
      pulsewidth_1_scopto = n * 1000;
      if (debugmode){
        Serial.print("New scopto pulse width (us): ");
        Serial.println(n * 1000);
      }
      break;

    case 14:
      // 14: Opto pulse width (n * 1000 us)
      pulsewidth_2_opto = n * 1000;
      if (debugmode){
        Serial.print("New opto pulse width (us): ");
        Serial.println(pulsewidth_2_opto);
      }
      break;

    case 15:
      // 15: Use scheduler (n = 1 yes, 0 no) 
      usescheduler = (n == 1);
      
      // Reset
      if (usescheduler){
        stimenabled = false;

        if (debugpins){
          digitalWrite(schedulerpin, HIGH);
          digitalWrite(preoptopin, HIGH);
          digitalWrite(inoptopin, LOW);
          digitalWrite(postoptopin, LOW);
        }
      }
      else{
        stimenabled = true;
        if (debugpins){
          digitalWrite(schedulerpin, LOW);
          digitalWrite(preoptopin, LOW);
          digitalWrite(inoptopin, LOW);
          digitalWrite(postoptopin, LOW);
        }
      }
      schedulerrunning = false;

      if (debugmode){
        Serial.print("Use scheduler: ");
        Serial.println(stimenabled);
      }
      break;

    case 16:
      // 16: Number of trains (n * 10 of trains)
      ntrain = n;
      itrain = 0;
      if (debugmode){
        Serial.print("Number of trains: ");
        Serial.println(ntrain);
      }
      break;

    case 17:
      // 17: Enable manual scheduler override (n = 1 yes, 0 no) 
      manualscheduleoverride = (n == 1);
      if (debugmode){
        Serial.print("Enable manual scheduler override: ");
        Serial.println(manualscheduleoverride);
      }
      break;

    case 18:
      // 18: Delay time after opto (n * 100 ms)
      nfoodpulsedelay = n * 100;
      if (debugmode){
        Serial.print("Delay time after opto (ms): ");
        Serial.println(nfoodpulsedelay);
      }
      break;

    case 19:
      // Pulse on time (n * 10 ms)
      foodpulse_ontime = n * 10;
      if (debugmode){
        Serial.print("Food pulse on time (ms): ");
        Serial.println(foodpulse_ontime);
      }
      break;

    case 20: 
      // 20: Pulse cycle time (n * 10 ms)
      foodpulse_cycletime = n * 10;
      if (debugmode){
        Serial.print("Food pulse cycle time (ms): ");
        Serial.println(foodpulse_cycletime);
      }
      break;

    case 21:
      // 21: Pulses per train (n)
      foodpulses = n;
      foodpulses_left = 0;
      if (debugmode){
        Serial.print("Pulses per train: ");
        Serial.println(foodpulses);
      }
      break;

    case 22:
      // 22: Conditional or not (n = 1 yes, 0 no) 
      foodttlconditional = (n == 1);
      foodttlwait = false;
      if (foodttlconditional){
        foodttlcuewait = false;
        foodttlactionwait = false;
        cueon = false;
        actionperiodon = false;
      }
      
      if (debugmode){
        Serial.print("Food pulses conditional or not: ");
        Serial.println(foodttlconditional);
      }
      break;

    case 23:
      // 23 encoder useage (n = 1 yes, 0 no) 
      useencoder = (n == 1);
      if (debugmode){
        Serial.print("Use encoder or not: ");
        Serial.println(useencoder);
      }
      break;

    case 24:
      // 24: Use Food TTL or not (n = 1 yes, 0 no) 
      usefoodpulses = (n == 1);
      if (debugmode){
        Serial.print("Food TTL used: ");
        Serial.println(usefoodpulses);
      }
      break;

    case 25:
      // 25: Audio sync (n = 1 yes, 0 no);
      syncaudio = (n == 1);
      if (debugmode){
        Serial.print("Audio sync or not: ");
        Serial.println(syncaudio);
      }
      break;

    case 26:
      // 26: audio sync tone frequency (n * 100 Hz)
      audiofreq = n * 100;
      if (debugmode){
        Serial.print("Audio sync tone frequency: ");
        Serial.println(audiofreq);
      }
      break;

    case 27:
      // 27: Listening mode on or off (n = 1 yes, 0 no)
      listenmode = (n == 1);
      if (listenmode){
        manualscheduleoverride = true;
      }
      if (debugmode){
        Serial.print("Listening mode on or not: ");
        Serial.println(listenmode);
        if (debugmode){
          Serial.println("Manual over-ride on.");
        }
      }
      break;

    case 28:
      // 28: Listen mode polarity (1 = active high, 0 = active low)
      listenpol = (n == 1); // Polarity for listen mode false = active low, true = active high. Either way, it has to be 3.3V logic
      if (listenpol){
        pinMode(switchpin, INPUT_PULLDOWN); // active high
      }
      else {
        pinMode(switchpin, INPUT_PULLUP); // active low
      }
      if (debugmode){
        Serial.print("Input opto trigger polarity (1 = active high, 0 = active low): ");
        Serial.println(listenpol);
      }
      break;

    case 29:
      // 29: Tristate pin polarity (1 = active high, 0 = active low);
      tristatepinpol = (n == 1);
      if (debugmode){
        Serial.print("Tristate pin polarity (1 = active high, 0 = active low): ");
        Serial.println(tristatepinpol);
      }
      
      if (optophotommode || tcpmode){
        digitalWrite(tristatepin, tristatepinpol); // Let it go through
        if (debugmode){
          Serial.print("Auto switched to output = ");
          Serial.println(tristatepinpol);
        }
      }
      else if (samecoloroptomode){
        digitalWrite(tristatepin, !tristatepinpol); // Do not let it go through
        if (debugmode){
          Serial.print("Auto switched to output = ");
          Serial.println(!tristatepinpol);
        }
      }
      break;

    case 30:
      // 30: Use buzzer cue or not (n = 1 yes, 0 no)
      usebuzzcue = (n == 1);
      if (!usebuzzcue){
        noTone(audiopin);
      }
      if (debugmode){
        Serial.print("Use buzzer (1 = yes, 0 = no): ");
        Serial.println(usebuzzcue);
      }
      break;

    case 31:
      // 31: Buzzer delay (n * 100 ms)
      buzzdelay = n * 100;
      if (debugmode){
        Serial.print("New buzzer delay (ms): ");
        Serial.println(buzzdelay);
      }
      break;

    case 32:
      // 32: Buzzer duration (n * 100 ms)
      buzzdur = n * 100;
      if (debugmode){
        Serial.print("New buzzer duration (ms): ");
        Serial.println(buzzdur);
      }
      break;

    case 33:
      // Action period delay (n * 100 ms)
      actiondelay = n * 100;
      if (debugmode){
        Serial.print("New action period delay (ms): ");
        Serial.println(actiondelay);
      }
      break;

    case 34:
      // Action period delay (n * 100 ms)
      actiondur = n * 100;
      if (debugmode){
        Serial.print("New action period duration (ms): ");
        Serial.println(actiondur);
      }
      break;

    case 35:
      // 35: Delivery period duration (n * 100 ms);
      deliverydur = n * 100;
      if (debugmode){
        Serial.print("New delivery period duration (ms): ");
        Serial.println(deliverydur);
      }
      break;

    case 36:
      // 36: Cycle 1 for optophotometry (only changed for pure optogenetic experiments; cycle = n * 100 us)
      cycletime_photom_1_opto = n * 100;
      cycletime_photom_1 = cycletime_photom_1_opto; // in micro secs (Ch1)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      if (debugmode){
        Serial.print("New opto cycle 1 (us): ");
        Serial.println(cycletime_photom_1_opto);
      }
      break;

   case 37:
      // 37: Cycle 2 for optophotometry (only changed for pure optogenetic experiments; cycle = n * 100 us)
      cycletime_photom_2_opto = n * 100;
      cycletime_photom_2 = cycletime_photom_2_opto; // in micro secs (Ch1)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      if (debugmode){
        Serial.print("New opto cycle 2 (us): ");
        Serial.println(cycletime_photom_2_opto);
      }
      break;
       
    case 38:
      // 38: use RNG or not (n = 1 yes, 0 no)
      useRNG = (n == 1);

      if (useRNG){
        trainpass = false;
      }
      
      if (debugmode){
        Serial.print("Use RNG (1 = yes, 0 = no): ");
        Serial.println(useRNG);
      }
      break;

    case 39:
      // 39: Pass chance in percent (30 = 30% pass chance)
      threshrng = n;
      if (debugmode){
        Serial.print("New RNG pass chance (%): ");
        Serial.println(threshrng);
      }
      break;

    case 40:
      // 40: RNG ITI or not (n = 1 yes, 0 no)
      randomiti = (n == 1);
      if (debugmode){
        Serial.print("Randomize ITI (1 = yes, 0 = no): ");
        Serial.println(randomiti);
      }
      break;

    case 41:
      // 41: min randomized ITI (n * 1 s)
      rng_cycle_min = n;
      if (debugmode){
        Serial.print("Min randomized ITI (s): ");
        Serial.println(rng_cycle_min);
      }
      break;

    case 42:
      // 42: max randomized ITI (n * 2 s)
      rng_cycle_max = n;
      if (debugmode){
        Serial.print("Max randomized ITI (s): ");
        Serial.println(rng_cycle_max);
      }
      break;

    case 43:
      // 43: auto encoder serial feedback (n = 1 yes, 0 no)
      autoencserial = (n == 1);
      if (debugmode){
        Serial.print("Auto encoder serial communication (1 = yes, 0 = no): ");
        Serial.println(autoencserial);
      }
      break;

    case 44:
      // 44: Auto trial and rng echo (n = 1 yes, 0 no)
      autoechotrialinfo = (n == 1);
      if (debugmode){
        Serial.print("Auto trial and rng info serial communication (1 = yes, 0 = no): ");
        Serial.println(autoechotrialinfo);
      }
      break;

    case 45:
      // 45: Auto trial and rng echo periodicity (n * 100 ms)
      echotime_slow = n * 100;
      if (debugmode){
        Serial.print("Auto trial and rng info serial communication periodicity (ms): ");
        Serial.println(echotime_slow);
      }
      break;

    case 46:
      // 46: Adding trains (trains = trains + n * 256)
      ntrain = ntrain + n * 256;
      itrain = 0;
      if (debugmode){
        Serial.print("Number of trains: ");
        Serial.println(ntrain);
      }
      break;

    case 47:
      // 47: Change tcp behavioral train cycle (train_cycle = n * 50)
      tcptrain_cycle = n * fps;
      if (debugmode){
        Serial.print("New tcp train cycle (s): ");
        Serial.println(tcptrain_cycle / fps);
      }
      break;

    case 48:
      // 48: Opto then Food (n = 1 yes, 0 no)
      optothenfood = (n == 1);
      if (debugmode){
        Serial.print("Opto then Food (1 = yes, 0 = no): ");
        Serial.println(optothenfood);
      }
      break;

    case 49:
      // 49: Complement delay time before opto (n s)
      nfoodpulsedelay_complement = n * fps;
      if (debugmode){
        Serial.print("Complement delay time ebfore opto (s): ");
        Serial.println(nfoodpulsedelay_complement / fps);
      }
      break;

    case 50:
      // 50: Adding delay time after opto (+ n * 256 * 100 ms)
      nfoodpulsedelay = nfoodpulsedelay + n * 256 * 100;
      if (debugmode){
        Serial.print("Delay time after opto (ms): ");
        Serial.println(nfoodpulsedelay);
      }
      break;

    case 51:
      // 51: Adding lead time before opto (+ n * 256 s) 
      nfoodpulsedelay_complement = nfoodpulsedelay_complement + n * 256 * fps;
      if (debugmode){
        Serial.print("Complement delay time ebfore opto (s): ");
        Serial.println(nfoodpulsedelay_complement / fps);
      }
      break;

    case 52:
      // 52: Adding delay (+ n * 256 * 50, n = 1 means 256 second). Only relevant when scheduler is on
      preoptotime = preoptotime + n * 256; // in seconds (max is high because npreoptopulse is unsigned int which is 32 bits)
      npreoptopulse = preoptotime * fps; // preopto pulse number
      ipreoptopulse = 0;
      itrain = 0;
      break;

    case 53:
      // 53: Cycle 1 for TCP (only changed for pure TCP experiments; cycle = n * 100 us)
      cycletime_photom_1_tcp = n * 100;
      cycletime_photom_1 = cycletime_photom_1_tcp; // in micro secs (Ch1)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      
      if (debugmode){
        Serial.print("New tcp cycle 1 (us): ");
        Serial.println(cycletime_photom_1_tcp);
      }
      break;
      
    case 54:
      // 54: Cycle 2 for TCP (only changed for pure TCP experiments; cycle = n * 100 us)
      cycletime_photom_2_tcp = n * 100;
      cycletime_photom_2 = cycletime_photom_2_tcp; // in micro secs (Ch2)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      
      if (debugmode){
        Serial.print("New tcp cycle 2 (us): ");
        Serial.println(cycletime_photom_2_tcp);
      }
      break;

    case 55:
      // 55: Add Cycle 1 for optophotometry (cycle = cycle + n * 256 * 100 us);
      cycletime_photom_1_opto = cycletime_photom_1_opto + n * 100 * 256;
      cycletime_photom_1 = cycletime_photom_1_opto; // in micro secs (Ch1)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      if (debugmode){
        Serial.print("New opto cycle 1 (us): ");
        Serial.println(cycletime_photom_1_opto);
      }
      break;

    case 56:
      // 56: Add Cycle 2 for optophotometry (cycle = cycle + n * 256 * 100 us);
      cycletime_photom_2_opto = cycletime_photom_2_opto + n * 100 * 256;
      cycletime_photom_2 = cycletime_photom_2_opto; // in micro secs (Ch1)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      if (debugmode){
        Serial.print("New opto cycle 2 (us): ");
        Serial.println(cycletime_photom_2_opto);
      }
      break;

    case 57:
      // 57: Add Cycle 1 for TCP (cycle = cycle + n * 256 * 100 us);
      cycletime_photom_1_tcp = cycletime_photom_1_tcp + n * 100 * 256;
      cycletime_photom_1 = cycletime_photom_1_tcp; // in micro secs (Ch1)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      
      if (debugmode){
        Serial.print("New tcp cycle 1 (us): ");
        Serial.println(cycletime_photom_1_tcp);
      }
      break;

    case 58:
      // 58: Add Cycle 2 for TCP (cycle = cycle + n * 256 * 100 us);
      cycletime_photom_2_tcp = cycletime_photom_2_tcp + n * 100 * 256;
      cycletime_photom_2 = cycletime_photom_2_tcp; // in micro secs (Ch2)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      
      if (debugmode){
        Serial.print("New tcp cycle 2 (us): ");
        Serial.println(cycletime_photom_2_tcp);
      }
      break;

    case 59:
      // 59: Opto-photometry overlap (n = 1 yes, 0 no)
      optophotooverlap = (n == 1);
      if (debugmode){
        Serial.print("Opto-photometry overlap: ");
        Serial.println(optophotooverlap);
      }
      break;
  }

  if (debugpins){
    if (!serialpinon){
      serialpinon = true;
      digitalWrite(serialpin, HIGH);
    }
    else{
      serialpinon = false;
      digitalWrite(serialpin, LOW);
    }
  }
}


// Switch parameters
void modeswitch(void){
  if (optophotommode){
    pulsewidth_1 = pulsewidth_1_tcp;
    pulsewidth_2 = pulsewidth_2_opto; // Switch out the parameters
    cycletime_photom_1 = cycletime_photom_1_opto; // in micro secs (Ch1)
    cycletime_photom_2 = cycletime_photom_2_opto; // in micro secs (Ch2)
    digitalWrite(tristatepin, tristatepinpol); // Let ch2 go through. Write false if active low (false)
        
    if ((debugmode || serialdebug)){
      Serial.println("Optophotometry mode.");
    }
  }
  
  else if (tcpmode){
    pulsewidth_1 = pulsewidth_1_tcp;
    pulsewidth_2 = pulsewidth_2_tcp; // Switch out the parameters
    cycletime_photom_1 = cycletime_photom_1_tcp; // in micro secs (Ch1)
    cycletime_photom_2 = cycletime_photom_2_tcp; // in micro secs (Ch2)
    digitalWrite(tristatepin, tristatepinpol); // Let ch2 go through; Write false if active low (false)
    
    if ((debugmode || serialdebug)){
      Serial.println("Two-color photometry mode.");
      Serial.println("Scheduler is disabled");
    }
  }

  else if (samecoloroptomode){
    pulsewidth_1 = pulsewidth_1_tcp; // Switch out the parameters. Use tcp mode until scopto is on
    pulsewidth_2 = pulsewidth_2_tcp; // Switch out the parameters
    cycletime_photom_1 = cycletime_photom_1_scopto; // in micro secs (Ch1)
    cycletime_photom_2 = cycletime_photom_2_scopto; // in micro secs (Ch2)
    digitalWrite(AOpin, LOW);
    digitalWrite(tristatepin, !tristatepinpol); // DC Ch2. Write true if active low (false)
    
    if ((debugmode || serialdebug)){
      Serial.println("Same-color optophotometry mode.");
    }
  }
  
  // Update fps
  fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
}

// Show all parameters
void showpara(void){
  Serial.println("============== photometry ==============");
  if (optophotommode){
    Serial.println("Optophotometry mode.");
  }
  else if (tcpmode){
    Serial.println("Two-color photometry mode.");
  }
  else if (samecoloroptomode){
    Serial.println("Same color optophotometry mode.");
  }
  Serial.print("Ch1 (us): ");
  Serial.print(pulsewidth_1);
  Serial.print("/");
  Serial.println(cycletime_photom_1);
  Serial.print("Ch2 (us): ");
  Serial.print(pulsewidth_2);
  Serial.print("/");
  Serial.println(cycletime_photom_2);
  
  // Photometry
  Serial.println("============== TCP ==============");
  Serial.print("Ch1 photometry cycle time (us): ");
  Serial.println(cycletime_photom_1);
  Serial.print("Ch1 photometry on time (us): ");
  Serial.println(pulsewidth_1);
  Serial.print("Ch2 photometry cycle time (us): ");
  Serial.println(cycletime_photom_2);
  Serial.print("Ch2 photometry on time (us): ");
  Serial.println(pulsewidth_2);

  // Opto
  Serial.println("============== Optophoto ==============");
  Serial.print("Opto freq (Hz): ");
  Serial.println(fps / opto_per);
  Serial.print("Opto pulses per train: ");
  Serial.println(train_length);
  Serial.print("Opto train cycle (s): ");
  Serial.println(train_cycle / fps);
  Serial.print("Pulse width (ms): ");
  Serial.println(pulsewidth_2_opto/1000);
  Serial.print("Opto-photometry overlap: ");
  Serial.println(optophotooverlap);

  // Same color opto
  Serial.println("============== SCoptophoto ==============");
  Serial.print("Same-color opto freq (Hz): ");
  Serial.println(fps / scopto_per);
  Serial.print("Same-color opto train length (s): ");
  Serial.println(sctrain_length / (fps/scopto_per));
  Serial.print("Polarity for tristate enable (1 = active high, 0 = active low): ");
  Serial.println(tristatepinpol); // Polarity for the tristatepin (1 = active high, 0 = active low). Default active low.)
  Serial.print("Same-color opto train cycle (s): ");
  Serial.println(sctrain_cycle / fps);
  Serial.print("Pulse width (ms): ");
  Serial.println(pulsewidth_1_scopto/1000);
  
  Serial.print("TCP behavior train cycle (s): ");
  Serial.println(tcptrain_cycle / fps);

  // Scheduler
  Serial.println("============== Scheduler ==============");
  Serial.print("Scheduler enabled: ");
  Serial.println(usescheduler);
  Serial.print("Preopto time (s): ");
  Serial.println(preoptotime);
  Serial.print("Preopto pulses: ");
  Serial.println(npreoptopulse);
  Serial.print("Number of opto trains: ");
  Serial.println(ntrain);
  Serial.print("Manual Overrideable: ");
  Serial.println(manualscheduleoverride);
  Serial.print("Listen mode: ");
  Serial.println(listenmode);
  Serial.print("Listen polarity (1 - positive, 0 - negative): ");
  Serial.println(listenpol);
  Serial.print("Use opto RNG (Does not apply to listen mode): ");
  Serial.println(useRNG);
  Serial.print("Opto RNG pass rate is (%): ");
  Serial.println(threshrng);
  Serial.print("Randomize ITI (1 - yes, 0 - no): ");
  Serial.println(randomiti);
  Serial.print("Randomized ITI range (s): [");
  Serial.print(rng_cycle_min);
  Serial.print(",");
  Serial.print(rng_cycle_max);
  Serial.println(")");
  
  // Cam
  Serial.println("============== Camera ==============");
  Serial.print("Cam frequency (Hz): ");
  Serial.println(freq);
  Serial.print("Cam cycle time (ms): ");
  Serial.println(cycletime/1000);
  Serial.print("Cam on time (us): ");
  Serial.println(ontime);
  Serial.print("Audio sync: ");
  Serial.println(syncaudio);
  Serial.print("Audio sync tone frequency: ");
  Serial.println(audiofreq);

  // Encoder
  Serial.println("============== Encoder ==============");
  Serial.print("Encoder enabled: ");
  Serial.println(useencoder);
  Serial.print("Auto encoder serial: ");
  Serial.println(autoencserial);

  // Food TTL
  Serial.println("============= Food TTL =============");
  Serial.print("Food TTL: ");
  Serial.println(usefoodpulses);
  Serial.print("Delay from the opto start (s): ");
  Serial.println(nfoodpulsedelay / 1000);
  Serial.print("Food TTL on time (ms): ");
  Serial.println(foodpulse_ontime);
  Serial.print("Food TTL cycle time (ms): ");
  Serial.println(foodpulse_cycletime);
  Serial.print("Food TTLs per train: ");
  Serial.println(foodpulses);
  Serial.print("Food TTL trains: ");
  Serial.println("Same as opto");
  Serial.print("Opto then food TTLs: ");
  Serial.println(optothenfood);
  Serial.print("Complement delay before the opto start (s): ");
  Serial.println(nfoodpulsedelay_complement / fps);

  // Food TTL Buzzer
  Serial.println("========== Food TTL Buzzer ========");
  Serial.print("Food TTL Buzzer: ");
  Serial.println(usebuzzcue);
  Serial.print("Food TTL Buzzer delay (ms): ");
  Serial.println(buzzdelay);
  Serial.print("Food TTL Buzzer duration (ms): ");
  Serial.println(buzzdur);

  // Food TTL Conditional
  Serial.println("====== Food TTL Conditional ======");
  Serial.print("Food TTL Conditional: ");
  Serial.println(foodttlconditional);
  Serial.print("Food TTL Action delay (ms): ");
  Serial.println(actiondelay);
  Serial.print("Food TTL Action duration (ms): ");
  Serial.println(actiondur);
  Serial.print("Food TTL Time out window (ms): ");
  Serial.println(deliverydur);
  
  // System wide
  Serial.println("============= System =============");
  Serial.print("System-wide step time (us): ");
  Serial.println(step_size);
  Serial.print("Auto echo trial info: ");
  Serial.println(autoechotrialinfo);
  Serial.print("Auto echo trial periodicity (ms): ");
  Serial.println(cycletime_slow);
}

void slowserialecho(void){
  // Echoing back trial info on a schedule
  // Only when pulsing
  if (pulsing && autoechotrialinfo){
    if ((tnowmillis - echotime_slow) >= cycletime_slow){
      // Enters here every once a slow cycle (1s default)
      echotime_slow = tnowmillis;

      // Echo back scheduler info
      echo[0] = 0;
      echo[1] = 0;
      echo[2] = 0;
      echo[3] = 1;
      if (!usescheduler){
        echo[2] = 1; // Echo back [0 0 1 1] as no scheduler
      }
      if (inpreopto){
        echo[0] = itrain;
        echo[1] = ntrain; // first byte shows n train, second byte shows itrain
        echo[2] = 2; // Echo back [0 0 2 1] as preopto
      }
      else if (inpostopto){
        echo[0] = itrain;
        echo[1] = ntrain; // first byte shows n train, second byte shows itrain
        echo[2] = 3;; // Echo back [0 0 3 1] as postopto
      }
      else {
        echo[0] = itrain;
        echo[1] = ntrain; // first byte shows n train, second byte shows itrain
      }
      Serial.write(echo, 4);

      // Echo back RNG info
      echo[3] = 2;
      echo[2] = useRNG; // Echo back [X X 0 2] as no RNG
      echo[1] = usescheduler; // Echo back [X 0 X 2] as no RNG
      echo[0] = trainpass;
      Serial.write(echo, 4);
    }
  }
}

void camerapulse(void){
  if (pulsing){
    if ((tnow - pulsetime) >= cycletime){
      if (!onoff){
        pulsetime = micros();
        if (autoencserial){
          pos = myEnc.read();
          Serial.write((byte *) &pos, 4); // Send position
        }
        
        // Serial.println(pos);
        digitalWrite(cam_pin, HIGH);
        digitalWrite(led_pin, HIGH);

        // Audio
        if (syncaudio){
          tone(audiopin, audiofreq);
        }
        
        onoff = true;

        // Advance pulse counter
        pcount++;
      }
    }
    else if ((tnow - pulsetime) >= ontime){
      if (onoff){
        digitalWrite(cam_pin, LOW);
        digitalWrite(led_pin, LOW);
        onoff = false;

        // Audio
        if (syncaudio){
          noTone(audiopin);
        }
      }
    }
  }
  else {
    if (onoff){
      digitalWrite(cam_pin, LOW);
      digitalWrite(led_pin, LOW);
      onoff = false;

      // Audio
      if (syncaudio){
        noTone(audiopin);
      }
    }
  }
}

// Arm foodttl
void armfoodttl(void){
  // Food ttl
  foodttlarmed = true;
  foodttlwait = true;
  inputttl = !foodttlconditional;
  tfood0 = tnowmillis;

  if (usebuzzcue){
    foodttlcuewait = true;
    cueon = false;
  }
  
  if (foodttlconditional){
    foodttlactionwait = true;
    actionperiodon = false;
  }
  
  if ((debugmode || serialdebug) && showfoodttl){
    Serial.print("Food TTL armed at (ms): ");
    Serial.println(tfood0);
  }
}

// Food TTL
void foodttl(void){
//  unsigned int tfooddelta = (tnowmillis - tfood0);
//  if ((tfooddelta % 100) == 0){
//    Serial.println(tfooddelta);
//  }
      
  // 1. Cue
  if (usebuzzcue){
    if (foodttlcuewait){
      // In waiting
      if ((tnowmillis - tfood0) >= buzzdelay){
        // First time trying to get out of the buzz delay
        foodttlcuewait = false;
        tone(audiopin, audiofreq);
        cueon = true;
        
        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("Cue on at (ms): ");
          Serial.println(tnowmillis);
        }
      }
    }
    else if (cueon){
      // After waiting and cue is on
      if ((tnowmillis - tfood0) >= (buzzdelay + buzzdur)){
        // Cue long enough
        cueon = false;
        noTone(audiopin);
        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("Cue off at (ms): ");
          Serial.println(tnowmillis);
        }
      }
    }
  }

  // 2. Action
  if (foodttlconditional){
    if (foodttlactionwait){
      // Waiting period for action
      if ((tnowmillis - tfood0) >= actiondelay){
        // First time trying to get out of the action delay
        foodttlactionwait = false;
        actionperiodon = true;

        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("Action period on at (ms): ");
          Serial.println(tnowmillis);
        }
      }
    }
    else if (actionperiodon){
      if(!inputttl){
        // In action perdio until getting a pulse
        inputttl = digitalRead(foodTTLinput); // active high

        if ((debugmode || serialdebug) && showfoodttl){
          if (inputttl){
            Serial.print("Food TTL detected at (ms): ");
            Serial.println(tnowmillis);
          }
        }
      }
      // Trying to end action period
      if ((tnowmillis - tfood0) >= (actiondelay + actiondur)){
        // Action period long enough
        actionperiodon = false;

        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("Action period off at (ms): ");
          Serial.println(tnowmillis);
        }
      }
    }
  }
  
  // 3. Delivery
  // In the waiting period for delivery
  if (foodttlwait){
    // In waiting
    if (((tnowmillis - tfood0) >= nfoodpulsedelay) && ((tnowmillis - tfood0) < (nfoodpulsedelay + deliverydur))){
      // Trying to get out of the waiting period
      if (foodttlconditional){
        // Trying to get out of the waiting period (happens when detection or time out (below)
        if (inputttl){
          foodpulses_left = foodpulses;
          foodttlwait = false;
          foodttlon = false;
          tfood1 = tfood0; // % Setup cycle time for the actual delivery
          if ((debugmode || serialdebug) && showfoodttl){
            Serial.print("Starting food delivery with ");
            Serial.print(foodpulses_left);
            Serial.print(" Pulses at (ms): ");
            Serial.println(tnowmillis);
          }
        }
      }
      else{
        // Unconditional
        foodpulses_left = foodpulses;
        foodttlwait = false;
        foodttlon = false;
        tfood1 = tfood0; // % Setup cycle time for the actual delivery
        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("Starting food delivery with ");
          Serial.print(foodpulses_left);
          Serial.print(" Pulses at (ms): ");
          Serial.println(tnowmillis);
        }
      }
    }

    // Time out
    if ((tnowmillis - tfood0) >= (nfoodpulsedelay + deliverydur)){
      // Should never reach here unless conditional
      foodttlwait = false;
      foodttlon = false;
      if ((debugmode || serialdebug) && showfoodttl){
        Serial.print("Food delivery timed out at (ms): ");
        Serial.println(tnowmillis);
      }
    }
  }

  // Out of the waiting period
  else{
    if (foodpulses_left > 0){
      // Still pulses left
      if (((tnowmillis - tfood1) >= (foodpulse_cycletime)) && !foodttlon){
        // Beginning of each cycle
        tfood1 = tnowmillis;
        foodttlon = true;
        digitalWrite(foodTTLpin, HIGH);
//        digitalWrite(led_pin, HIGH);
        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("Food pulse on at (ms): ");
          Serial.print(tnowmillis);
        }
      }
      else if (((tnowmillis - tfood1) >= (foodpulse_ontime)) && foodttlon){
        // Turn off
        foodttlon = false;
        digitalWrite(foodTTLpin, LOW);
//        digitalWrite(led_pin, LOW);
        foodpulses_left--;
        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("-");
          Serial.print(tnowmillis);
          Serial.print(". Pulses left: ");
          Serial.println(foodpulses_left);
        }
      }
    }
    else if (foodttlon){
      // No pulses left
      foodttlarmed = false;
      foodttlon = false;

      if ((debugmode || serialdebug) && showfoodttl){
        Serial.print("Food delivery done at (ms): ");
        Serial.println(tnowmillis);
      }
    }
  }
}

// External TTL trigger
void externalttltrig(void){
  if ((digitalRead(switchpin) == listenpol) && !manualon){
    stimenabled = true;
    inpreopto = false;
    inpostopto = false;
    inopto = true;
    itrain = 0;
    ntrain = 1; // If manually triggered, it's only gonna be 1 train at a time
    pmt_counter_for_opto = 0;
    opto_counter = 0;
    counter_for_train = 0; // Number of cycles
    foodpulses_left = 0;
    inputttl = !foodttlconditional;
    foodttlwait = false;
    manualon = true;

    if (usebuzzcue){
      foodttlcuewait = true;
    }
    
    if (foodttlconditional){
      foodttlactionwait = true;
    }

    if (debugpins){
      digitalWrite(preoptopin, LOW);
      digitalWrite(inoptopin, HIGH);
      digitalWrite(postoptopin, LOW);
    }
    
    if ((debugmode || serialdebug) && showscheduler){
      Serial.println("External pulse detected detected.");
    }
  }
  if (manualon){
    if (digitalRead(switchpin) == !listenpol){
      manualon = false;

      if ((debugmode || serialdebug) && showscheduler){
        Serial.println("External pulse detected ended.");
      }
    }
  }
}

// Scheduler disable
void schedulerdisable(void){
  stimenabled = false;
  inopto = false;
  inpostopto = true;

  if (debugpins){
    digitalWrite(preoptopin, LOW);
    digitalWrite(inoptopin, LOW);
    digitalWrite(postoptopin, HIGH);
  }
  
  if ((debugmode || serialdebug) && showscheduler){
    Serial.println("Stim is disabled. Entering post-opto phase.");
  }
}

// RNG
void rng(byte *arraylc, byte maxrnglc, byte minrnglc, int l){
  for (int rngind = 0; rngind < l; rngind++){
    arraylc[rngind] = Entropy.random(minrnglc, maxrnglc); // SLOW 1 ms per
//    result[rngind] = random(0, maxrnglc); // Fast 0.1 us per
  }
  if ((debugmode || serialdebug)){
    Serial.print("Generated ");
    Serial.print(l);
    Serial.print(" RNG numbers [");
    Serial.print(minrnglc);
    Serial.print(",");
    Serial.print(maxrnglc);
    Serial.println(").");
  }
}
