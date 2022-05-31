#include <Adafruit_DotStar.h> // v1.2 doesn't work. Use v1.15
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define NUMPIXELS 1 // Number of LEDs in strip
#define DATAPIN 7
#define CLOCKPIN 8
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);


  
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// pin
const byte buttonpin = 3;

// What info do we want
/*
 * 1. First byte
 * 1.0 Pulsing - 1 bit
 * 1.1 Mode - 3 bits
 * 1.2 Scheduling on/off - 1 bit
 * 1.3 Preopto-opto-postopto - 3 bits
 *
 * 2. Second byte
 * itrial
 * 
 * 3. Third byte
 * ntrial
 * 
 * 4. Fourth byte
 * 4.0 Using RNG - 1 bit
 * 4.1 RNG pass - 1 bit
 * 4.2 Food pulses - 1 bit
 * 4.3 Food pulse conditional - 1 bit
 * 4.4 Use cue - 1 bit
 * 4.5 Animal got it - 1 bit
 * 
 */
// =================== I2C ===================
byte m, n, o, p;

// First byte
bool pulsing;
bool tcpmode, optophotommode, samecoloroptomode;
bool usescheduler;
bool inpreopto;
bool inopto;
bool inpostopto;

// Second byte
byte itrain;

// Third byte
byte ntrain;

// Fourth byte
bool useRNG;
bool trainpass;
bool usefoodpulses;
bool foodttlconditional;
bool usebuzzcue;
bool inputttl;

// Screen
bool refresh;


// =================== Display ===================
/* 1. Time/STOP + Mode
 * 2. Scheduler off / Preopto / Trial / Postopto
 * 3. Behavior
 * 4. RNG
 */
char buffer[20];
byte minute = 12;
byte second = 5;

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  
  // put your setup code here, to run once:
  // Oled Initilize
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  delay(500); // Pause for 1 second

  // pinMode
  pinMode(buttonpin, INPUT_PULLUP);
  
  // onboard LED
  strip.begin(); // Initialize pins for output
  strip.setBrightness(30);
  strip.show();  // Turn all LEDs off ASAP

  // Force update
  refresh = true;

  // Debug
  m = 0;
  bitSet(m, 0);
  bitSet(m, 3);
  bitSet(m, 4);
  bitSet(m, 6);

  n = 39;
  o = 50;

  p = 0;
  bitSet(p, 0);
  bitSet(p, 1);
  bitSet(p, 2);
  bitSet(p, 3);
  bitSet(p, 4);
  bitSet(p, 5);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (refresh){
    refresh = false;

    // LED
    ledswitch();

    // LCD
    parsei2c();
    updatelcd();
    
    
  }
}

// Oled prints
void rowprint(char t[], byte s){
  display.setTextSize(s);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F(t));

  display.display();
}

void rowprintln(char t[], byte s){
  display.setTextSize(s);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.println(F(t));

  display.display();
}

void ledswitch(void){
  if (pulsing){
    if (inpreopto){
      strip.setPixelColor(0, 0x404000);
      strip.show();  // Turn all LEDs off ASAP
    }
    if (inopto){
      strip.setPixelColor(0, 0x008000);
      strip.show();  // Turn all LEDs off ASAP
    }
    if (inpostopto){
      strip.setPixelColor(0, 0x004040);
      strip.show();  // Turn all LEDs off ASAP
    }
  }
  else{
    strip.setPixelColor(0, 0x800000);
    strip.show();  // Turn all LEDs off ASAP
  }
}

void parsei2c(void){
  // First byte
  pulsing = bitRead(m, 0);
  tcpmode = bitRead(m, 1);
  optophotommode = bitRead(m, 2);
  samecoloroptomode = bitRead(m, 3);
  usescheduler = bitRead(m, 4);
  inpreopto = bitRead(m, 5);
  inopto = bitRead(m, 6);
  inpostopto  = bitRead(m, 7);

  // Second byte
  itrain = n;

  // Third byte
  ntrain = o;

  // Fourth byte
  useRNG = bitRead(p, 0);
  trainpass = bitRead(p, 1);
  usefoodpulses = bitRead(p, 2);
  foodttlconditional = bitRead(p, 3);
  usebuzzcue = bitRead(p, 4);
  inputttl = bitRead(p, 5);

}

void updatelcd(void){
  display.clearDisplay();
  display.setCursor(0, 0);

  /* 1. Time/STOP + Mode
   * 2. Scheduler off / Preopto / Trial / Postopto
   * 3. Behavior
   * 4. RNG
   */
  // First row
  if (pulsing){
    sprintf(buffer, "%02d:%02d", minute, second);
    rowprint(buffer, 1);
  }
  else{
    rowprint("[READY]", 1);
  }
  if (tcpmode){
    rowprintln(" TCP", 1);
  }
  else if (optophotommode){
    rowprintln(" OptoPhotom", 1);
  }
  else if (samecoloroptomode){
    rowprintln(" SCoptoPhotom", 1);
  }

  // Second row
  if (!usescheduler){
    rowprintln("No Scheduler", 1);
  }
  else if (inpreopto){
    rowprintln("PRE-Opto", 1);
  }
  else if (inpostopto){
    rowprintln("POST-Opto", 1);
  }
  else {
    sprintf(buffer, "Trial: %03d/%03d", itrain, ntrain);
    rowprintln(buffer, 1);
  }

  // Third row
  if (!usefoodpulses){
    rowprintln("No Behavior", 1);
  }
  else if (!foodttlconditional) {
    rowprintln("Pavlovian", 1);
  }
  else {
    // Conditional
    if (usebuzzcue){
      rowprint("Cued Conditional: ", 1);
    }
    else{
      rowprint("Conditional: ", 1);
    }
    sprintf(buffer, "%d", inputttl);
    rowprintln(buffer, 1);
  }

  // Fourth row
  if (!useRNG || !usescheduler){
    rowprintln("No RNG", 1);
  }
  else{
    sprintf(buffer, "RNG: %d", trainpass);
    rowprint(buffer, 1);
  }
}
