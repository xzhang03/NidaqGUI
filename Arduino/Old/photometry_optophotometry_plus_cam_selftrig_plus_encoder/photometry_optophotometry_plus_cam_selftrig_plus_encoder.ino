// handle the damn led
#include <Adafruit_DotStar.h>
#include <SPI.h>
#define NUMPIXELS 1 // Number of LEDs in strip
#define DATAPIN 7
#define CLOCKPIN 8
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

// Encoder
//#define ENCODER_DO_NOT_USE_INTERRUPTS
//#include <Encoder.h>
//Encoder myEnc(4,5); // pick your pins, reverse for sign flip


// debug
#define debugmode true
#define showpulses false

// Photometry mode
bool optophotommode = true;

// pins
byte ch1_pin = 0; // try to leave 3 4 open. 3 is fine if needed
byte ch2_pin = 2;
byte cam_pin = 1;
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
// unsigned int sweep_start;
bool onoff = false;
long pos = 0;
byte m, n;

// photometry time variables
unsigned long int t0; // When each cycle begins
unsigned long int t1; // Time in each cycle
int pulsewidth_1 = 6000; // in micro secs (ch1)
int pulsewidth_2; // in micro secs (ch2)
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

// photometry and optophotometry switches
bool ch1_on = false;
bool ch2_on = false;
bool opto = false;
bool train = false;

// Counters
byte pmt_counter_for_opto = 0;
byte opto_counter = 0;
long counter_for_train = 0; // Number of cycles

void setup() {
  Serial.begin(9600);
//  myEnc.write(0);
  
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
  
  // Onboard LED
  if (debugmode){
    strip.setPixelColor(0, 0x100000); // red
  }
  else {
    strip.setPixelColor(0, 0x050505); // white
  }
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
}

void loop() {
//  sweep_start = micros();
  
  if (Serial.available() >= 2){
    // Read 2 bytes
    m = Serial.read();
    n = Serial.read();

    if (debugmode){
      m = m - '0';
      n = n - '0';
    }

    // m table
    // 2: new cycle time (1000000/n, n in Hz)
    // 1: start pulse
    // 0: stop pulse
    // 5: Quad encoder position
    // 3: TCP mode (n = 0 TCP, n = 1 optophotometry)
    // 4: Set delay(delay_cycle = n * 50, n in seconds)
    // 9: Show all parameters
    // 6: Change opto frequency (opto_per = n, f = 50/n)
    // 7: Change opto pulse per train (train_length = n)
    // 8: Change opto train cycle (train_cycle = n * 50, n in seconds)

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
//        myEnc.write(0);    // zero the position
//        pos = 0;
        break;

      case 0:
        // End pulsing
        pulsing = false;
        if (debugmode){
          Serial.println("Pulse stop.");
        }
        break;
      case 3:
        // tcp mode
        if (n == 0){
          // tcp
          optophotommode = false;
        }
        else{
          // optophotometry
          optophotommode = true;
        }
        modeswitch();
        break;
      case 4:
        // Set delay(delay_cycle = n * 50, n in seconds)
        // WIP
        break;
      case 5:
        // Give position
//        pos = myEnc.read();
        Serial.write((byte *) &pos, 4);
        break;
      case 9:
        // List parameters
        showpara();
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
          Serial.print("Opto train cycle (s): ");
          Serial.println(train_cycle / 50);
        }
        break;
    }
  }
  
  tnow = micros();
  
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
  // 1
  if ((t1 >= (cycletime_photom_1 + cycletime_photom_2))){
    if (!ch1_on){
      // This part happens once every 20 ms
      t0 = micros();
      digitalWrite(ch1_pin, HIGH);
      ch1_on = true;

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
  }
  
  // 2
  else if ((t1 >= (pulsewidth_1)) && (t1 < (cycletime_photom_1))){
    if (ch1_on){
      digitalWrite(ch1_pin, LOW);
      ch1_on = false;
    }
  }
  
  // 3
  else if ((t1 >= (cycletime_photom_1)) && (t1 < (cycletime_photom_1 + pulsewidth_2))){
    if (!optophotommode){
      // tcp
      if (!ch2_on){
        digitalWrite(ch2_pin, HIGH);
        ch2_on = true;
      }
    }
    else{
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
    // tcp and optophotometry
    if (ch2_on){
      digitalWrite(ch2_pin, LOW);
      ch2_on = false;
    }
  }
  
  delayMicroseconds(step_size);
//  Serial.println(micros() - sweep_start);
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
  else{
    pulsewidth_2 = pulsewidth_2_tcp; // Switch out the parameters
    cycletime_photom_1 = cycletime_photom_1_tcp; // in micro secs (Ch1)
    cycletime_photom_2 = cycletime_photom_2_tcp; // in micro secs (Ch2)
    if (debugmode){
      Serial.println("Two-color photometry mode.");
    }
  }
}

// Show all parameters
void showpara(void){
  if (debugmode){
    if (optophotommode){
      Serial.println("Optophotometry mode.");
    }
    else{
      Serial.println("Two-color photometry mode.");
    }
    
    Serial.print("Cam frequency (Hz): ");
    Serial.println(freq);
    Serial.print("Cam cycle time (us): ");
    Serial.println(cycletime);
    Serial.print("Cam on time (us): ");
    Serial.println(ontime);
  
    Serial.print("Ch1 photometry cycle time (us): ");
    Serial.println(cycletime_photom_1);
    Serial.print("Ch1 photometry on time (us): ");
    Serial.println(pulsewidth_1);
    Serial.print("Ch2 photometry cycle time (us): ");
    Serial.println(cycletime_photom_2);
    Serial.print("Ch2 photometry on time (us): ");
    Serial.println(pulsewidth_2);
  
    Serial.print("Opto freq (Hz): ");
    Serial.println(50 / opto_per);
    Serial.print("Opto pulses per train: ");
    Serial.println(train_length);
    Serial.print("Opto train cycle (s): ");
    Serial.println(train_cycle / 50);
  
    Serial.print("System-wide step time (us): ");
    Serial.println(step_size);
  }
}
  
