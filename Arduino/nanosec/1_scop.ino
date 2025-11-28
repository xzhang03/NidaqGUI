// same-color optophotometry functions go here

// Scoptophotometry overflow
void scoptophoto_ov(void){
  turn_ch1_off();

  if (lastpulseopto){
    lastpulseopto = false;

    // Return to normal pulse width after the train is off. Only at last pulse
    pulsewidth_1 = pulsewidth_1_tcp;
    digitalWrite(AOpin, LOW);
    digitalWrite(tristatepin, !tristatepinpol); // !false = active low = off

    // Scheduler disable
    if ((itrain >= ntrain) && stimenabled && inopto && schedulerrunning){
      schedulerdisable();
    }
  }
}

// Scoptophotometry P1: About to turn on Ch1 for photometry or stim 
// (may not turn on for opto if RNG or skipping a pulse)
void scoptophoto_p1(void){
  // Advance counters
  // Scheduler allows train and opto counters to advance
  if (stimenabled){
    counter_for_train_complement = sctrain_cycle - counter_for_train; // Counting 1499-0. 0 when train starts
    pmt_counter_for_opto++; // Counting 1-5, on 1 opto is on
    counter_for_train++; // Counting 1-1500, on 1 train is on
  }
  
  if ((debugmode || serialdebug) && showpulses){
    Serial.print("Photometry ");
    Serial.print(counter_for_train);
    Serial.print(" - ");
    Serial.println(pmt_counter_for_opto);
  }

  // Beginning of each train cycle. Turn train mode on.
  // Only if stim is enabled though
  if ((counter_for_train == 1) && stimenabled){
    // Debug train on to train on
//        Serial.println(tnow - ttest1);s
//        ttest1 = tnow;
    
    train = true;
    opto_counter = 0;
            
    if (usescheduler){
      // In scheduler and current train is on
      // This affects scopto
      if (useRNG){
        // Figure out if current train passes
        trainpass = rngvec[itrain] < threshrng;
      }
      if (usefoodRNG){
        // Food RNG
        foodpass = rngvec_food[itrain] < foodRNGpassthresh;
      }
      
      if (randomiti){
        // Get randomized ITI. This affects scopto.
        sctrain_cycle = rngvec_ITI[itrain] * fps;
      }

      itrain_dtt = itrain; // Advance for deterministic trailtypes
      itrain++; // Advance train count

      if ((debugmode || serialdebug) && showscheduler){
        Serial.print("Scheduler train #");
        Serial.print(itrain);
        Serial.print("/");
        Serial.println(ntrain);

        if (useRNG){
          Serial.print("RNG says train pass = ");
          Serial.println(trainpass);
        }
        if (usefoodRNG){
          Serial.print("Food RNG says = ");
          Serial.println(foodpass);
        }
        if (randomiti){
          Serial.print("RNG says next cycle is (s): ");
          Serial.println(sctrain_cycle / fps);
        }
      }
    }

    // RNG (only in scheduler mode and useRNG is on can RNG be considered)
    if ((!usescheduler) || (!useRNG) || (trainpass)){
      // schedule mode is off or not using RNG or train passed anyway
      // In other words, this step is skipped when schedulemode = 1 && useRNG = 1 && and trainpass = 0
      // If train pass we set analog output high
      pulsewidth_1 = pulsewidth_1_scopto; // Change pulse width
      digitalWrite(AOpin, HIGH);
      digitalWrite(tristatepin, tristatepinpol); // false = active low = on.
//          Serial.println("OPTO actually ON");
    }
    
    if (usefoodpulses && optothenfood && pulsing){
      // Only applies to opto-then-food here
      armfoodttl(); // Arm food ttl
    }
    
    if ((debugmode || serialdebug) && showopto){
      Serial.println("Train on");
      Serial.println("Analog on");
      Serial.println("Transceiver on");
      Serial.println("Ch1 pulse width set to scopto");
    }
  }

  // Turn on opto mode
  if (train){
    if (pmt_counter_for_opto==1){
      // Debug opto on to opto on
//          Serial.println(tnow - ttest1);
//          ttest1 = tnow;
      
      opto = true;
    }
  }
  else {
    // When no train just regular photometry
    turn_ch1_on();
  }

  // Turn on light
  if (opto){
//        Serial.println("OPTO actually ON");
    turn_ch1_on();
    opto_counter++;
    opto = false;

    // debug ch1 on to ch1 off for opto
    if ((debugmode || serialdebug) && showopto){
      ttest1 = tnow;
      ftest1 = true;
    }
  }

  // Reset counter
  // reset pmt counter
  if (pmt_counter_for_opto >= scopto_per){
    pmt_counter_for_opto = 0;
  }
  
  // reset opto counter
  if ((opto_counter >= sctrain_length) && train){
    train = false;
    lastpulseopto = true;
  }

  // Serial.println(counter_for_train);
  // reset train counter
  if (counter_for_train >= sctrain_cycle){
    counter_for_train = 0;
  }
}

// Scoptophotometry P2: Just turned off Ch1, about to adjust if this is the last pulse of train
void scoptophoto_p2(void){
  if (lastpulseopto){
    lastpulseopto = false;

    // Return to normal pulse width after the train is off. Only at last pulse
    pulsewidth_1 = pulsewidth_1_tcp;
    digitalWrite(AOpin, LOW);
    digitalWrite(tristatepin, !tristatepinpol); // !false = active low = off

    if ((debugmode || serialdebug) && showopto){
      Serial.println("Train off");
    }
    
    if ((debugmode || serialdebug) && showopto){
      Serial.println("Ch1 pulse width returned to tcp");
      Serial.println("Analog off");
      Serial.println("Transceiver off");
    }
    
    // Scheduler disable
    if ((itrain >= ntrain) && stimenabled && inopto && schedulerrunning){
      schedulerdisable();
    }   
  }
}
