// Food/reward functions go here

// Arm foodTTL in the food-then-opto mode (TCP/Opto/scopto). Scheduler or not. 
void preopto_rev_arm(void){
  if ((!optothenfood) && (usefoodpulses) && (pulsing) && (counter_for_train_complement == nfoodpulsedelay_complement)){
    // Only applies to food-then-opto mode here
    armfoodttl(); // Arm food ttl
  }
  /*
  else if ((debugmode) && (showfoodttl) && (!optothenfood) && (usefoodpulses) && (pulsing)){
    if ((counter_for_train_complement % 50) == 0){
      Serial.print(counter_for_train_complement);
      Serial.println(" pulses until food arming.");
    }
  }
  */
}

// Arm foodttl (This is a bottleneck that all behavioral tasks have to go through).
void armfoodttl(void){
  // Food ttl
  foodttlarmed = true;
  foodttlwait = true;
  tfood0 = tnowmillis;

  // Determine the trial type
  trialtype = gettrialtype();
    
  if (usecue){
    foodttlcuewait = true;
    cueon = false;

    // Multiple trial type
    cuedur = cuedur_vec[trialtype];
  }

  // Determine conditional
  foodttlconditional = foodttlconditional_vec[trialtype];
  inputttl = !foodttlconditional;
  if (foodttlconditional){
    foodttlactionwait = true;
    actionperiodon = false;

    // Multi trial types
    deliverydur = deliverydur_vec[trialtype];
  }

  // Delivery (multi trial types)
  foodpulse_ontime = foodpulse_ontime_vec[trialtype];
  foodpulse_cycletime = foodpulse_cycletime_vec[trialtype];
  foodpulses = foodpulses_vec[trialtype];
    
  if ((debugmode || serialdebug) && showfoodttl){
    Serial.print("Food TTL armed at (ms): ");
    Serial.println(tfood0);
  }
}

// Food TTL
void foodttl(void){
//  unsigned int tfooddelta = (tnowmillis - tfood0);
//  if ((tfooddelta % 100) == 0){
//    Serial.println(tfooddelta);
//  }
  
  // 1. Cue
  if (usecue){
    if (foodttlcuewait){
      // In waiting
      if ((tnowmillis - tfood0) >= cuedelay){
        // First time trying to get out of the cue delay
        foodttlcuewait = false;
        docueon(trialio, trialtype); // Cue on default type 1
        
        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("Cue on at (ms): ");
          Serial.println(tnowmillis);
        }
      }
    }
    else if (cueon){
      // After waiting and cue is on
      if ((tnowmillis - tfood0) >= (cuedelay + cuedur)){
        // Cue long enough and time to turn off (default type 1)
        docueoff(trialio);
        
        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("Cue off at (ms): ");
          Serial.println(tnowmillis);
        }
      }
    }
  }

  // 2. Action
  if (foodttlconditional){
    if (foodttlactionwait){
      // Waiting period for action
      if ((tnowmillis - tfood0) >= actiondelay){
        // First time trying to get out of the action delay
        foodttlactionwait = false;
        actionperiodon = true;

        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("Action period on at (ms): ");
          Serial.println(tnowmillis);
        }
      }
    }
    else if (actionperiodon){
      if(!inputttl){
        // In action perdio until getting a pulse
        inputttl = checklicks(trialio); // Check licks

        if ((debugmode || serialdebug) && showfoodttl){
          if (inputttl){
            Serial.print("Food TTL detected at (ms): ");
            Serial.println(tnowmillis);
          }
        }
      }
      // Trying to end action period
      if ((tnowmillis - tfood0) >= (actiondelay + actiondur)){
        // Action period long enough
        actionperiodon = false;

        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("Action period off at (ms): ");
          Serial.println(tnowmillis);
        }
      }
    }
  }
  
  // 3. Delivery (waiting period)
  // In the waiting period for delivery
  if (foodttlwait){
    // In waiting
    if (((tnowmillis - tfood0) >= nfoodpulsedelay) && ((tnowmillis - tfood0) < (nfoodpulsedelay + deliverydur))){
      // After minimal food pulse delay (in terms of pulses) but before time out
      // Trying to get out of the waiting period
      if (foodttlconditional){
        // Trying to get out of the waiting period (happens when detection or time out (below)
        if (inputttl){
          // Conditional and licked
          // Check RNG
          if (usefoodRNG){
            if (foodpass){
              // RNG passed
              foodpulses_left = foodpulses;
            }
            else{
              // RNG did not pass
              foodpulses_left = 0;
            }
          }
          else{
            // No food rng
            foodpulses_left = foodpulses;
          }
          
          foodttlwait = false;
          foodttlon = false;
          tfood1 = tfood0; // % Setup cycle time for the actual delivery
          if ((debugmode || serialdebug) && showfoodttl){
            Serial.print("Starting food delivery with ");
            Serial.print(foodpulses_left);
            Serial.print(" Pulses at (ms): ");
            Serial.println(tnowmillis);
          }
        }
      }
      else{
        // Unconditional
        // Check RNG
        if (usefoodRNG){
          if (foodpass){
            // RNG passed
            foodpulses_left = foodpulses;
          }
          else{
            // RNG did not pass
            foodpulses_left = 0;
          }
        }
        else{
          // No food rng
          foodpulses_left = foodpulses;
        }
        
        foodttlwait = false;
        foodttlon = false;
        tfood1 = tfood0; // % Setup cycle time for the actual delivery
        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("Starting food delivery with ");
          Serial.print(foodpulses_left);
          Serial.print(" Pulses at (ms): ");
          Serial.println(tnowmillis);
        }
      }
    }

    // Time out
    if ((tnowmillis - tfood0) >= (nfoodpulsedelay + deliverydur)){
      // Should never reach here unless conditional
      foodttlwait = false;
      foodttlon = false;
      if ((debugmode || serialdebug) && showfoodttl){
        Serial.print("Food delivery timed out at (ms): ");
        Serial.println(tnowmillis);
      }
    }
  }

  // 4. Delivery (Out of the waiting period)
  else{
    if (foodpulses_left > 0){
      // Still pulses left
      if (((tnowmillis - tfood1) >= (foodpulse_cycletime)) && !foodttlon){
        // Beginning of each cycle
        tfood1 = tnowmillis;
        dofoodon(trialio); // Food delivery on
        
        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("Food pulse on at (ms): ");
          Serial.print(tnowmillis);
        }
      }
      else if (((tnowmillis - tfood1) >= (foodpulse_ontime)) && foodttlon){
        // Turn off
        dofoodoff(trialio); // Food delivery off
        
        foodpulses_left--;
        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("-");
          Serial.print(tnowmillis);
          Serial.print(". Pulses left: ");
          Serial.println(foodpulses_left);
        }
      }
    }
    else if (foodttlon){
      // No pulses left
      foodttlarmed = false;
      foodttlon = false;

      if ((debugmode || serialdebug) && showfoodttl){
        Serial.print("Food delivery done at (ms): ");
        Serial.println(tnowmillis);
      }
    }
  }
}

// Get trial IO
// Trial IO is a 16-bit integer with the following bit mapping
// 15-13: Cue type (choose 1)
// Bit:                                        15  14  13
// PWM tone/LED cue (1 channel):              [ 1   0   0]
// Digital cue using dio expander (MCP23008): [ 0   1   0]
// PWM cue using i2c led (PCA9685):           [ 0   0   1]
// Internal RGB pwm (3 channels):             [ 1   0   1]

// 12-4: i2c led mode only
// 12-10: 3 bit of R intensty
// 9-7: 3 bit of G intensity
// 6-4: 3 bit of B intensity

// 12-4: DIO mode (MCP23008)
// 12-9: 4 bit of 4 output channels (12 - DO7, 11 - DO6, 10 - DO5, 9 - DO4)

// 3: 1 - active high action, 0 - active low action
// 2: 1 - use DIO to delivery outcome, 0 - use native nanosec food port
// 1-0: 2 bit DIO port number (DIO delivery mode only)

byte gettrialtype(void){
  byte trialtypenow = 0;

  if (trialrngwrite == 255){
    // Trial-by-trial determination of RNG'ed trial types 
    if (ntrialtypes == 1){
      trialtypenow = 0;
      trialio = trialio0;
    }
    else{
      if (rngvec_trialtype[itrain] < freq_cumvec[0]){
        trialtypenow = 0;
        trialio = trialio0;
      }
      else if (rngvec_trialtype[itrain] < freq_cumvec[1]){
        trialtypenow = 1;
        trialio = trialio1;
      }
      else if (rngvec_trialtype[itrain] < freq_cumvec[2]){
        trialtypenow = 2;
        trialio = trialio2;
      }
      else if (rngvec_trialtype[itrain] < freq_cumvec[3]){
        trialtypenow = 3;
        trialio = trialio3;
      }
    }
  }
  else {
    // Deterministic trial type
    trialtypenow = detvec_trialtype[itrain_dtt];
    if (trialtypenow == 0){
      trialio = trialio0;
    }
    else if (trialtypenow == 1){
      trialio = trialio1;
    }
    else if (trialtypenow == 2){
      trialio = trialio2;
    }
    else if (trialtypenow == 3){
      trialio = trialio3;
    }
  }
  return trialtypenow;
}

// Cue on
// Type 1: tone PWM
void docueon(uint16_t trialio, uint8_t trialtype){
  byte cuetype = getcuetype(trialio); // 0 - native pwm, 1 - MCP23008 DIO, 2 - external PWM, 3 - internal RGB pwm
  const uint16_t cscale[8] = {0, 3, 11, 35, 114, 374, 1223, 3998}; // Color scale (log scale, max 4095). This is for external PWM (PCA9685).
  const uint8_t cscale2[8] = {0, 4, 8, 16, 32, 64, 128, 255}; // Color scale (log scale, max 255). This is for internal RGB PWM.
  byte Rv = (trialio >> 10) & 0b111 ; // Red value
  byte Gv = (trialio >> 7) & 0b111 ; // Green value
  byte Bv = (trialio >> 4) & 0b111 ; // Blue value
  cueon = true;
  
  switch (cuetype){
    case 0:
      // Native PWM
      tone(audiopin, audiofreq);

      #if useMCP23008
        // Hardcoded trial type flags
        switch (trialtype){
          case 0:
            MCP.digitalWrite(4, HIGH);
            break;
          case 1:
            MCP.digitalWrite(5, HIGH);
            break;
          case 2:
            MCP.digitalWrite(6, HIGH);
            break;
          case 3:
            MCP.digitalWrite(7, HIGH);
            break;
        }
      #endif
      break;

    case 1:
      // MCP23008
      #if useMCP23008
        MCP.digitalWrite(4, bitRead(trialio, 9));
        MCP.digitalWrite(5, bitRead(trialio, 10));
        MCP.digitalWrite(6, bitRead(trialio, 11));
        MCP.digitalWrite(7, bitRead(trialio, 12));
      #endif
      #if universalcuemarker
        digitalWrite(audiopin, HIGH);
      #endif
      break;

    case 2:
      // External pwm
      #if usePCA9685
        pwm.setPin(0, cscale[Rv], false);
        pwm.setPin(1, cscale[Gv], false);
        pwm.setPin(2, cscale[Bv], false);
      #endif
      
      #if useMCP23008
        // Hardcoded trial type flags
        switch (trialtype){
          case 0:
            MCP.digitalWrite(4, HIGH);
            break;
          case 1:
            MCP.digitalWrite(5, HIGH);
            break;
          case 2:
            MCP.digitalWrite(6, HIGH);
            break;
          case 3:
            MCP.digitalWrite(7, HIGH);
            break;
        }
      #endif

      #if universalcuemarker
        digitalWrite(audiopin, HIGH);
      #endif
      break;

    case 3:
      // Internal pwm (using extra pins[1-3], i.e., pins 7, 8, 9)
      analogWrite(extrapins[1], cscale2[Rv]);
      analogWrite(extrapins[2], cscale2[Gv]);
      analogWrite(extrapins[3], cscale2[Bv]);
      
      #if useMCP23008
        // Hardcoded trial type flags
        switch (trialtype){
          case 0:
            MCP.digitalWrite(4, HIGH);
            break;
          case 1:
            MCP.digitalWrite(5, HIGH);
            break;
          case 2:
            MCP.digitalWrite(6, HIGH);
            break;
          case 3:
            MCP.digitalWrite(7, HIGH);
            break;
        }
      #endif

      #if universalcuemarker
        digitalWrite(audiopin, HIGH);
      #endif
      break;
  }
}

// Cue off
// Type 1: tone PWM
void docueoff(uint16_t trialio){
  byte cuetype = getcuetype(trialio); // 0 - native pwm, 1 - MCP23008 DIO, 2 - external PWM, 3 - internal RGB pwm
  byte Rv = (trialio >> 10) & 0b111 ; // Red value
  byte Gv = (trialio >> 7) & 0b111 ; // Green value
  byte Bv = (trialio >> 4) & 0b111 ; // Blue value
  cueon = false;
  
  switch (cuetype){
    case 0:
      // Native PWM
      noTone(audiopin);
      #if useMCP23008
        MCP.digitalWrite(4, LOW);
        MCP.digitalWrite(5, LOW);
        MCP.digitalWrite(6, LOW);
        MCP.digitalWrite(7, LOW);
      #endif
      break;

    case 1:
      // MCP23008
      #if useMCP23008
        MCP.digitalWrite(4, 0);
        MCP.digitalWrite(5, 0);
        MCP.digitalWrite(6, 0);
        MCP.digitalWrite(7, 0);
      #endif
      #if universalcuemarker
        digitalWrite(audiopin, LOW);
      #endif
      break;

    case 2:
      // External pwm
      #if usePCA9685
        if (Rv > 0){
          pwm.setPin(0, 0, false);
        }
        if (Gv > 0){
          pwm.setPin(1, 0, false);
        }
        if (Gv > 0){
          pwm.setPin(2, 0, false);
        }
      #endif
      
      #if useMCP23008
        MCP.digitalWrite(4, LOW);
        MCP.digitalWrite(5, LOW);
        MCP.digitalWrite(6, LOW);
        MCP.digitalWrite(7, LOW);
      #endif

      #if universalcuemarker
        digitalWrite(audiopin, LOW);
      #endif
      break;

    case 3:
      // Internal pwm
      if (Rv > 0){
        pinMode(extrapins[1], OUTPUT);
        digitalWrite(extrapins[1], LOW);
      }
      if (Gv > 0){
        pinMode(extrapins[2], OUTPUT);
        digitalWrite(extrapins[2], LOW);
      }
      if (Bv > 0){
        pinMode(extrapins[3], OUTPUT);
        digitalWrite(extrapins[3], LOW);
      }
      
      #if useMCP23008
        MCP.digitalWrite(4, LOW);
        MCP.digitalWrite(5, LOW);
        MCP.digitalWrite(6, LOW);
        MCP.digitalWrite(7, LOW);
      #endif

      #if universalcuemarker
        digitalWrite(audiopin, LOW);
      #endif
      break;
      
  }
}

// food on (also includes other reactions)
// Type 1: direct digital pin write
void dofoodon(uint16_t trialio){
  byte foodtype = bitRead(trialio, 2); // 1 - DIO board, 0 - native
  byte DIOport = trialio & 0b11; // DO 0-3
  foodttlon = true;
  
  switch (foodtype){
    case 0:
      digitalWrite(foodTTLpin, HIGH);
      break;
    case 1:
      #if useMCP23008
        MCP.digitalWrite(DIOport, HIGH);
      #endif
      break;
  }
}

// food off (also includes other reactions)
// Type 1: direct digital pin write
void dofoodoff(uint16_t trialio){
  byte foodtype = bitRead(trialio, 2); // 1 - DIO board, 0 - native
  byte DIOport = trialio & 0b11; // DO 0-3
  foodttlon = false;
  
  switch (foodtype){
    case 0:
      digitalWrite(foodTTLpin, LOW);
      break;
    case 1:
      #if useMCP23008
        MCP.digitalWrite(DIOport, LOW);
      #endif
      break;
  }
}

// Check licks
// Type 1: digital read of food TTL pin
bool checklicks(uint16_t trialio){
  bool lickout = false;
  byte licktype = bitRead(trialio, 3); // 1 - active high, 0 - active low
  
  switch (licktype){
    case 1:
      lickout = digitalRead(foodTTLinput); // active high 
      break;
    case 0:
      lickout = !digitalRead(foodTTLinput); // active low
      break;
  }
  return lickout;
}

// Get cue type
// 15-13: Cue type (choose 1)
// Bit:                                           15  14  13
// 0. PWM tone/LED cue (1 channel):              [ 1   0   0]
// 1. Digital cue using dio expander (MCP23008): [ 0   1   0]
// 2. PWM cue using i2c led (PCA9685):           [ 0   0   1]
// 3. Internal RGB pwm (3 channels):             [ 1   0   1]
uint8_t getcuetype(uint16_t trialioprivate){
  uint8_t cuetype;
  switch ((trialioprivate >> 12) & 0B111){
    case 0B100:
      cuetype = 0;
      break;
    
    case 0B010:
      cuetype = 1;
      break;

    case 0B001:
      cuetype = 2;
      break;

    case 0B101:
      cuetype = 3;
      break;
  }  
  return cuetype;
}

// Test food
// This code is blocking, unfortunately.
void testfoodttls(uint8_t testclicks){
  uint16_t itestfoodttls;
  for (itestfoodttls = 0; itestfoodttls < testclicks; itestfoodttls++){
    digitalWrite(foodTTLpin, HIGH);
    delay(150);
    digitalWrite(foodTTLpin, LOW);
    delay(150);
  } 
}
