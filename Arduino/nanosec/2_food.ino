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

  if (usebuzzcue){
    foodttlcuewait = true;
    cueon = false;
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
  if (usebuzzcue){
    if (foodttlcuewait){
      // In waiting
      if ((tnowmillis - tfood0) >= buzzdelay){
        // First time trying to get out of the buzz delay
        foodttlcuewait = false;
        tone(audiopin, audiofreq);
        cueon = true;
        
        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("Cue on at (ms): ");
          Serial.println(tnowmillis);
        }
      }
    }
    else if (cueon){
      // After waiting and cue is on
      if ((tnowmillis - tfood0) >= (buzzdelay + buzzdur)){
        // Cue long enough
        cueon = false;
        noTone(audiopin);
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
        inputttl = digitalRead(foodTTLinput); // active high

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
  
  // 3. Delivery
  // In the waiting period for delivery
  if (foodttlwait){
    // In waiting
    if (((tnowmillis - tfood0) >= nfoodpulsedelay) && ((tnowmillis - tfood0) < (nfoodpulsedelay + deliverydur))){
      // Trying to get out of the waiting period
      if (foodttlconditional){
        // Trying to get out of the waiting period (happens when detection or time out (below)
        if (inputttl){
          foodpulses_left = foodpulses;
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

  // Out of the waiting period
  else{
    if (foodpulses_left > 0){
      // Still pulses left
      if (((tnowmillis - tfood1) >= (foodpulse_cycletime)) && !foodttlon){
        // Beginning of each cycle
        tfood1 = tnowmillis;
        foodttlon = true;
        digitalWrite(foodTTLpin, HIGH);
//        digitalWrite(led_pin, HIGH);
        if ((debugmode || serialdebug) && showfoodttl){
          Serial.print("Food pulse on at (ms): ");
          Serial.print(tnowmillis);
        }
      }
      else if (((tnowmillis - tfood1) >= (foodpulse_ontime)) && foodttlon){
        // Turn off
        foodttlon = false;
        digitalWrite(foodTTLpin, LOW);
//        digitalWrite(led_pin, LOW);
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
