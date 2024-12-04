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

    #if (debugpins)
      digitalWrite(preoptopin, LOW);
      digitalWrite(inoptopin, HIGH);
      digitalWrite(postoptopin, LOW);
    #endif

    if (useschedulerindicator){
      shedulerindicator(inoptocolor);
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

  #if (debugpins)
    digitalWrite(preoptopin, LOW);
    digitalWrite(inoptopin, LOW);
    digitalWrite(postoptopin, HIGH);
  #endif

  if (useschedulerindicator){
    shedulerindicator(postoptocolor);
  }
  
  if ((debugmode || serialdebug) && showscheduler){
    Serial.println("Stim is disabled. Entering post-opto phase.");
  }
}

void shedulerindicator(byte color){
  const uint16_t cscale[8] = {0, 3, 11, 35, 114, 374, 1223, 3998}; // Color scale (log scale, is 4096)

  // Turn off
  if (color == 0){
    // Color input = 0 means turn off everything
    pwm.setPin(0, 0, false);
    pwm.setPin(1, 0, false);
    pwm.setPin(2, 0, false);
    return;
  }

  // Turn on
  // Get color
  byte Rv = ((color >> 4) & 0b11);
  if (Rv > 0){
    Rv = Rv+ (bitRead(color, 6) << 2); // Red value
  }
  
  byte Gv = ((color >> 2) & 0b11);
  if (Gv > 0){
    Gv = Gv + (bitRead(color, 6) << 2); // Green value
  }
  
  byte Bv = ((color) & 0b11);
  if (Bv > 0){
    Bv = Bv + (bitRead(color, 6) << 2); // Green value;
  }

  #if serialdebug
    Serial.print("Rv: ");
    Serial.print(Rv);
    Serial.print(" ");
    Serial.println(cscale[Rv]);
    Serial.print("Gv: ");
    Serial.print(Gv);
    Serial.print(" ");
    Serial.println(cscale[Gv]);
    Serial.print("Bv: ");
    Serial.print(Bv);
    Serial.print(" ");
    Serial.println(cscale[Bv]);
  #endif

  // Write color
  pwm.setPin(0, cscale[Rv], false);
  pwm.setPin(1, cscale[Gv], false);
  pwm.setPin(2, cscale[Bv], false);
}
