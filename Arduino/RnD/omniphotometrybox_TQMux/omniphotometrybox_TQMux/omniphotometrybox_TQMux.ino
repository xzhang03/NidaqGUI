// Multiplexer for using Nanosec (omniphotometrybox) with Thorlabs TCube
#include <Adafruit_SSD1327.h>

#define debugmode true

// photometry analogout = A0
// opto analogout = A1
// SCL pin = SCL
// SDA pin = SDA

// SSD1327
#define OLED_RESET -1
Adafruit_SSD1327 display(128, 128, &Wire, OLED_RESET, 1000000);

// Pins
const byte LEDsw = 11; // LED switch. This turns off pulsing mode (active high)
const byte LEDout = 10; // Output to drive an LED indicator
const byte SW1p = 9; // Push button, active high
const byte SW1b = 7; // Knob direction B, active high
const byte SW1a = 12; // Knob direction A, active high
bool encoderdir = true; // Swap if it's going the other way

// DAC
unsigned int val0 = 0;
unsigned int val1 = 0;
const unsigned int valmin = 0;
const unsigned int valmax = 4095;
unsigned int a0out = 255; // 12 bit
unsigned int a1out = 4095; // 12 bit
bool ch0update = false;
bool ch1update = false;

// UI
const byte incfine = 1;
const byte inccoarse = 100;
bool editing = false; // editing = not running
bool editled = false;
bool asel = false; // false = A0, true = A1
bool incsel = false; // false = fine, true = coarse
bool update = false;
byte ys[4] = {60, 78, 96, 114};
byte sqsel = 0; // Square select (0 = A0+, 1 = A0++, 2 = A1+, 3 = A1++)
const byte sqselmax = 3;
const byte sqselmin = 0;

// Time
unsigned long int t0, t1;

// Debounce
unsigned long int tdeb = 10000;
unsigned long int tdeblong = 200000;

// Debug
bool updateserial = false;

void setup() {
  Serial.begin(9600);
  display.begin(0x3D);
  delay(1000);

  if (debugmode){
    Serial.println("Testing screen");
  }
  
  
  // put your setup code here, to run once:
  pinMode(LEDsw, INPUT);
  pinMode(LEDout, OUTPUT);
  pinMode(SW1p, INPUT);
  pinMode(SW1b, INPUT);
  pinMode(SW1a, INPUT);

  // Interrupts
  attachInterrupt(digitalPinToInterrupt(SW1a), knob1, RISING);
  attachInterrupt(digitalPinToInterrupt(SW1b), knob2, RISING);
  attachInterrupt(digitalPinToInterrupt(SW1p), button, RISING);
  
  // Display
  display.clearDisplay();
  display.display();
  drawmenu();
  display.display();
//  
  t0 = micros();
}

void loop() {
  // put your main code here, to run repeatedly:

  t1 = micros();

  // Debug
  if (debugmode && updateserial){
    Serial.print("A: ");
    Serial.print(digitalRead(SW1a));
    Serial.print(" B: ");
    Serial.print(digitalRead(SW1b));
    Serial.print(" P: ");
    Serial.print(digitalRead(SW1p));
    Serial.print(" SW: ");
    Serial.print(digitalRead(LEDsw));
    Serial.print(" Val0: ");
    Serial.print(val0);
    Serial.print(" Val1: ");
    Serial.print(val1);
    Serial.print(" ASelect: ");
    Serial.print(asel);
    Serial.print(" IncSelect: ");
    Serial.println(incsel);
    updateserial = false;
  }

  // Editing vs runningmodes
  editing = digitalRead(LEDsw);
  if (editled != editing){
    digitalWrite(LEDout, editing);
    editled = editing;
    update = true;
  }

  // Update menu
  if (update){
    display.clearDisplay();
    drawmenu();
  }

  // Ch0
  if (ch0update){
    analogWrite(A0, val0);
    ch0update = false;
  }

  // Ch1
  if (ch1update){
    analogWrite(A1, val1);
    ch1update = false;
  }
  
  // Delays
  delayMicroseconds(100);
}

void knob1(void){
  if ((t1 - t0 >= tdeb) && editing){
    t0 = t1;
    updateserial = true;
    update = true;
    if (!digitalRead(SW1b)){
      if (encoderdir){
        if (asel){
          val1change(true);
          ch1update = true;
        }
        else{
          val0change(true);
          ch0update = true;
        }
      }
      else {
        if (asel){
          val1change(false);
          ch1update = true;
        }
        else {
          val0change(false);
          ch0update = true;
        }
      }
    }
  }
}

void knob2(void){
  if ((t1 - t0 >= tdeb) && editing){
    t0 = t1;
    updateserial = true;
    update = true;
    if (!digitalRead(SW1a)){
      if (encoderdir){
        if (asel){
          val1change(false);
          ch1update = true;
        }
        else{
          val0change(false);
          ch0update = true;
        }
      }
      else {
        if (asel){
          val1change(true);
          ch1update = true;
        }
        else{
          val0change(true);
          ch0update = true;
        }
      }
    }
  }
}

void button(void){
  if ((t1 - t0 >= tdeblong) && editing){
    t0 = t1;

    if (sqsel < sqselmax){
      sqsel++;
    }
    else{
      sqsel = 0;
    }

    asel = sqsel > 1; // 0-1 A0, 2-3 A1
    incsel = (sqsel % 2) == 1; // 0,2 = false = fine, 1,3 = true = course
    update = true;
    updateserial = true;
  }
}

void val0change(bool dir){
  Serial.println(dir);
  if (dir){
    // Increment
    if (incsel){
      // Coarse increment
      if (val0 + inccoarse <= valmax){
        val0 = val0 + inccoarse;
      }
      else{
        val0 = valmax;
      }
    }
    else{
      // fine increment
      if (val0 + incfine <= valmax){
        val0 = val0 + incfine;
      }
      else{
        val0 = valmax;
      }
    }
  }
  else{
    // Decrement
    if (incsel){
      // Coarse decrement
      if (val0 >= inccoarse){
        val0 = val0 - inccoarse;
      }
      else{
        val0 = 0;
      }
    }
    else{
      // fine decrement
      if (val0 >= incfine){
        val0 = val0 - incfine;
      }
      else{
        val0 = 0;
      }
     }
  }
}

void val1change(bool dir){
  if (dir){
    // Increment
    if (incsel){
      // Coarse increment
      if (val1 + inccoarse <= valmax){
        val1 = val1 + inccoarse;
      }
      else{
        val1 = valmax;
      }
    }
    else{
      // fine increment
      if (val1 + incfine <= valmax){
        val1 = val1 + incfine;
      }
      else{
        val1 = valmax;
      }
    }
  }
  else{
    // Decrement
    if (incsel){
      // Coarse decrement
      if (val1 >= inccoarse){
        val1 = val1 - inccoarse;
      }
      else{
        val1 = 0;
      }
    }
    else{
      // fine decrement
      if (val1 >= incfine){
        val1 = val1 - incfine;
      }
      else{
        val1 = 0;
      }
    }
  }
}

void drawmenu(void) {
  display.setTextSize(2);
  display.setTextWrap(false);
  display.setTextColor(SSD1327_WHITE);
  display.setCursor(0,0);
  display.println("  Nanosec");  
  display.println("   TQMux");
  if (editing){
    display.println("|>EDITING<|"); 
  }
  else{
    display.println(" *RUNNING*");
  }

  display.setCursor(0,ys[0]);
  display.print("A0: ");  
  display.print(val0);

  display.setCursor(0, ys[2]);
  display.print("A1: ");
  display.print(val1);  
  
  if (editing){
    display.setCursor(109, ys[0]);
    display.print("+");
    display.setCursor(103, ys[1]);
    display.print("++");
    display.setCursor(109, ys[2]);
    display.print("+");
    display.setCursor(103, ys[3]);
    display.print("++");
  
    display.drawRect(101, ys[sqsel], 26, 14, SSD1327_WHITE);
    display.drawRect(100, ys[sqsel]-1, 28, 16, SSD1327_WHITE);
  }

  display.display();
  

  update = false;
}
