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

// Arm foodttl
void armfoodttl(void){
  // Food ttl
  foodttlarmed = true;
  foodttlwait = true;
  inputttl = !foodttlconditional;
  tfood0 = tnowmillis;

  if (usecue){
    foodttlcuewait = true;
    cueon = false;

    // Determine the trial type
    trialtype = gettrialtype();
    
    // Determine cue type for this trial
    cuetype = getcuetype(trialtype);
  }
  
  if (foodttlconditional){
    foodttlactionwait = true;
    actionperiodon = false;
  }
  
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
  
  // 0. Food not alarm and not during cue, nothing to do here - exit.
  if (~foodttlarmed && ~cueon){
    return;
  }
      
  // 1. Cue
  if (usecue){
    if (foodttlcuewait){
      // In waiting
      if ((tnowmillis - tfood0) >= cuedelay){
        // First time trying to get out of the cue delay
        foodttlcuewait = false;
        docueon(cuetype); // Cue on default type 1
        
        
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
        docueoff(cuetype);
        
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
        inputttl = checklicks(1); // Check licks (type 1)

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
          foodpulses_left = foodpulses;
          foodtype = getfoodtype(trialtype);
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
        foodpulses_left = foodpulses;
        foodtype = getfoodtype(trialtype);
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
        dofoodon(foodtype); // Food delivery on - default type 1
        
        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("Food pulse on at (ms): ");
          Serial.print(tnowmillis);
        }
      }
      else if (((tnowmillis - tfood1) >= (foodpulse_ontime)) && foodttlon){
        // Turn off
        dofoodoff(foodtype); // Food delivery off - default type 1
        
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

// Get trial type
byte gettrialtype(void){
  byte trialtypenow = 1;
  return trialtypenow;
}

// Get cue type
byte getcuetype(byte trialtype){
  byte cuetypenow = 1;
  switch (trialtype){
    case 1:
      cuetypenow = 1;
      break;
  }
  return cuetypenow;
}

// Cue on
// Type 1: tone PWM
void docueon(byte cuetype){
  switch (cuetype){
    case 1:
      tone(audiopin, audiofreq);
      cueon = true;
      break;
  }
}

// Cue off
// Type 1: tone PWM
void docueoff(byte cuetype){
  switch (cuetype){
    case 1:
      noTone(audiopin);
      cueon = false;
      break;
  }
}

// Get food type
// Type 1 pulse trains
byte getfoodtype(byte trialtype){
  byte foodtypenow = 1;
  switch (trialtype){
    case 1:
      foodtypenow = 1;
      break;
  }
  return foodtypenow;
}

// food on (also includes other reactions)
// Type 1: direct digital pin write
void dofoodon(byte foodtype){
  switch (foodtype){
    case 1:
      foodttlon = true;
      digitalWrite(foodTTLpin, HIGH);
//      digitalWrite(led_pin, HIGH);
      break;
  }
}

// food off (also includes other reactions)
// Type 1: direct digital pin write
void dofoodoff(byte foodtype){
  switch (foodtype){
    case 1:
      foodttlon = false;
      digitalWrite(foodTTLpin, LOW);
//      digitalWrite(led_pin, LOW);
      break;
  }
}

// Check licks
// Type 1: digital read of food TTL pin
bool checklicks(byte licktype){
  bool lickout = false;
  switch (licktype){
    case 1:
      lickout = digitalRead(foodTTLinput); // active high 
      break;
  }
  return lickout;
}
