#include<Wire.h>

// Debug
#define debug false

// I2c (doesn't work with interrupt apparently)
#define usei2c false
#define i2caddress 0x11
byte m, mecho;

// Dip switches
#define useDefpin true // if set false here go to below to change ch1_max and ch2_max

// Pins
const byte ch1_input = 18;
const byte ch1_input2 = 19;
const byte ch2_input = 20;
const byte ch2_input2 = 21;
const byte ch1_output[4] = {2, 3, 4, 5};
const byte ch2_output[4] = {6, 7, 8, 9};
const byte ledpin = 25;

// Def pin to set maxes (dip switches)
const byte defpin1 = 10; // Channel 1 high bit
const byte defpin2 = 11; // Channel 1 low bit
const byte defpin3 = 12; // Channel 2 high bit
const byte defpin4 = 13; // Channel 2 low bit

// channels
const byte max_ch = 4;
volatile byte ch1, ch2;
volatile byte ch1_ind = 0;
volatile byte ch2_ind = 0;
volatile byte ch1_max = 2; // 4 means 0 - 3 (ABCDABCD shift)
volatile byte ch2_max = 2; // 4 means 0 - 3 (ABCDABCD shift)

// flags
volatile bool checktimer = true;
volatile bool timergood = false;
volatile bool resettimer = false;

// Timers
long int t0, t1;
long int ipi = 50; // Interpulse interval 50 us

// Operation core
void setup() {
 // put your main code here, to run repeatedly:
  

  // Pins
  pinMode(ch1_input, INPUT_PULLDOWN);
  pinMode(ch2_input, INPUT_PULLDOWN);
  for (byte i = 0; i < max_ch; i++){
    pinMode(ch1_output[i], OUTPUT);
    pinMode(ch2_output[i], OUTPUT);
    digitalWrite(ch1_output[i], LOW);
    digitalWrite(ch2_output[i], LOW);
  }

  // Definition Pins
  pinMode(defpin1, INPUT_PULLDOWN);
  pinMode(defpin2, INPUT_PULLDOWN);
  pinMode(defpin3, INPUT_PULLDOWN);
  pinMode(defpin4, INPUT_PULLDOWN);
  
  if (useDefpin){
    byte df1 = digitalRead(defpin1);
    byte df2 = digitalRead(defpin2);
    byte df3 = digitalRead(defpin3);
    byte df4 = digitalRead(defpin4);
    ch1_max = df1 * 2 + df2 + 1;
    ch2_max = df3 * 2 + df4 + 1;
  }
  
  // Interrupts
  attachInterrupt(digitalPinToInterrupt(ch1_input), ch1_send, RISING);
  attachInterrupt(digitalPinToInterrupt(ch2_input), ch2_send, RISING);
  attachInterrupt(digitalPinToInterrupt(ch1_input2), ch1_shift, FALLING);
  attachInterrupt(digitalPinToInterrupt(ch2_input2), ch2_shift, FALLING);
  
  t0 = micros();
}

void loop() {
  // Update timer
  t1 = micros();

  // Timer reset
  if (resettimer){
    t0 = t1;
    resettimer = false;
  }

  // Check timer is good
  if (checktimer){
    if ((t1 - t0) > ipi){
      timergood = true;
      checktimer = false;
    }
  }
}

// I2c core
void setup1(){
  if (debug){
    Serial.begin(9600);
    Serial.println("Debug mode.");
  }
  
  // Wire
  if (usei2c){
    Wire.setSDA(0);
    Wire.setSCL(1);
    Wire.begin(i2caddress);
    Wire.onReceive(updatemax);
    Wire.onRequest(echomax);
  }
  
  pinMode(ledpin, OUTPUT);
}

void loop1(){
  if (debug){
    if (Serial.available() > 1){
      Serial.read();
      Serial.read();
      Serial.print("I got ");
      Serial.print(m, BIN);
      Serial.print(" = ");
      Serial.print((m >> 4) & 0b00001111, BIN);
      Serial.print(" + ");
      Serial.print(m & 0b00001111, BIN);
      Serial.print(", meaning: ");
      Serial.print(ch1_max);
      Serial.print(" & ");
      Serial.println(ch2_max);
    }
  }
}

void updatemax(int idk){
  while (Wire.available() > 0){
    m = Wire.read();
    byte m1 = (m >> 4) & 0b00001111; // High 4 bits
    byte m2 = m & 0b00001111; // Low 4 bits
    if (m2 > max_ch){
      m2 = max_ch;
    }

    if (debug){
      Serial.print(m, BIN);
      Serial.print(" = ");
      Serial.print(m1, BIN);
      Serial.print(" + ");
      Serial.print(m2, BIN);
      Serial.print(". ");
    }
    
    switch (m1){
      case 0:
        ch1_max = m2;
        break;
      case 1:
        ch2_max = m2;
        break;
    }

    if (debug){
      Serial.print(" Ch1 max: ");
      Serial.print(ch1_max);
      Serial.print(". Ch2 max: ");
      Serial.println(ch2_max);
    }
    
  }
}

void echomax(){
  mecho = (ch1_max << 4) + ch2_max;
  Wire.write(mecho);
}

void ch1_send(void){
  if (timergood){
    if (ch1_ind == 0){
      digitalWrite(ledpin, HIGH);
    }
    digitalWrite(ch1, HIGH);
    timergood = false;
    resettimer = true;
    checktimer = true;
  }
}

void ch2_send(void){
  if (timergood){
    digitalWrite(ch2, HIGH);
    timergood = false;
    resettimer = true;
    checktimer = true;
  }
}

void ch1_shift(void){
  if (timergood){
    digitalWrite(ch1, LOW);
    digitalWrite(ledpin, LOW);
    
    // Shift
    ch1_ind++;
    if (ch1_ind >= ch1_max){
      ch1_ind = 0;
    }
    ch1 = ch1_output[ch1_ind];
    
    timergood = false;
    resettimer = true;
    checktimer = true;
  }
}

void ch2_shift(void){
  if (timergood){
    digitalWrite(ch2, LOW);

    // Shift
    ch2_ind++;
    if (ch2_ind >= ch2_max){
      ch2_ind = 0;
    }
    ch2 = ch2_output[ch2_ind];
    
    timergood = false;
    resettimer = true;
    checktimer = true;
  }
}
