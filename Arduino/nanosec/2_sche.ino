// Scheduler functions go here

// Initialize scheduler flags
void sche_ini(void){
  // If scheduler is used. disable stims
  stimenabled = !usescheduler;
  schedulerrunning = false;
  inpreopto = true;
  inopto = false;
  inpostopto = false;
}

// Doing the preopto period and the transition to inopto
void sche_preopto(void){
  if ((ipreoptopulse >= npreoptopulse) && (!stimenabled) && inpreopto && schedulerrunning){
    stimenabled = true;
    inpreopto = false;
    inopto = true;

    if (debugpins){
      digitalWrite(preoptopin, LOW);
      digitalWrite(inoptopin, HIGH);
      digitalWrite(postoptopin, LOW);
    }
    
    if ((debugmode || serialdebug) && showscheduler){
      Serial.println("Stim is enabled. Entering opto phase.");
    }
  }
  // Just advance
  else if (schedulerrunning && inpreopto && (!stimenabled) && (!listenmode)){
    ipreoptopulse++;

    // For food -> opto experiments, counting down to when the first foodttlarm comes out (first trains starts at 0);
    counter_for_train_complement = npreoptopulse - ipreoptopulse;
    
    if ((debugmode || serialdebug) && showscheduler){
      if ((ipreoptopulse % 50) == 0){
          Serial.print("Preopto pulse #");
          Serial.print(ipreoptopulse);
          Serial.print("/");
          Serial.println(npreoptopulse);
      }
    }
  }
}


// Scheduler disable
void schedulerdisable(void){
  stimenabled = false;
  inopto = false;
  inpostopto = true;

  if (debugpins){
    digitalWrite(preoptopin, LOW);
    digitalWrite(inoptopin, LOW);
    digitalWrite(postoptopin, HIGH);
  }
  
  if ((debugmode || serialdebug) && showscheduler){
    Serial.println("Stim is disabled. Entering post-opto phase.");
  }
}
