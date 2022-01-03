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
 * 7. Mechanical switch to test 6
 * 8. TTL pulses for other parts of the experiment
 */

 
// =============== Debug ===============
#define debugmode true
#define showpulses false


// ============== Encoder ==============
//#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
Encoder myEnc(4,5); // pick your pins, reverse for sign flip


// =============== Modes ===============

// Photometry mode
bool optophotommode = true; // Green + red
bool tcpmode = false;
bool samecoloroptomode = false; // Green + blue (same led for photometry and opto);
bool useencoder = true;
bool usefoodpulses = true;
bool usescheduler = true;

// pins
byte ch1_pin = 2; // try to leave 0 1 open. 
byte ch2_pin = 3; // 405 nm or red opto
byte AOpin = 3; // Use same pin as ch2 until true analog outputs are used in the future
byte tristatepin = 6; // Use to control tristate transceivers (AO (used as digital here), 0, or disconnected)
byte cam_pin = 7; // Cam pulses
byte switchpin = 8; // Toggle switch (active low)
byte foodTTLpin = 9; // output TTL to trigger food etc
byte led_pin = 13; // onboard led

// time variables for camera
unsigned long int tnow;
unsigned long int pulsetime = 0;
unsigned int step_size = 100; // in micros

// cam trig variables
bool pulsing = false;
byte freq = 30; // Pulse at 30 Hz (default)
unsigned int ontime = 20000; // On for 20 ms
unsigned long cycletime = 1000000 / freq;
bool onoff = false;
long pos = 0;
byte m, n;

// photometry time variables
unsigned long int t0; // When each cycle begins
unsigned long int t1; // Time in each cycle
int pulsewidth_1 = 6000; // in micro secs (ch1)
int pulsewidth_2 = 6000; // in micro secs (ch2)
long cycletime_photom_1; // in micro secs (Ch1)
long cycletime_photom_2; // in micro secs (Ch2)

// tcp photometry time variables
const int pulsewidth_2_tcp = 6000; // in micro secs (ch2)
const long cycletime_photom_1_tcp = 10000; // in micro secs (Ch1)
const long cycletime_photom_2_tcp = 10000; // in micro secs (Ch2)

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
const int pulsewidth_2_scopto = 0; // in micro secs (ch1). Unused
const long cycletime_photom_1_scopto = 20000; // in micro secs (Ch1)
const long cycletime_photom_2_scopto = 0; // in micro secs (Ch2). Irrelevant

// Scheduleer variables
unsigned int preoptotime = 120; // in seconds (max 1310)
unsigned int npreoptopulse = preoptotime * 50; // preopto pulse number
unsigned int ipreoptopulse = 0; // preopto pulse number
byte ntrain = 255; // Number of trains (red opto)
byte itrain = 0; // Current number of trains
byte nsctrain = 255; // Number of stim periods (same color opto)
byte isctrain = 0; // Current number of sctrains
bool inpreopto = true;
bool inopto = false;
bool inpostopto = false;

// Food TTL (basically sync'ed with opto)
unsigned int nfoodpulsedelay = 100; // In photometry cycles after opto pulse onset (AFP). Duration is AFP/50;
unsigned int ifoodpulsedelay = 0; // Current pulse
unsigned int foodpulse_ontime = 150; // in ms
unsigned int foodpulse_cycletime = 300; // in ms
byte foodpulse_length = 5; // Number of food ttl pulses per stim period (BF). Duration is  BF / (50 / AF).

// photometry and optophotometry switches
bool ch1_on = false;
bool ch2_on = false;
bool opto = false;
bool train = false;
bool transceiveron = true;

// Counters
byte pmt_counter_for_opto = 0;
byte opto_counter = 0;
long counter_for_train = 0; // Number of cycles

void setup() {
  Serial.begin(9600);
  
  if (useencoder){
    myEnc.write(0);
  }
  
  pinMode(cam_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(ch1_pin, OUTPUT);
  pinMode(ch2_pin, OUTPUT);
  digitalWrite(cam_pin, LOW);
  digitalWrite(led_pin, LOW);
  digitalWrite(ch1_pin, LOW);
  digitalWrite(ch2_pin, LOW);

  // Load up the variables
  modeswitch();
  
  if (debugmode){
    Serial.println("Experiment start.");  
    showpara();
  }

  t0 = micros();
}

void loop() {
//  sweep_start = micros(); // Debug for loop time
  tnow = micros();
  
  if (Serial.available() >= 2){
    // Read 2 bytes
    m = Serial.read();
    n = Serial.read();
    
    // Parse serial
    parseserial();
  }
  
  // Cam
  if (pulsing){
    if ((tnow - pulsetime) >= cycletime){
      if (!onoff){
        pulsetime = micros();
        Serial.write((byte *) &pos, 4);
        digitalWrite(cam_pin, HIGH);
        digitalWrite(led_pin, HIGH);
        onoff = true;
      }
    }
    else if ((tnow - pulsetime) >= ontime){
      if (onoff){
        digitalWrite(cam_pin, LOW);
        digitalWrite(led_pin, LOW);
        onoff = false;
      }
    }
  }
  else {
    if (onoff){
      digitalWrite(cam_pin, LOW);
      digitalWrite(led_pin, LOW);
      onoff = false;
    }
  }

  // photometry
  t1 = tnow - t0;
  
  // 1. Start a new cycle (once every 20 ms)
  if ((t1 >= (cycletime_photom_1 + cycletime_photom_2))){
    // This part happens once every 20 ms
    t0 = micros();
    digitalWrite(ch1_pin, HIGH);
    ch1_on = true;
    // Serial.println(t1); Confirm only once every 20 ms

    if (optophotommode){
      // Advance counters
      pmt_counter_for_opto++;
      counter_for_train++;

      if (debugmode && showpulses){
        Serial.print("Photometry ");
        Serial.print(counter_for_train);
        Serial.print(" - ");
        Serial.println(pmt_counter_for_opto);
      }

      // Beginning of each train cycle. Turn train mode on.
      if (counter_for_train == 1){
        train = true;
        if (debugmode && showpulses){
          Serial.println("Train on");
        }
      }
    
      // Turn on opto mode
      if (train){
        if (pmt_counter_for_opto==1){
          opto = true;
        }
      }

      // reset pmt counter
      if (pmt_counter_for_opto >= opto_per){
        pmt_counter_for_opto = 0;
      }
      
      // reset opto counter
      if (opto_counter >= train_length){
        train = false;
        opto_counter = 0;
        if (debugmode && showpulses){
          Serial.println("Train off");
        }
      }
    
      // Serial.println(counter_for_train);
      // reset train counter
      if (counter_for_train >= train_cycle){
        counter_for_train= 0;
      }
    }
  }
  
  // 2
  else if ((t1 >= (pulsewidth_1)) && (t1 < (cycletime_photom_1))){
    // Happens multiple times per cycle (twice actually for uno, might be getting close to the limit)
    // Serial.println(t1);
    
    // Always happen
    if (ch1_on){
      digitalWrite(ch1_pin, LOW);
      ch1_on = false;
    }
  }
  
  // 3
  else if ((t1 >= (cycletime_photom_1)) && (t1 < (cycletime_photom_1 + pulsewidth_2))){
    if (tcpmode){
      // tcp
      if (!ch2_on){
        digitalWrite(ch2_pin, HIGH);
        ch2_on = true;
      }
    }
    // Opto photometry
    else if (optophotommode){
      // optophotometry
      if (opto && !ch2_on){
        digitalWrite(ch2_pin, HIGH);
        ch2_on = true;
        opto_counter++;
        opto = false;
        if (debugmode && showpulses){
          Serial.print("Opto ");
          Serial.println(opto_counter);
        }
      }
    }
    
  }
  
  // 4
  else if (t1 >= (cycletime_photom_1 + pulsewidth_2)){
    // tcp
    if (ch2_on && tcpmode){
      digitalWrite(ch2_pin, LOW);
      ch2_on = false;
    }
  }
  
  delayMicroseconds(step_size);
//  Serial.println(micros() - sweep_start);
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
  // 4: Set delay(delay_cycle = n * 50, n in seconds). Only relevant when scheduler is on
  // 6: Change opto frequency (opto_per = n, f = 50/n)
  // 7: Change opto pulse per train (train_length = n)
  // 8: Change opto train cycle (train_cycle = n * 50, n in seconds)
  // 10: Change scopto frequency (scopto_per = n, f = 50/n)
  // 11: Change scopto train cycle (train_cycle = n * 50)
  // 12: Change scopto pulse per train (train_length = n)
  // 13: Change scopto pulse width (n * 1000)
  // 14: Reserved for opto pulse width
  
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
        Serial.println("Pulse start.");
      }
      if (useencoder){
        myEnc.write(0);    // zero the position
        pos = 0;        
      }
      break;

    case 0:
      // End pulsing
      pulsing = false;
      if (debugmode){
        Serial.println("Pulse stop.");
      }
      break;

    case 5:
      // Give position
      if (useencoder){
        pos = myEnc.read();
      }      
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
      // Set delay(delay_cycle = n * 50, n in seconds)
      preoptotime = n; // in seconds (max 1310)
      npreoptopulse = preoptotime * 50; // preopto pulse number
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
  }
}


// Switch parameters
void modeswitch(void){
  if (optophotommode){
    pulsewidth_2 = pulsewidth_2_opto; // Switch out the parameters
    cycletime_photom_1 = cycletime_photom_1_opto; // in micro secs (Ch1)
    cycletime_photom_2 = cycletime_photom_2_opto; // in micro secs (Ch2)
    if (debugmode){
      Serial.println("Optophotometry mode.");
    }
  }
  
  else if (tcpmode){
    pulsewidth_2 = pulsewidth_2_tcp; // Switch out the parameters
    cycletime_photom_1 = cycletime_photom_1_tcp; // in micro secs (Ch1)
    cycletime_photom_2 = cycletime_photom_2_tcp; // in micro secs (Ch2)
    if (debugmode){
      Serial.println("Two-color photometry mode.");
    }
  }

  else if (samecoloroptomode){
    pulsewidth_1 = cycletime_photom_1_tcp; // Switch out the parameters. Use tcp mode until scopto is on
    pulsewidth_2 = cycletime_photom_2_tcp; // Switch out the parameters
    cycletime_photom_1 = cycletime_photom_1_scopto; // in micro secs (Ch1)
    cycletime_photom_2 = cycletime_photom_2_scopto; // in micro secs (Ch2)
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
    Serial.print("Number of same-color opto trains: ");
    Serial.println(nsctrain);
    
    // Cam
    Serial.println("============== Camera ==============");
    Serial.print("Cam frequency (Hz): ");
    Serial.println(freq);
    Serial.print("Cam cycle time (us): ");
    Serial.println(cycletime);
    Serial.print("Cam on time (us): ");
    Serial.println(ontime);

    // Encoder
    Serial.println("============== Encoder ==============");
    Serial.print("Encoder enabled: ");
    Serial.println(useencoder);

    // Food TTL
    Serial.println("============= Food TTL =============");
    Serial.print("Food TTL: ");
    Serial.println(usefoodpulses);
    Serial.print("Delay from the opto start (s): ");
    Serial.println(nfoodpulsedelay / 50);
    Serial.print("Food TTL on time (ms): ");
    Serial.println(foodpulse_ontime);
    Serial.print("Food TTL cycle time (ms): ");
    Serial.println(foodpulse_cycletime);
    Serial.print("Food TTLs per train: ");
    Serial.println(foodpulse_length);
    Serial.print("Food TTL trains: ");
    Serial.println("Same as opto");
    
    // Food TTL
    Serial.println("============= System =============");
    Serial.print("System-wide step time (us): ");
    Serial.println(step_size);
  }
}
  
