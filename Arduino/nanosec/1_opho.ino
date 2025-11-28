// optophotometry functions goes here

// Optophotometry overflow
void optophoto_ov(void){
  turn_ch2_off();

  if ((opto_counter >= train_length) && optophotommode && train){
    // reset opto counter
    train = false;
    
    if ((debugmode || serialdebug) && showopto){
      Serial.println("Train off");
    }
  }
  
  // Scheduler disable
  if ((itrain >= ntrain) && stimenabled && inopto && schedulerrunning){
    schedulerdisable();
  }
}

// Optophotometry P1: Just turned on ch1, about to decide what to do with ch2
void optophoto_p1(void){
  // Advance counters (not using scheduler or stim is enabled)
  if (stimenabled){
    counter_for_train_complement = train_cycle - counter_for_train; // Counting 1499-0. 0 when train starts
    pmt_counter_for_opto++; // Counting 1-5, on 1 opto is on
    counter_for_train++; // Counting 1-1500, on 1 train is on
  }
  
  if ((debugmode || serialdebug) && showpulses){
    Serial.print("Photometry ");
    Serial.print(counter_for_train);
    Serial.print(" - ");
    Serial.println(pmt_counter_for_opto);
  }

  // Beginning of each train cycle. Turn train mode on (if stim is enabled).
  if ((counter_for_train == 1) && stimenabled){
    // Debug train on to train on
//        Serial.println(tnow - ttest1);
//        ttest1 = tnow;
    
    train = true;
    opto_counter = 0;

    if (usescheduler){
      // In scheduler and RNG mode is on
      // This affects optophotometry only
      if (useRNG){
        // Food RNG
        trainpass = rngvec[itrain] < threshrng;
      }
      if (usefoodRNG){
        foodpass = rngvec_food[itrain] < foodRNGpassthresh;
      }

      if (randomiti){
        // Get randomized ITI. This affects scopto.
        train_cycle = rngvec_ITI[itrain] * fps;
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
          Serial.println(train_cycle / fps);
        }
      }
    }

    if (usefoodpulses && optothenfood && pulsing){
      // Only applies to opto-then-food here
      armfoodttl(); // Arm food ttl
    }
    
    if ((debugmode || serialdebug) && showopto){
      Serial.println("Train on");
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

  // reset pmt counter
  if (pmt_counter_for_opto >= opto_per){
    pmt_counter_for_opto = 0;
  }
  
  // Serial.println(counter_for_train);
  // reset train counter
  if (counter_for_train >= train_cycle){
    counter_for_train = 0;
  }

  // Overlap mode (sending opto pulse now)
  if ((optophotooverlap) && (opto) && (!ch2_on)){
    // RNG (only in scheduler mode and useRNG is on can RNG be considered)
    if ((!usescheduler) || (!useRNG) || (trainpass)){
      // schedule mode is off or not using RNG or train passed anyway
      // In other words, this step is skipped when schedulemode = 1 && useRNG = 1 && and trainpass = 0
      turn_ch2_on();
    }
    
    opto_counter++;
    opto = false;
    if ((debugmode || serialdebug) && showopto){
      ttest1 = tnow;
      ftest1 = true;
    }
  }
}

// Optophotometry P3: Decide to turn on Ch2 or not
// Won't turn on if overlapmode (i.e., turned on earlier)
void optophoto_p3(void){
  if (!optophotooverlap){
    // optophotometry and not-overlap mode (overlapping means the opto pulse is sent earlier with photometry pulse)
    if ((opto) && (!ch2_on)){
      // RNG (only in scheduler mode and useRNG is on can RNG be considered)
      if ((!usescheduler) || (!useRNG) || (trainpass)){
        // schedule mode is off or not using RNG or train passed anyway
        // In other words, this step is skipped when schedulemode = 1 && useRNG = 1 && and trainpass = 0
        digitalWrite(ch2_pin, HIGH);
//          Serial.println("OPTO actually ON");
      }
      ch2_on = true;
      opto_counter++;
      opto = false;
      if ((debugmode || serialdebug) && showopto){
        ttest1 = tnow;
        ftest1 = true;
      }
    }
  }
}

// Optophotometry P4: Just turned off Ch2, deciding whether this is the last train
void optophoto_p4(void){
  if ((opto_counter >= train_length) && train){
        
    // reset opto counter
    train = false;
    
    if ((debugmode || serialdebug) && showopto){
      Serial.println("Train off");
    }

    // Scheduler disable
    if ((itrain >= ntrain) && stimenabled && inopto && schedulerrunning){
      schedulerdisable();
    }  
  } 
}
