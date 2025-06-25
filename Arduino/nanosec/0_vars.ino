

// ====== Camera, audio, and encoder ======
// time variables for camera
unsigned long int pulsetime = 0;
//unsigned int step_size = 10; // in micros

// cam trig variables
bool pulsing = false;
bool pulsing_test = false; 
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
bool manualon = false;
bool listenpol = true; // Polarity for listen mode false = active low, true = active high. Either way, it has to be 3.3V logic

// ============ Scheduler indicator ============
// This uses the same PCA9685 module as the PWM trial cue at the moment. Do not use the two at the same time
// May split out different adddresses in the future (in the 2_i2c tab)
bool useschedulerindicator = false;
byte preoptocolor =  0B01000011; // [0 Shared_bit R R G G B B], shared bit is applied to all of R, G, B of this byte
byte inoptocolor =   0B01001100;
byte postoptocolor = 0B01110000;
byte switchoff_indicator = 0; // 0: off, 1: stay, 2: preopto, 3: inopto, 4: postopto

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
bool inputttl = false; // Animal licked
bool foodttlwait = false; // In waiting for delivery
bool foodttlon = false;

// ======== Food TTL Conditional ========
// Cue: Opto start => cue delay => cue duration
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
unsigned int cuedelay = 2000; //
unsigned int cuedur = 1000; // Time for food cue
unsigned int actiondelay = 2000;
unsigned int actiondur = 5000;
unsigned int deliverydur = 5000; //
bool actionperiodon = false;

// ============ Multiple trial types ============
#define maxtrialtypes 4
byte trialtype = 0; // Trial type to use
byte trialtype_edit = 0; // Trial type to edit
byte ntrialtypes = 1; // If more than 1, RNG kicks in
byte rngvec_trialtype[maxrngind]; // Initialize array for RNG trial type
uint16_t trialio  = 0b0000000000001000;
uint16_t trialio0 = 0b0000000000001000; // See 2_food
uint16_t trialio1 = 0b0011110000001000;
uint16_t trialio2 = 0b0010001110001000;
uint16_t trialio3 = 0b0010000001111000;
byte freq_vec[maxtrialtypes] = {3, 0, 0, 0}; // Frequency weights of the trial types
byte freq_cumvec[maxtrialtypes] = {3, 3, 3, 3}; // Cumulative version of above
unsigned int cuedur_vec[maxtrialtypes] = {1000, 1000, 1000, 1000}; // Time for food cue
unsigned int foodpulse_ontime_vec[maxtrialtypes] = {150, 150, 150, 150};; // in ms
unsigned int foodpulse_cycletime_vec[maxtrialtypes] = {300, 300, 300, 300}; // in ms
byte foodpulses_vec[maxtrialtypes] = {5, 5, 5, 5}; // Number of food ttl pulses per stim period.
unsigned int deliverydur_vec[maxtrialtypes] = {5000, 5000, 5000, 5000};
bool foodttlconditional_vec[maxtrialtypes] = {false, false, false, false};

// ============== I2c ==============
bool usei2c = true;

// ============ Switches ============
// photometry and optophotometry switches

bool opto = false;
bool train = false;
bool lastpulseopto = false; // Last pulse coming up

// ============ Counters ============
// Counters
byte pmt_counter_for_opto = 0;
byte opto_counter = 0;
unsigned long counter_for_train = 0; // Number of cycles
unsigned long counter_for_train_complement = 0;

// ============ Performance check ============

#if perfcheck
  unsigned long int tnow_pc;
  unsigned long int cycles_pc = 1000000;
  bool pc_going = false;
  unsigned long int ix_pc = 0;
  long tpc;
#endif
