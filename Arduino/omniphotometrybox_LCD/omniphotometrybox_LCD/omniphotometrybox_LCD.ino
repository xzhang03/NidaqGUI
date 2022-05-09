#include <Adafruit_DotStar.h> // v1.2 doesn't work. Use v1.15
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
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

// =============== Modes ===============
// Photometry mode
bool optophotommode = false; // Green + red
bool tcpmode = true; //
bool samecoloroptomode = false; // Green + blue (same led for photometry and opto);
bool useencoder = true;
bool usefoodpulses = false; //
bool syncaudio = false;
bool usescheduler = false; // 
bool manualscheduleoverride = false; // Only works when using scheduler (keep false if the opto input is left floating)
bool listenmode = false;
bool usebuzzcue = false;
bool useRNG = false; // RNG for opto
bool randomiti = false; // Randomize ITI


// =============== Time ===============
// General time variables
unsigned long int tnow, tnowmillis;

// ============== cam ==============
byte freq = 30; // Pulse at 30 Hz (default)


// ============ Photometry ============
// tcp photometry time variables
const int pulsewidth_1_tcp = 6000; // in micro secs (ch2)
const int pulsewidth_2_tcp = 6000; // in micro secs (ch2)
const unsigned long cycletime_photom_1_tcp = 10000; // in micro secs (Ch1)
const unsigned long cycletime_photom_2_tcp = 10000; // in micro secs (Ch2)

// ============ Opto ============
// Opto varaibles
byte opto_per = 5; // Number of photometry pulses per opto pulse (A). Can be: 1, 2, 5, 10, 25, 50. Pulse freq is 50 / A
byte train_length = 10; // Number of opto pulses per train (B). Duration is  B / (50 / A).
long train_cycle = 30 * 50; // First number is in seconds. How often does the train come on.
int pulsewidth_2_opto = 10000; // in micro secs (ch2)
unsigned long cycletime_photom_1_opto = 6500; // in micro secs (Ch1)
unsigned long cycletime_photom_2_opto = 13500; // in micro secs (Ch2)

// Same color opto variables
byte scopto_per = 5; // Number of photometry pulses per opto pulse (AO). Can be: 1, 2, 5, 10, 25, 50. Pulse freq is 50 / AO
byte sctrain_length = 10; // Number of photometry pulses per stim period (BO). Duration is  BO / (50 / AO).
long sctrain_cycle = 30 * 50; // First number is in seconds. How often does the train come on. 
unsigned int pulsewidth_1_scopto = 10000; // in micro secs (ch1)
const unsigned long cycletime_photom_1_scopto = 20000; // in micro secs (Ch1)
const unsigned long cycletime_photom_2_scopto = 0; // in micro secs (Ch2). Irrelevant

// ============ Scheduler ============
unsigned int preoptotime = 120; //in seconds (max is high because unsigned int is 32 bit for teensy)
unsigned int npreoptopulse = preoptotime * 50; // preopto pulse number
unsigned int ipreoptopulse = 0; // preopto pulse number
unsigned int ntrain = 10; // Number of trains
byte itrain = 0; // Current number of trains
bool inpreopto = true;
bool inopto = false;
bool inpostopto = false;


// ============ Hardware RNG ============
bool trainpass = true;

// Randomize ITI
byte rng_cycle_min = 30; // Min cycle when ITI is randomized (in seconds, assuming 20 ms pulse cycle)
byte rng_cycle_max = 40; // Max cycle when ITI is randomized (in seconds, assuming 20 ms pulse cycle)

// ============ Food TTL ============
// Food TTL (basically sync'ed with opto)
unsigned int nfoodpulsedelay = 2000; // Time after opto pulse train onset in ms
unsigned int foodpulse_ontime = 150; // in ms
unsigned int foodpulse_cycletime = 300; // in ms
byte foodpulses = 5; // Number of food ttl pulses per stim period.
byte foodpulses_left = 0; // Try counting down this time. Probably easier to debug

// ======== Food TTL Conditional ========
bool foodttlconditional = false; //
unsigned int buzzdelay = 2000; //
unsigned int buzzdur = 1000; // Time for buzzer cue 
unsigned int actiondelay = 2000;
unsigned int actiondur = 5000;
unsigned int deliverydur = 5000;

// Flags
bool pulsing = false;
bool refresh = true;

void setup() {
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
  
//  ledswitch();
  
  display.clearDisplay();
  display.setCursor(0, 0);
  rowprintln("12345678901234567890", 1);
  rowprintln("12345678901234567890", 2);
  rowprintln("Same F&S  Fill Line", 1);
  rowprintln("StrC StrW Clip Color", 1);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (refresh){
    ledswitch();
    refresh = false;
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
