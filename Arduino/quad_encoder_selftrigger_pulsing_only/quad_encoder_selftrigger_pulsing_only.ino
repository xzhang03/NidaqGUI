#include <DIO2.h>

// This is now the DUE version 

// #include <Encoder.h>

// Encoder myEnc(2,3); // pick your pins, reverse for sign flip

bool pulsing = false;
byte freq = 30; // Pulse at 30 Hz
unsigned int ontime = 10000; // On for 10 ms
unsigned int cycletime = 1000000 / freq;
// unsigned int sweep_start;
unsigned long int pulsetime = 0;
bool onoff = false;
unsigned long int tnow;
unsigned int step_size = 100; // in micros
byte m;



void setup() {
  Serial.begin(9600);
  //SerialUSB.begin(115200); // for real-time feedback
  // myEnc.write(0);
  
  pinMode2(12, OUTPUT);
  digitalWrite2(12, LOW);
  
  Serial.print("Frequency (Hz): ");
  Serial.println(freq);
  Serial.print("Cycle time (us): ");
  Serial.println(cycletime);
  Serial.print("On time (us): ");
  Serial.println(ontime);
  Serial.print("Step size (us): ");
  Serial.println(step_size);
}

void loop() {
//  sweep_start = micros();
  
  if (Serial.available() > 0){
    m  = Serial.read();
    
    if (m == 1){
      pulsing = true;
      pulsetime = micros();
    }
    else if (m < 1){
      pulsing = false;
    }
  }

  tnow = micros();
  
  if (pulsing){
    if (((tnow - pulsetime) % cycletime) <= (ontime)){
      if (~onoff){
        digitalWrite2(12, HIGH);
        onoff = true;
      }
    }
    else {
      if (onoff){
        digitalWrite2(12, LOW);
        onoff = false;
      }
    }
  }

  delayMicroseconds(step_size);
//  Serial.println(micros() - sweep_start);
}
  
