#include <DIO2.h>

// handle the damn led
#include <Adafruit_DotStar.h>
#include <SPI.h>
#define NUMPIXELS 1 // Number of LEDs in strip
#define DATAPIN 7
#define CLOCKPIN 8
Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);


// This is now the DUE version 

// #include <Encoder.h>

#define debugmode false

// Encoder myEnc(2,3); // pick your pins, reverse for sign flip
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
const int pulsewidth = 6000; // in micro secs
const long cycletime_photom = 10000; // in micro secs

// photometry variables
bool ch1_on = false;
bool ch2_on = false;




void setup() {
  Serial.begin(9600);
  //SerialUSB.begin(115200); // for real-time feedback
  // myEnc.write(0);
  
  pinMode2(cam_pin, OUTPUT);
  pinMode2(led_pin, OUTPUT);
  pinMode2(ch1_pin, OUTPUT);
  pinMode2(ch2_pin, OUTPUT);
  digitalWrite2(cam_pin, LOW);
  digitalWrite2(led_pin, LOW);
  digitalWrite2(ch1_pin, LOW);
  digitalWrite2(ch2_pin, LOW);
  
  /*
  Serial.print("Frequency (Hz): ");
  Serial.println(freq);
  Serial.print("Cycle time (us): ");
  Serial.println(cycletime);
  Serial.print("On time (us): ");
  Serial.println(ontime);
  Serial.print("Step size (us): ");
  Serial.println(step_size);
  */
  
  if (debugmode){
    Serial.println("Experiment start.");  
    Serial.print("Cycle time: ");
    Serial.print(cycletime);
    Serial.println(" us.");
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
    
    /*
    if (m == 5){
      // Give position
      pos = myEnc.read();
      Serial.write((byte *) &pos, 4);
    }
    */
    
    if (m == 2){
      // Set frequency
      cycletime = 1000000 / n;

      if (debugmode){
        Serial.print("New cycle time: ");
        Serial.print(cycletime);
        Serial.println(" us.");
      }
    }
    else if (m == 1){
      // Start pulsing
      pulsing = true;
      pulsetime = micros();
      if (debugmode){
        Serial.println("Pulse start.");
      }
      // myEnc.write(0);    // zero the position
      // pos = 0;
    }
    else if (m == 0){
      // End pulsing
      pulsing = false;
      if (debugmode){
        Serial.println("Pulse stop.");
      }
    }
    else if (m == 5){
      // Echo back 0
      Serial.write((byte *) &pos, 4);
    }
  }
  
  tnow = micros();
  
  // pos = myEnc.read();
  // Cam
  if (pulsing){
    if ((tnow - pulsetime) >= cycletime){
      if (~onoff){
        pulsetime = micros();
        Serial.write((byte *) &pos, 4);
        digitalWrite2(cam_pin, HIGH);
        digitalWrite2(led_pin, HIGH);
        onoff = true;
      }
    }
    else if ((tnow - pulsetime) >= ontime){
      if (onoff){
        digitalWrite2(cam_pin, LOW);
        digitalWrite2(led_pin, LOW);
        onoff = false;
      }
    }
  }
  else {
    if (onoff){
      digitalWrite2(cam_pin, LOW);
      digitalWrite2(led_pin, LOW);
      onoff = false;
    }
  }

  // photometry
  t1 = tnow - t0;
  if ((t1 >= (cycletime_photom*2))){
    if (~ch1_on){
      t0 = micros();
      digitalWrite2(ch1_pin, HIGH);
      ch1_on = true;
    }
  }
  else if ((t1 >= (pulsewidth)) && (t1 < (cycletime_photom))){
    if (ch1_on){
      digitalWrite2(ch1_pin, LOW);
      ch1_on = false;
    }
  }
  
  else if ((t1 >= (cycletime_photom)) && (t1 < (cycletime_photom + pulsewidth))){
    if (~ch2_on){
      digitalWrite2(ch2_pin, HIGH);
      ch2_on = true;
    }
  }
  else if (t1 >= (cycletime_photom + pulsewidth)){
    if (ch2_on){
      digitalWrite2(ch2_pin, LOW);
      ch2_on = false;
    }
  }
  
  delayMicroseconds(step_size);
//  Serial.println(micros() - sweep_start);
}
  
