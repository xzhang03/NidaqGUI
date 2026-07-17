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

    if (useschedulerindicator > 0){
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

  if (useschedulerindicator > 0){
    shedulerindicator(postoptocolor);
  }
  
  if ((debugmode || serialdebug) && showscheduler){
    Serial.println("Stim is disabled. Entering post-opto phase.");
  }
}


void shedulerindicator(byte color){
  // 3 audiopin (digital), 2 PWMINT, 1 PCA9685
  const uint16_t cscale[8] = {0, 3, 11, 35, 114, 374, 1223, 3998}; // Color scale (log scale, max 4095). This is for external PWM (PCA9685).
  const uint8_t cscale2[8] = {0, 4, 8, 16, 32, 64, 128, 255}; // Color scale (log scale, max 255). This is for internal RGB PWM.

  // Turn off
  if (color == 0){
    // Color input = 0 means turn off everything
    if (useschedulerindicator == 1){
      #if usePCA9685
        pwm.setPin(0, 0, false);
        pwm.setPin(1, 0, false);
        pwm.setPin(2, 0, false);
      #endif
    }
    else if (useschedulerindicator == 2){
      pinMode(extrapins[1], OUTPUT);
      digitalWrite(extrapins[1], LOW);
      pinMode(extrapins[2], OUTPUT);
      digitalWrite(extrapins[2], LOW);
      pinMode(extrapins[3], OUTPUT);
      digitalWrite(extrapins[3], LOW);
    }
    else if (useschedulerindicator == 3){
      digitalWrite(audiopin, LOW);
    }
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
    Serial.print("Scheduler indicator mode: ")
    Serial.println(useschedulerindicator);
    Serial.print("Rv: ");
    Serial.print(Rv);
    Serial.print(" ");
    Serial.print(cscale[Rv]);
    Serial.print(" | ");
    Serial.println(cscale2[Rv]);
    Serial.print("Gv: ");
    Serial.print(Gv);
    Serial.print(" ");
    Serial.print(cscale[Gv]);
    Serial.print(" | ");
    Serial.println(cscale2[Gv]);
    Serial.print("Bv: ");
    Serial.print(Bv);
    Serial.print(" ");
    Serial.print(cscale[Bv]);
    Serial.print(" | ");
    Serial.println(cscale2[Bv]);
  #endif

  // Write color
  if (useschedulerindicator == 1){
    #if usePCA9685
      pwm.setPin(0, cscale[Rv], false);
      pwm.setPin(1, cscale[Gv], false);
      pwm.setPin(2, cscale[Bv], false);
    #endif
  }
  else if (useschedulerindicator == 2){
    analogWrite(extrapins[1], cscale2[Rv]);
    analogWrite(extrapins[2], cscale2[Gv]);
    analogWrite(extrapins[3], cscale2[Bv]);
  }
  else if (useschedulerindicator == 3){
    digitalWrite(audiopin, HIGH);
  }
}
