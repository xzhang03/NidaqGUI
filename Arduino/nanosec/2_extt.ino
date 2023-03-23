// External TTL functions go here
// External TTL trigger
// External TTL trigger does not work with multiple-trial types right now as it doesn't advance itrain

void externalttltrig(void){
  if ((digitalRead(switchpin) == listenpol) && !manualon){
    stimenabled = true;
    inpreopto = false;
    inpostopto = false;
    inopto = true;
    itrain = 0; // Does not advance train
    ntrain = 1; // If manually triggered, it's only gonna be 1 train at a time
    pmt_counter_for_opto = 0;
    opto_counter = 0;
    counter_for_train = 0; // Number of cycles
    foodpulses_left = 0;
    inputttl = !foodttlconditional;
    foodttlwait = false;
    manualon = true;

    if (usecue){
      foodttlcuewait = true;
    }
    
    if (foodttlconditional){
      foodttlactionwait = true;
    }

    if (debugpins){
      digitalWrite(preoptopin, LOW);
      digitalWrite(inoptopin, HIGH);
      digitalWrite(postoptopin, LOW);
    }
    
    if ((debugmode || serialdebug) && showscheduler){
      Serial.println("External pulse detected detected.");
    }
  }
  if (manualon){
    if (digitalRead(switchpin) == !listenpol){
      manualon = false;

      if ((debugmode || serialdebug) && showscheduler){
        Serial.println("External pulse detected ended.");
      }
    }
  }
}
