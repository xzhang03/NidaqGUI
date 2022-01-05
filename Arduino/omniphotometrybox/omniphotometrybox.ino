// Third Gen photometry box v3.0
// Handle almost everything you need for photometry
// Needs a decen number of pins
// Stephen Zhang

/* 1. Two color photometry
 * 2. Optophotometry (switch on)
 * 3. Sample color optophotometry (analogy input control intensity)
 * 4. Cam pulses with adjustable frequency
 * 5. Encoder at the same frequency at 4
 * 6. Preprogram experiment timescale: baseline -> opto -> postopto
 * 7. External input for scheduler (6)
 * 8. TTL pulses for other parts of the experiment
 * 9. Audio syncing
 * 
 */

 
// =============== Debug ===============
#define debugmode false // Master switch for all serial debugging
#define showpulses false // Extremely verbose
#define showopto true
#define showscheduler true
#define showfoodttl true
#define debugpins true
unsigned long ttest1 = 0;
bool ftest1 = false;

// ============== Encoder ==============
//#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
Encoder myEnc(4,5); // pick your pins, reverse for sign flip


// =============== Modes ===============
// Photometry mode
bool optophotommode = false; // Green + red
bool tcpmode = true; //
bool samecoloroptomode = false; // Green + blue (same led for photometry and opto);
bool useencoder = true;
bool usefoodpulses = false;
bool syncaudio = false;
bool usescheduler = true;
bool manualscheduleoverride = true; // Only works when using scheduler
bool listenmode = true;

// =============== Pins ===============
const byte ch1_pin = 2; // try to leave 0 1 open. 
const byte ch2_pin = 3; // 405 nm or red opto
const byte AOpin = 3; // Use same pin as ch2 until true analog outputs are used in the future
const byte tristatepin = 6; // Use to control tristate transceivers (AO (used as digital here), 0, or disconnected). Active low
const byte cam_pin = 7; // Cam pulses
const byte switchpin = 8; // Toggle switch (active low)
const byte foodTTLpin = 9; // output TTL to trigger food etc
const byte foodTTLinput = 10; // input TTL for conditional food pulses (3.3 V only!!)
const byte led_pin = 13; // onboard led
const byte audiosyncpin = 21; // Pin for audio signal

// ============= debugpins =============
const byte serialpin = 14; // Parity signal for serial pin
const byte schedulerpin = 15; // On when scheduler is used
const byte preoptopin = 16; // preopto
const byte inoptopin = 17; // preopto
const byte postoptopin = 20; // preopto
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
unsigned int audiofreq = 2000;

// ============== Serial ==============
byte m, n;

// ============ Photometry ============
// photometry time variables
unsigned long int t0; // When each cycle begins
unsigned long int t1; // Time in each cycle
int pulsewidth_1 = 6000; // in micro secs (ch1)
int pulsewidth_2 = 6000; // in micro secs (ch2)
long cycletime_photom_1; // in micro secs (Ch1)
long cycletime_photom_2; // in micro secs (Ch2)

// tcp photometry time variables
const int pulsewidth_1_tcp = 6000; // in micro secs (ch2)
const int pulsewidth_2_tcp = 6000; // in micro secs (ch2)
const long cycletime_photom_1_tcp = 10000; // in micro secs (Ch1)
const long cycletime_photom_2_tcp = 10000; // in micro secs (Ch2)

// ============ Opto ============
// Opto varaibles
byte opto_per = 5; // Number of photometry pulses per opto pulse (A). Can be: 1, 2, 5, 10, 25, 50. Pulse freq is 50 / A
byte train_length = 10; // Number of opto pulses per train (B). Duration is  B / (50 / A).
long train_cycle = 30 * 50; // First number is in seconds. How often does the train come on.
const int pulsewidth_2_opto = 10000; // in micro secs (ch2)
const long cycletime_photom_1_opto = 6500; // in micro secs (Ch1)
const long cycletime_photom_2_opto = 13500; // in micro secs (Ch2)

// Same color opto variables
byte scopto_per = 5; // Number of photometry pulses per opto pulse (AO). Can be: 1, 2, 5, 10, 25, 50. Pulse freq is 50 / AO
byte sctrain_length = 10; // Number of photometry pulses per stim period (BO). Duration is  BO / (50 / AO).
long sctrain_cycle = 30 * 50; // First number is in seconds. How often does the train come on.
unsigned int pulsewidth_1_scopto = 10000; // in micro secs (ch1)
const long cycletime_photom_1_scopto = 20000; // in micro secs (Ch1)
const long cycletime_photom_2_scopto = 0; // in micro secs (Ch2). Irrelevant

// ============ Scheduler ============
unsigned int preoptotime = 120; // in seconds (max 1310)
unsigned int npreoptopulse = preoptotime * 50; // preopto pulse number
unsigned int ipreoptopulse = 0; // preopto pulse number
byte ntrain = 10; // Number of trains
byte itrain = 0; // Current number of trains
bool inpreopto = true;
bool inopto = false;
bool inpostopto = false;
bool stimenabled = true;
bool schedulerrunning = false;
bool manualon = false;

// ============ Food TTL ============
// Food TTL (basically sync'ed with opto)
unsigned int nfoodpulsedelay = 2000; // Time after opto pulse train onset in ms
unsigned int foodpulse_ontime = 150; // in ms
unsigned int foodpulse_cycletime = 300; // in ms
byte foodpulses = 5; // Number of food ttl pulses per stim period.
byte foodpulses_left = 0; // Try counting down this time. Probably easier to debug
unsigned long tfood0;
bool foodttlconditional = false;
bool inputttl = false;
bool foodttlarmed = false;
bool foodttlwait = false;
bool foodtllon = false;

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
long counter_for_train = 0; // Number of cycles

void setup() {
  Serial.begin(19200);
  
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
  pinMode(switchpin, INPUT_PULLUP);
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
  if (usefoodpulses && foodttlarmed){
    foodttl();
  }
  
  // photometry
  t1 = tnow - t0;
  
  // 1. Start a new cycle (once every 20 ms)
  if ((t1 >= (cycletime_photom_1 + cycletime_photom_2))){
    // Debug ch1 on to ch1 on
    // Serial.println(tnow - ttest1);
    // ttest1 = tnow;
    
    // This part happens once every 20 ms
    t0 = micros();

    // TCP or optophotometry mode
    if (tcpmode || optophotommode){
      digitalWrite(ch1_pin, HIGH);
      ch1_on = true;
      // Serial.println(t1); Confirm only once every 20 ms
    }
    
    if (!tcpmode){
      // Scheduler business here since it's not tcp
      // Manual stim
      if (manualscheduleoverride && usescheduler){
        if ((digitalRead(switchpin) == 0) && !manualon){
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
          manualon = true;

          if (debugpins){
            digitalWrite(preoptopin, LOW);
            digitalWrite(inoptopin, HIGH);
            digitalWrite(postoptopin, LOW);
          }
          
          if (debugmode && showscheduler){
            Serial.println("External pulse detected detected.");
          }
        }
        if (manualon){
          if (digitalRead(switchpin) == 1){
            manualon = false;
    
            if (debugmode && showscheduler){
              Serial.println("External pulse detected ended.");
            }
          }
        }
      }
      
      // Enable stim
      if ((ipreoptopulse >= npreoptopulse) && (!stimenabled) && inpreopto && schedulerrunning){
        stimenabled = true;
        inpreopto = false;
        inopto = true;

        if (debugpins){
          digitalWrite(preoptopin, LOW);
          digitalWrite(inoptopin, HIGH);
          digitalWrite(postoptopin, LOW);
        }
        
        if (debugmode && showscheduler){
          Serial.println("Stim is enabled. Entering opto phase.");
        }
      }
      // Just advance
      else if (schedulerrunning && inpreopto && (!stimenabled) && (!listenmode)){
        ipreoptopulse++;
        if (debugmode && showscheduler){
          if ((ipreoptopulse % 50) == 0){
              Serial.print("Preopto pulse #");
              Serial.print(ipreoptopulse);
              Serial.print("/");
              Serial.println(npreoptopulse);
          }
        }
      }
    }
    

    // scopto mode
    if (samecoloroptomode){
      // Advance counters
      // Scheduler allows train and opto counters to advance
      if (stimenabled){
        pmt_counter_for_opto++; // Counting 1-5, on 1 opto is on
        counter_for_train++; // Counting 1-1500, on 1 train is on
      }
      
      if (debugmode && showpulses){
        Serial.print("Photometry ");
        Serial.print(counter_for_train);
        Serial.print(" - ");
        Serial.println(pmt_counter_for_opto);
      }

      // Beginning of each train cycle. Turn train mode on.
      // Only if stim is enabled though
      if ((counter_for_train == 1) && stimenabled){
        // Debug train on to train on
//        Serial.println(tnow - ttest1);
//        ttest1 = tnow;
        
        train = true;
        opto_counter = 0;
        pulsewidth_1 = pulsewidth_1_scopto; // Change pulse width
        digitalWrite(AOpin, HIGH);
        digitalWrite(tristatepin, LOW);

        if (usescheduler){
          itrain++; // Advance train count

          if (debugmode && showscheduler){
            Serial.print("Scheduler train #");
            Serial.print(itrain);
            Serial.print("/");
            Serial.println(ntrain);
          }
        }

        if (usefoodpulses){
          // Food ttl
          foodttlarmed = true;
          foodttlwait = true;
          inputttl = !foodttlconditional;
          tfood0 = tnowmillis;

          if (debugmode && showfoodttl){
            Serial.print("Food TTL armed at (ms): ");
            Serial.println(tfood0);
          }
        }
        
        if (debugmode && showopto){
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
        digitalWrite(ch1_pin, HIGH);
        ch1_on = true;

        opto_counter++;
        opto = false;

        // debug ch1 on to ch1 off for opto
        if (debugmode && showopto){
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
        counter_for_train= 0;
      }
    }

    // Optophotometry
    if (optophotommode){
      // Advance counters (not using scheduler or stim is enabled)
      if (stimenabled){
        pmt_counter_for_opto++; // Counting 1-5, on 1 opto is on
        counter_for_train++; // Counting 1-1500, on 1 train is on
      }
      
      if (debugmode && showpulses){
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
          itrain++; // Advance train count

          if (debugmode && showscheduler){
            Serial.print("Scheduler train #");
            Serial.print(itrain);
            Serial.print("/");
            Serial.println(ntrain);
          }
        }

        if (usefoodpulses){
          // Food ttl
          foodttlarmed = true;
          foodttlwait = true;
          inputttl = !foodttlconditional;
          tfood0 = tnowmillis;
          

          if (debugmode && showfoodttl){
            Serial.print("Food TTL armed at (ms): ");
            Serial.println(tfood0);
          }
        }
        
        if (debugmode && showopto){
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
        counter_for_train= 0;
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
      if (debugmode && showopto){
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
        digitalWrite(tristatepin, HIGH);

        if (debugmode && showopto){
          Serial.println("Train off");
        }
        
        if (debugmode && showopto){
          Serial.println("Ch1 pulse width returned to tcp");
          Serial.println("Analog off");
          Serial.println("Transceiver off");
        }
        
        // Scheduler disable
        if ((itrain >= ntrain) && stimenabled && inopto && schedulerrunning){
          stimenabled = false;
          inopto = false;
          inpostopto = true;

          if (debugpins){
            digitalWrite(preoptopin, LOW);
            digitalWrite(inoptopin, LOW);
            digitalWrite(postoptopin, HIGH);
          }
          
          if (debugmode && showscheduler){
            Serial.println("Stim is disabled. Entering post-opto phase.");
          }
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
    else if (optophotommode){
      // optophotometry
      if ((opto) && (!ch2_on)){
        digitalWrite(ch2_pin, HIGH);
        ch2_on = true;
        opto_counter++;
        opto = false;
        if (debugmode && showopto){
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
      if (debugmode && showopto){
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

      
      if ((opto_counter == train_length) && optophotommode && train){
        // reset opto counter
        train = false;
        
        if (debugmode && showopto){
          Serial.println("Train off");
        }

        // Scheduler disable
        if ((itrain >= ntrain) && stimenabled && inopto && schedulerrunning){
          stimenabled = false;
          inopto = false;
          inpostopto = true;

          if (debugpins){
            digitalWrite(preoptopin, LOW);
            digitalWrite(inoptopin, LOW);
            digitalWrite(postoptopin, HIGH);
          }
          
          if (debugmode && showscheduler){
            Serial.println("Stim is disabled. Entering post-opto phase.");
          }
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
  
  // ========== Opto & photom ==========
  // 3: TCP mode (n = 0 TCP, n = 1 optophotometry, n = 2 samecolor optophotometry)
  // 6: Change opto frequency (opto_per = n, f = 50/n)
  // 7: Change opto pulse per train (train_length = n)
  // 8: Change opto train cycle (train_cycle = n * 50, n in seconds)
  // 10: Change scopto frequency (scopto_per = n, f = 50/n)
  // 11: Change scopto train cycle (train_cycle = n * 50)
  // 12: Change scopto pulse per train (train_length = n)
  // 13: Change scopto pulse width (n * 1000)
  // 14: Reserved for opto pulse width

  // ============ Scheduler ============
  // 15: Use scheduler (n = 1 yes, 0 no) 
  // 4: Set delay (delay_cycle = n * 50, n in seconds). Only relevant when scheduler is on
  // 16: Number of trains (n = n of trains)
  // 17: Enable manual scheduler override
  // 27: Listening mode on or off (n = 1 yes, 0 no). Will turn on manual override.

  // ============ Food TTL ============
  // 24: Use Food TTL or not (n = 1 yes, 0 no) 
  // 18: Delay time after opto (n * 1000 ms)
  // 19: Pulse on time (n * 10 ms)
  // 20: Pulse cycle time (n * 10 ms)
  // 21: Pulses per train (n)
  // 22: Conditional or not (n = 1 yes, 0 no) 
  
  // ============= Encoder =============
  // 23 encoder useage (n = 1 yes, 0 no) 

  // ============ Audio sync ============
  // 25: Audio sync (n = 1 yes, 0 no)
  // 26: Audio sync tone frequency (n * 100 Hz)
  
  
  switch (m){
    case 2:
      // Set frequency
      cycletime = 1000000 / n;

      if (debugmode){
        Serial.print("New cam cycle time (us): ");
        Serial.println(cycletime);
      }
      break;
      
    case 1:
      // Start pulsing
      pulsing = true;
      pulsetime = micros();
      if (debugmode){
        Serial.println("Cam pulse start.");
      }
      if (useencoder){
        myEnc.write(0);    // zero the position
        pos = 0;        
      }

      if (tcpmode){
        usescheduler = false;
        schedulerrunning = false;
      }
      
      // Scheduler reset
      if (usescheduler){
        ipreoptopulse = 0;
        itrain = 0;
        pmt_counter_for_opto = 0;
        opto_counter = 0;
        counter_for_train = 0; // Number of cycles
        foodpulses_left = 0;
        inputttl = !foodttlconditional;
        foodttlarmed = false;
        foodttlwait = false;
        foodtllon = false;
        inpreopto = true;
        inopto = false;
        inpostopto = false;
        stimenabled = false;
        schedulerrunning = true;

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
      break;

    case 0:
      // End pulsing
      pulsing = false;
      if (usescheduler){
        stimenabled = false;
        schedulerrunning = false;
      }
      if (debugmode){
        Serial.println("Cam pulse stop.");
      }
      break;

    case 5:
      // Give position
      if (useencoder){
        pos = myEnc.read();
      }
      else{
        pos = 0;
      }
//      Serial.println(pos);      
      Serial.write((byte *) &pos, 4);
      break;
      
    case 9:
      // List parameters
      showpara();
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
      // 4: Set delay (delay_cycle = n * 50, n in seconds). Only relevant when scheduler is on
      preoptotime = n; // in seconds (max 1310)
      npreoptopulse = preoptotime * 50; // preopto pulse number
      ipreoptopulse = 0;
      itrain = 0;
      break;
      
    case 6:
      // 6: Change opto frequency (opto_per = n)
      opto_per = n;

      if (debugmode){
        Serial.print("New opto freq (Hz): ");
        Serial.println(50 / opto_per);
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
      train_cycle = n * 50;
      if (debugmode){
        Serial.print("New opto train cycle (s): ");
        Serial.println(train_cycle / 50);
      }
      break;

    case 10:
      // 10: Change scopto frequency (scopto_per = n, f = 50/n)
      scopto_per = n;

      if (debugmode){
        Serial.print("New scopto freq (Hz): ");
        Serial.println(50 / opto_per);
      }
      break;

    case 11:
      // 11: Change scopto train cycle (train_cycle = n * 50)
      sctrain_cycle = n * 50;
      if (debugmode){
        Serial.print("New scopto train cycle (s): ");
        Serial.println(sctrain_cycle / 50);
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
      // 14: Reserved for opto pulse width
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
      // 16: Number of trains (n = n of trains)
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

    case 24:
      // 24: Use Food TTL or not (n = 1 yes, 0 no) 
      usefoodpulses = (n == 1);
      if (debugmode){
        Serial.print("Food TTL used: ");
        Serial.println(usefoodpulses);
      }
      break;
    
    case 18:
      // 18: Delay time after opto (n * 1000 ms)
      nfoodpulsedelay = n * 1000;
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
        if (listenmode){
          Serial.println("Manual over-ride on.");
        }
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
    digitalWrite(tristatepin, LOW); // Let ch2 go through;
        
    if (debugmode){
      Serial.println("Optophotometry mode.");
    }
  }
  
  else if (tcpmode){
    pulsewidth_1 = pulsewidth_1_tcp;
    pulsewidth_2 = pulsewidth_2_tcp; // Switch out the parameters
    cycletime_photom_1 = cycletime_photom_1_tcp; // in micro secs (Ch1)
    cycletime_photom_2 = cycletime_photom_2_tcp; // in micro secs (Ch2)
    digitalWrite(tristatepin, LOW); // Let ch2 go through;
    
    // Disbale scheduler
    stimenabled = false;
    usescheduler = false;
    schedulerrunning = false;
    if (debugpins){
      digitalWrite(schedulerpin, LOW);
      digitalWrite(preoptopin, LOW);
      digitalWrite(inoptopin, LOW);
      digitalWrite(postoptopin, LOW);
    }
    
    if (debugmode){
      Serial.println("Two-color photometry mode.");
      Serial.println("Scheduler is disabled");
    }
  }

  else if (samecoloroptomode){
    pulsewidth_1 = pulsewidth_1_tcp; // Switch out the parameters. Use tcp mode until scopto is on
    pulsewidth_2 = pulsewidth_2_tcp; // Switch out the parameters
    cycletime_photom_1 = cycletime_photom_1_scopto; // in micro secs (Ch1)
    cycletime_photom_2 = cycletime_photom_2_scopto; // in micro secs (Ch2)
    digitalWrite(tristatepin, HIGH); // DC Ch2
    
    if (debugmode){
      Serial.println("Same-color optophotometry mode.");
    }
  }
}

// Show all parameters
void showpara(void){
  if (debugmode){
    Serial.print("============== photometry ==============");
    if (optophotommode){
      Serial.println("Optophotometry mode.");
    }
    else if (tcpmode){
      Serial.println("Two-color photometry mode.");
    }
    else if (samecoloroptomode){
      Serial.println("Same color optophotometry mode.");
    }
    
    // Photometry
    Serial.print("Ch1 photometry cycle time (us): ");
    Serial.println(cycletime_photom_1);
    Serial.print("Ch1 photometry on time (us): ");
    Serial.println(pulsewidth_1);
    Serial.print("Ch2 photometry cycle time (us): ");
    Serial.println(cycletime_photom_2);
    Serial.print("Ch2 photometry on time (us): ");
    Serial.println(pulsewidth_2);

    // Opto
    Serial.print("Opto freq (Hz): ");
    Serial.println(50 / opto_per);
    Serial.print("Opto pulses per train: ");
    Serial.println(train_length);
    Serial.print("Opto train cycle (s): ");
    Serial.println(train_cycle / 50);

    // Same color opto
    Serial.print("Same-color opto freq (Hz): ");
    Serial.println(50 / scopto_per);
    Serial.print("Same-color opto train length (s): ");
    Serial.println(sctrain_length / (50/scopto_per));

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
    
    // Cam
    Serial.println("============== Camera ==============");
    Serial.print("Cam frequency (Hz): ");
    Serial.println(freq);
    Serial.print("Cam cycle time (us): ");
    Serial.println(cycletime);
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
    
    // Food TTL
    Serial.println("============= System =============");
    Serial.print("System-wide step time (us): ");
    Serial.println(step_size);
  }
}

void camerapulse(void){
  if (pulsing){
    if ((tnow - pulsetime) >= cycletime){
      if (!onoff){
        pulsetime = micros();
//        pos = myEnc.read();
        // Serial.write((byte *) &pos, 4);
        // Serial.println(pos);
        digitalWrite(cam_pin, HIGH);
        digitalWrite(led_pin, HIGH);

        // Audio
        if (syncaudio){
          tone(audiosyncpin, audiofreq);
        }
        
        onoff = true;
      }
    }
    else if ((tnow - pulsetime) >= ontime){
      if (onoff){
        digitalWrite(cam_pin, LOW);
        digitalWrite(led_pin, LOW);
        onoff = false;

        // Audio
        if (syncaudio){
          noTone(audiosyncpin);
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
        noTone(audiosyncpin);
      }
    }
  }
}

void foodttl(void){
  if (foodttlwait){
    if (((tnowmillis - tfood0) >= nfoodpulsedelay)){
      // Out of the waiting period (happens once per train)
      foodttlwait = false;
      foodtllon = false;
      if (inputttl){
        foodpulses_left = foodpulses;
        
      }
      
      if (debugmode && showfoodttl){
        Serial.print("Starting food delivery with ");
        Serial.print(foodpulses_left);
        Serial.print(" Pulses at (ms): ");
        Serial.println(tnowmillis);
      }
    }
    else if (foodttlconditional && !inputttl) {
      inputttl = digitalRead(foodTTLinput); // active high

      if (debugmode && showfoodttl){
        if (inputttl){
          Serial.print("Food TTL detected at (ms): ");
          Serial.println(tnowmillis);
        }
      }
    }
  }

  // Out of the waiting period
  else{
    if (foodpulses_left > 0){
      // Still pulses left
      if (((tnowmillis - tfood0) >= (foodpulse_cycletime)) && !foodtllon){
        // Beginning of each cycle
        tfood0 = tnowmillis;
        foodtllon = true;
        digitalWrite(foodTTLpin, HIGH);
        if (debugmode && showfoodttl){
          Serial.print("Food pulse on at (ms): ");
          Serial.print(tnowmillis);
        }
      }
      else if (((tnowmillis - tfood0) >= (foodpulse_ontime)) && foodtllon){
        // Turn off
        foodtllon = false;
        digitalWrite(foodTTLpin, LOW);
        foodpulses_left--;
        if (debugmode && showfoodttl){
          Serial.print("-");
          Serial.print(tnowmillis);
          Serial.print(". Pulses left: ");
          Serial.println(foodpulses_left);
        }
      }
    }
    else{
      // No pulses left
      foodttlarmed = false;
      foodtllon = false;
      if (debugmode && showfoodttl){
        Serial.print("Food delivery done at (ms): ");
        Serial.println(tnowmillis);
      }
    }
  }
}
