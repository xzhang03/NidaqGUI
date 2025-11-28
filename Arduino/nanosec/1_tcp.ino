// Two-color photometry functions goes here

// TCP P1: Just turned on ch1, about to decide what to do about behavior
void tcp_p1(void){
  // Advance counters
  // Scheduler allows train and opto counters to advance
  if (stimenabled){
    counter_for_train_complement = tcptrain_cycle - counter_for_train; // Counting 1499-0. 0 when train starts
    counter_for_train++; // Counting 1-1500, on 1 food task is on
  }
  
  if ((debugmode || serialdebug) && showpulses){
    Serial.print("Photometry ");
    Serial.println(counter_for_train);
  }

  if ((counter_for_train == 1) && stimenabled){
    // Once per train
    if (usefoodpulses && optothenfood && pulsing){
      // Only applies to opto-then-food here
      itrain_dtt = itrain; // Advance for deterministic trailtypes
      armfoodttl(); // Arm food ttl
    }

    if (schedulerrunning){
      if (usefoodRNG){
        // Food RNG
        foodpass = rngvec_food[itrain] < foodRNGpassthresh;
      }
      if (randomiti){
        // Get randomized ITI. This affects scopto.
        tcptrain_cycle = rngvec_ITI[itrain] * fps;
      }
      
      itrain++; // Advance train count

      if ((debugmode || serialdebug) && showscheduler){
        Serial.print("Scheduler train #");
        Serial.print(itrain);
        Serial.print("/");
        Serial.println(ntrain);
        if (usefoodRNG){
          Serial.print("Food RNG says = ");
          Serial.println(foodpass);
        }
        if (randomiti){
          Serial.print("RNG says next cycle is (s): ");
          Serial.println(tcptrain_cycle / fps);
        }
      }
    }
  }

  if (counter_for_train >= tcptrain_cycle){
    counter_for_train = 0;
  }

  // Scheduler disable
  if ((itrain >= ntrain) && stimenabled && inopto && schedulerrunning){
    schedulerdisable();
  }
}
