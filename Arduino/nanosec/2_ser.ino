// Serial functions go here

// Parse serial
void parseserial(){
  // Read 2 bytes
  m = Serial.read();
  n = Serial.read();
    
  #if (debugmode)
    m = m - '0';
    n = n - '0';
    Serial.print("Inputs: ");
    Serial.print(m);
    Serial.print(" ");
    Serial.println(n);

    if (Serial.available() > 0){
      Serial.read();
    }
  #endif
  
  switch (m){
    // Initialize echo
    
    case 255:
      // 255: status update (n = variable)
      echo[0] = 0;
      echo[1] = 0;
      echo[2] = 0;
      echo[3] = 0;
    
      switch (n){
        case 0:
          // Echo back scheduler info
          echo[3] = 1;
          
          // Scheduler
          if (!usescheduler){
            echo[2] = 1; // Echo back [0 0 1 1] as no scheduler
          }
          else if (inpreopto){
            echo[2] = 2; // Echo back [0 0 2 1] as preopto
          }
          else if (inpostopto){
            echo[2] = 3;; // Echo back [0 0 3 1] as postopto
          }
          else {
            echo[0] = itrain;
            echo[1] = ntrain; // first byte shows n train, second byte shows itrain
          }
          break;
  
        case 38:
          // Echo back rng info
          echo[3] = 2;
          echo[2] = useRNG; // Echo back [X X 0 2] as no RNG
          echo[1] = usescheduler; // Echo back [X 0 X 2] as no RNG
          echo[0] = trainpass;
          break;
  
        case 39:
          // Echo back pass chance
          echo[3] = 3;
          echo[0] = threshrng;
          break;
       }
       Serial.write(echo, 4);
       break;
    
    case 254:
      Serial.print(nsver);
      #if TeensyTester // Tester - T, PCB - P, hand soldered - H
        Serial.print("T");
      #elif PCB
        Serial.print("P");
      #else
        Serial.print("H");
      #endif
      
      #if debugmode // Debug
        Serial.print("D");
      #elif serialdebug
        Serial.print("S");
      #else
        Serial.print("A");
      #endif

      Serial.print(F_CPU_ACTUAL/1000000);
      Serial.print(" ");
      Serial.print(__DATE__);
      Serial.print(" ");
      Serial.print(__TIME__);
      Serial.print(".");
      
      break;
     
    case 253:
      // Reboot
      if (n == 104){
        SCB_AIRCR = 0x05FA0004;
      }
      break;

    case 252:
      // 252: Universal identifier
      Serial.print("I'm nanosec.");
      break;
    
    case 0:
      // End pulsing
      pulsing = false;
      train = false;

      if (samecoloroptomode){
        // return analog pin to high impedence and reset pulse width
        pulsewidth_1 = pulsewidth_1_tcp;
        digitalWrite(AOpin, LOW);
        digitalWrite(tristatepin, !tristatepinpol); // !false = active low = off
      }
      
      if (echoencoderc){
        Serial.write((byte *) &pcount, 4);
      }
      if (usescheduler){
        stimenabled = false;
        schedulerrunning = false;

        if (useschedulerindicator){
          // What to do with indicator when the run is stopped
          // 0: off, 1: stay, 2: preopto, 3: inopto, 4: postopto
          switch (switchoff_indicator){
            case 0:
              shedulerindicator(0);
              break;
            case 1:
              break;
            case 2:
              shedulerindicator(preoptocolor);
              break;
            case 3:
              shedulerindicator(inoptocolor);
              break;
            case 4:
              shedulerindicator(postoptocolor);
              break;
          }
        }
        
      }
      if (usecue){
        noTone(audiopin);
      }
      #if perfcheck
        // Tag
        echo[0] = 128;
        echo[1] = 128;
        echo[2] = 128;
        echo[3] = 128;
        Serial.write(echo, 4);
        Serial.write((byte *) &ix_pc, 4);
        Serial.write((byte *) &tpc, 4);
      #endif
      
      if (debugmode){
        Serial.println("Cam pulse stop.");
      }
      break;
      
    case 1:
      // Start pulsing
      pulsing = true;
      pulsetime = tnow;
      echotime_slow = tnowmillis;

      // Train/opto related variables (relavant with or without scheduler)
      pmt_counter_for_opto = 0;
      opto_counter = 0;
      counter_for_train = 0; // Number of cycles

      // Food TTL related variables (relavant with or without scheduler)
      foodpulses_left = 0;
      inputttl = !foodttlconditional;
      foodttlarmed = false;
      foodttlwait = false;
      foodttlon = false;

      // Multiple trial types
      if (usefoodpulses){
        rng(rngvec_trialtype, freq_cumvec[ntrialtypes-1] , 0, maxrngind);
      }

      if (foodttlconditional){
        foodttlcuewait = false;
        foodttlactionwait = false;
        cueon = false;
        actionperiodon = false;
      }
      
      if (debugmode){
        Serial.println("Cam pulse start.");
      }
      if (useencoder){
        myEnc.write(0);    // zero the position
        pos = 0;
        pcount = 0;  
      }

      // Scheduler reset
      if (usescheduler){
        ipreoptopulse = 0;
        itrain = 0;
        inpreopto = true;
        inopto = false;
        inpostopto = false;
        stimenabled = false;
        schedulerrunning = true;
        counter_for_train_complement = npreoptopulse;
        
        // Opto RNG
        if (useRNG){
          // Generate array
          rng(rngvec, maxrng, 0, maxrngind);
        }

        // RNG ITI
        if (randomiti){
          // Generate array
          rng(rngvec_ITI, rng_cycle_max , rng_cycle_min, maxrngind);
        }
        
        #if (debugpins)
          digitalWrite(schedulerpin, HIGH);
          digitalWrite(preoptopin, HIGH);
          digitalWrite(inoptopin, LOW);
          digitalWrite(postoptopin, LOW);
        #endif

        if (useschedulerindicator){
          shedulerindicator(preoptocolor);
        }
      }
      else{
        stimenabled = true;
        inpreopto = false;

        // Complement counter (no scheduler)
        if (tcpmode){
          counter_for_train_complement = tcptrain_cycle;
        }
        else if (samecoloroptomode){
          counter_for_train_complement = sctrain_cycle;
        }
        else if(optophotommode){
          counter_for_train_complement = train_cycle;
        }
        
        
        #if (debugpins)
          digitalWrite(schedulerpin, LOW);
          digitalWrite(preoptopin, LOW);
          digitalWrite(inoptopin, LOW);
          digitalWrite(postoptopin, LOW);
        #endif    

        if (useschedulerindicator){
          shedulerindicator(0);
        }
      }
      break;

    case 2:
      // Set frequency
      freq = n;
      cycletime = 1000000 / n;

      if (debugmode){
        Serial.print("New cam frequency (Hz): ");
        Serial.println(n);
        Serial.print("New cam cycle time (ms): ");
        Serial.println(cycletime/1000);
      }
      break;

    case 3:
      // tcp mode
      if (n == 0){
        // tcp
        optophotommode = false;
        tcpmode = true;
        samecoloroptomode = false;
      }
      else if (n == 1){
        // optophotometry
        optophotommode = true;
        tcpmode = false;
        samecoloroptomode = false;
      }
      else if (n == 2){
        // same color optophotometry
        optophotommode = false;
        tcpmode = false;
        samecoloroptomode = true;
      }
      modeswitch();
      break;

    case 4:
      // 4: Set delay (delay_cycle = n * 50, n = 1 means 1 seconds). Only relevant when scheduler is on
      preoptotime = n; // in seconds (max is high because npreoptopulse is unsigned int which is 32 bits)
      npreoptopulse = preoptotime * fps; // preopto pulse number
      ipreoptopulse = 0;
      itrain = 0;
      break;
        
    case 5:
      // Give position
      if (useencoder){
        pos = myEnc.read();
        //      Serial.println(pos);      
        Serial.write((byte *) &pos, 4);
      }
      else{
        pos = 0;
        //      Serial.println(pos);      
        Serial.write((byte *) &pos, 4);
      }
      break;
      
    case 6:
      // 6: Change opto frequency (opto_per = n)
      opto_per = n;

      if (debugmode){
        Serial.print("New opto freq (Hz): ");
        Serial.println(fps / opto_per);
      }
      break;
      
    case 7:
      // 7: Change opto pulse per train (train_length = n)
      train_length = n;
      if (debugmode){
        Serial.print("New opto pulses per train: ");
        Serial.println(train_length);
      }
      break;
      
    case 8:
      // 8: Change opto train cycle (train_cycle = n * 50)
      train_cycle = n * fps;
      if (debugmode){
        Serial.print("New opto train cycle (s): ");
        Serial.println(train_cycle / fps);
      }
      break;

    case 9:
      // List parameters
      showpara();
      break;

    case 10:
      // 10: Change scopto frequency (scopto_per = n, f = 50/n)
      scopto_per = n;

      if (debugmode){
        Serial.print("New scopto freq (Hz): ");
        Serial.println(fps / opto_per);
      }
      break;

    case 11:
      // 11: Change scopto train cycle (train_cycle = n * 50)
      sctrain_cycle = n * fps;
      if (debugmode){
        Serial.print("New scopto train cycle (s): ");
        Serial.println(sctrain_cycle / fps);
      }
      break;

    case 12:
      // 12: Change scopto pulse per train (train_length = n)
      sctrain_length  = n;
      if (debugmode){
        Serial.print("New scopto pulses per train: ");
        Serial.println(sctrain_length);
      }
      break;

    case 13:
      // 13: Change scopto pulse width (n * 1000)
      pulsewidth_1_scopto = n * 1000;
      if (debugmode){
        Serial.print("New scopto pulse width (us): ");
        Serial.println(n * 1000);
      }
      break;

    case 14:
      // 14: Opto pulse width (n * 1000 us)
      pulsewidth_2_opto = n * 1000;
      if (debugmode){
        Serial.print("New opto pulse width (us): ");
        Serial.println(pulsewidth_2_opto);
      }
      break;

    case 15:
      // 15: Use scheduler (n = 1 yes, 0 no) 
      usescheduler = (n == 1);
      
      // Reset
      if (usescheduler){
        stimenabled = false;

        #if (debugpins)
          digitalWrite(schedulerpin, HIGH);
          digitalWrite(preoptopin, HIGH);
          digitalWrite(inoptopin, LOW);
          digitalWrite(postoptopin, LOW);
        #endif

        if (useschedulerindicator){
          // What to do with indicator when the run is stopped
          // 0: off, 1: stay, 2: preopto, 3: inopto, 4: postopto
          switch (switchoff_indicator){
            case 0:
              shedulerindicator(0);
              break;
            case 1:
              break;
            case 2:
              shedulerindicator(preoptocolor);
              break;
            case 3:
              shedulerindicator(inoptocolor);
              break;
            case 4:
              shedulerindicator(postoptocolor);
              break;
          }
        }
      }
      else{
        stimenabled = true;
        #if (debugpins)
          digitalWrite(schedulerpin, LOW);
          digitalWrite(preoptopin, LOW);
          digitalWrite(inoptopin, LOW);
          digitalWrite(postoptopin, LOW);
        #endif

        if (useschedulerindicator){
          shedulerindicator(0);
        }
      }
      schedulerrunning = false;

      if (debugmode){
        Serial.print("Use scheduler: ");
        Serial.println(stimenabled);
      }
      break;

    case 16:
      // 16: Number of trains (n * 10 of trains)
      ntrain = n;
      itrain = 0;
      if (debugmode){
        Serial.print("Number of trains: ");
        Serial.println(ntrain);
      }
      break;

    case 17:
      // 17: Enable manual scheduler override (n = 1 yes, 0 no) 
      manualscheduleoverride = (n == 1);
      if (debugmode){
        Serial.print("Enable manual scheduler override: ");
        Serial.println(manualscheduleoverride);
      }
      break;

    case 18:
      // 18: Delay time after opto (n * 100 ms)
      nfoodpulsedelay = n * 100;
      if (debugmode){
        Serial.print("Delay time after opto (ms): ");
        Serial.println(nfoodpulsedelay);
      }
      break;

    case 19:
      // Pulse on time (n * 10 ms)
      foodpulse_ontime_vec[trialtype_edit] = n * 10;
      if (debugmode){
        Serial.print("Food pulse on time (ms): ");
        Serial.println(foodpulse_ontime_vec[trialtype_edit]);
      }
      break;

    case 20: 
      // 20: Pulse cycle time (n * 10 ms)
      foodpulse_cycletime_vec[trialtype_edit] = n * 10;
      if (debugmode){
        Serial.print("Food pulse cycle time (ms): ");
        Serial.println(foodpulse_cycletime_vec[trialtype_edit]);
      }
      break;

    case 21:
      // 21: Pulses per train (n)
      foodpulses_vec[trialtype_edit] = n;
      foodpulses_left = 0;
      if (debugmode){
        Serial.print("Pulses per train: ");
        Serial.println(foodpulses_vec[trialtype_edit]);
      }
      break;

    case 22:
      // 22: Conditional or not (n = 1 yes, 0 no) 
      foodttlconditional_vec[trialtype_edit] = (n == 1);
      foodttlconditional = (n == 1);
      foodttlwait = false;
      if (foodttlconditional){
        foodttlcuewait = false;
        foodttlactionwait = false;
        cueon = false;
        actionperiodon = false;
      }
      
      if (debugmode){
        Serial.print("Food pulses conditional or not: ");
        Serial.println(foodttlconditional);
      }
      break;

    case 23:
      // 23 encoder useage (n = 1 yes, 0 no) 
      useencoder = (n == 1);
      if (debugmode){
        Serial.print("Use encoder or not: ");
        Serial.println(useencoder);
      }
      break;

    case 24:
      // 24: Use Food TTL or not (n = 1 yes, 0 no) 
      usefoodpulses = (n == 1);
      if (debugmode){
        Serial.print("Food TTL used: ");
        Serial.println(usefoodpulses);
      }
      break;

    case 25:
      // 25: Audio sync (n = 1 yes, 0 no);
      syncaudio = (n == 1);
      if (debugmode){
        Serial.print("Audio sync or not: ");
        Serial.println(syncaudio);
      }
      break;

    case 26:
      // 26: audio sync tone frequency (n * 100 Hz)
      audiofreq = n * 100;
      if (debugmode){
        Serial.print("Audio sync tone frequency: ");
        Serial.println(audiofreq);
      }
      break;

    case 27:
      // 27: Listening mode on or off (n = 1 yes, 0 no)
      listenmode = (n == 1);
      if (listenmode){
        manualscheduleoverride = true;
      }
      if (debugmode){
        Serial.print("Listening mode on or not: ");
        Serial.println(listenmode);
        if (debugmode){
          Serial.println("Manual over-ride on.");
        }
      }
      break;

    case 28:
      // 28: Listen mode polarity (1 = active high, 0 = active low)
      listenpol = (n == 1); // Polarity for listen mode false = active low, true = active high. Either way, it has to be 3.3V logic
      if (listenpol){
        pinMode(switchpin, INPUT_PULLDOWN); // active high
      }
      else {
        pinMode(switchpin, INPUT_PULLUP); // active low
      }
      if (debugmode){
        Serial.print("Input opto trigger polarity (1 = active high, 0 = active low): ");
        Serial.println(listenpol);
      }
      break;

    case 29:
      // 29: Tristate pin polarity (1 = active high, 0 = active low);
      tristatepinpol = (n == 1);
      if (debugmode){
        Serial.print("Tristate pin polarity (1 = active high, 0 = active low): ");
        Serial.println(tristatepinpol);
      }
      
      if (optophotommode || tcpmode){
        digitalWrite(tristatepin, tristatepinpol); // Let it go through
        if (debugmode){
          Serial.print("Auto switched to output = ");
          Serial.println(tristatepinpol);
        }
      }
      else if (samecoloroptomode){
        digitalWrite(tristatepin, !tristatepinpol); // Do not let it go through
        if (debugmode){
          Serial.print("Auto switched to output = ");
          Serial.println(!tristatepinpol);
        }
      }
      break;

    case 30:
      // 30: Use cue or not for food (n = 1 yes, 0 no)
      usecue = (n == 1);
      if (!usecue){
        noTone(audiopin);
      }
      if (debugmode){
        Serial.print("Use cue (1 = yes, 0 = no): ");
        Serial.println(usecue);
      }
      break;

    case 31:
      // 31: Cue delay (n * 100 ms)
      cuedelay = n * 100;
      if (debugmode){
        Serial.print("New cue delay (ms): ");
        Serial.println(cuedelay);
      }
      break;

    case 32:
      // 32: Cue duration (n * 100 ms)
      cuedur_vec[trialtype_edit] = n * 100;
      if (debugmode){
        Serial.print("New cue duration (ms): ");
        Serial.println(cuedur_vec[trialtype_edit]);
      }
      break;

    case 33:
      // Action period delay (n * 100 ms)
      actiondelay = n * 100;
      if (debugmode){
        Serial.print("New action period delay (ms): ");
        Serial.println(actiondelay);
      }
      break;

    case 34:
      // Action period delay (n * 100 ms)
      actiondur = n * 100;
      if (debugmode){
        Serial.print("New action period duration (ms): ");
        Serial.println(actiondur);
      }
      break;

    case 35:
      // 35: Delivery period duration (n * 100 ms);
      deliverydur_vec[trialtype_edit] = n * 100;
      if (debugmode){
        Serial.print("New delivery period duration (ms): ");
        Serial.println(deliverydur_vec[trialtype_edit]);
      }
      break;

    case 36:
      // 36: Cycle 1 for optophotometry (only changed for pure optogenetic experiments; cycle = n * 100 us)
      cycletime_photom_1_opto = n * 100;
      cycletime_photom_1 = cycletime_photom_1_opto; // in micro secs (Ch1)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      if (debugmode){
        Serial.print("New opto cycle 1 (us): ");
        Serial.println(cycletime_photom_1_opto);
      }
      break;

   case 37:
      // 37: Cycle 2 for optophotometry (only changed for pure optogenetic experiments; cycle = n * 100 us)
      cycletime_photom_2_opto = n * 100;
      cycletime_photom_2 = cycletime_photom_2_opto; // in micro secs (Ch1)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      if (debugmode){
        Serial.print("New opto cycle 2 (us): ");
        Serial.println(cycletime_photom_2_opto);
      }
      break;
       
    case 38:
      // 38: use RNG or not (n = 1 yes, 0 no)
      useRNG = (n == 1);

      if (useRNG){
        trainpass = false;
      }
      
      if (debugmode){
        Serial.print("Use RNG (1 = yes, 0 = no): ");
        Serial.println(useRNG);
      }
      break;

    case 39:
      // 39: Pass chance in percent (30 = 30% pass chance)
      threshrng = n;
      if (debugmode){
        Serial.print("New RNG pass chance (%): ");
        Serial.println(threshrng);
      }
      break;

    case 40:
      // 40: RNG ITI or not (n = 1 yes, 0 no)
      randomiti = (n == 1);
      if (debugmode){
        Serial.print("Randomize ITI (1 = yes, 0 = no): ");
        Serial.println(randomiti);
      }
      break;

    case 41:
      // 41: min randomized ITI (n * 1 s)
      rng_cycle_min = n;
      if (debugmode){
        Serial.print("Min randomized ITI (s): ");
        Serial.println(rng_cycle_min);
      }
      break;

    case 42:
      // 42: max randomized ITI (n * 2 s)
      rng_cycle_max = n;
      if (debugmode){
        Serial.print("Max randomized ITI (s): ");
        Serial.println(rng_cycle_max);
      }
      break;

    case 43:
      // 43: auto encoder serial feedback (n = 1 yes, 0 no)
      autoencserial = (n == 1);
      if (debugmode){
        Serial.print("Auto encoder serial communication (1 = yes, 0 = no): ");
        Serial.println(autoencserial);
      }
      break;

    case 44:
      // 44: Auto trial and rng echo (n = 1 yes, 0 no)
      autoechotrialinfo = (n == 1);
      if (debugmode){
        Serial.print("Auto trial and rng info serial communication (1 = yes, 0 = no): ");
        Serial.println(autoechotrialinfo);
      }
      break;

    case 45:
      // 45: Auto trial and rng echo periodicity (n * 100 ms)
      echotime_slow = n * 100;
      if (debugmode){
        Serial.print("Auto trial and rng info serial communication periodicity (ms): ");
        Serial.println(echotime_slow);
      }
      break;

    case 46:
      // 46: Adding trains (trains = trains + n * 256)
      ntrain = ntrain + n * 256;
      itrain = 0;
      if (debugmode){
        Serial.print("Number of trains: ");
        Serial.println(ntrain);
      }
      break;

    case 47:
      // 47: Change tcp behavioral train cycle (train_cycle = n * 50)
      tcptrain_cycle = n * fps;
      if (debugmode){
        Serial.print("New tcp train cycle (s): ");
        Serial.println(tcptrain_cycle / fps);
      }
      break;

    case 48:
      // 48: Opto then Food (n = 1 yes, 0 no)
      optothenfood = (n == 1);
      if (debugmode){
        Serial.print("Opto then Food (1 = yes, 0 = no): ");
        Serial.println(optothenfood);
      }
      break;

    case 49:
      // 49: Complement delay time before opto (n s)
      nfoodpulsedelay_complement = n * fps;
      if (debugmode){
        Serial.print("Complement delay time ebfore opto (s): ");
        Serial.println(nfoodpulsedelay_complement / fps);
      }
      break;

    case 50:
      // 50: Adding delay time after opto (+ n * 256 * 100 ms)
      nfoodpulsedelay = nfoodpulsedelay + n * 256 * 100;
      if (debugmode){
        Serial.print("Delay time after opto (ms): ");
        Serial.println(nfoodpulsedelay);
      }
      break;

    case 51:
      // 51: Adding lead time before opto (+ n * 256 s) 
      nfoodpulsedelay_complement = nfoodpulsedelay_complement + n * 256 * fps;
      if (debugmode){
        Serial.print("Complement delay time ebfore opto (s): ");
        Serial.println(nfoodpulsedelay_complement / fps);
      }
      break;

    case 52:
      // 52: Adding delay (+ n * 256 * 50, n = 1 means 256 second). Only relevant when scheduler is on
      preoptotime = preoptotime + n * 256; // in seconds (max is high because npreoptopulse is unsigned int which is 32 bits)
      npreoptopulse = preoptotime * fps; // preopto pulse number
      ipreoptopulse = 0;
      itrain = 0;
      break;

    case 53:
      // 53: Cycle 1 for TCP (only changed for pure TCP experiments; cycle = n * 100 us)
      cycletime_photom_1_tcp = n * 100;
      cycletime_photom_1 = cycletime_photom_1_tcp; // in micro secs (Ch1)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      
      if (debugmode){
        Serial.print("New tcp cycle 1 (us): ");
        Serial.println(cycletime_photom_1_tcp);
      }
      break;
      
    case 54:
      // 54: Cycle 2 for TCP (only changed for pure TCP experiments; cycle = n * 100 us)
      cycletime_photom_2_tcp = n * 100;
      cycletime_photom_2 = cycletime_photom_2_tcp; // in micro secs (Ch2)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      
      if (debugmode){
        Serial.print("New tcp cycle 2 (us): ");
        Serial.println(cycletime_photom_2_tcp);
      }
      break;

    case 55:
      // 55: Add Cycle 1 for optophotometry (cycle = cycle + n * 256 * 100 us);
      cycletime_photom_1_opto = cycletime_photom_1_opto + n * 100 * 256;
      cycletime_photom_1 = cycletime_photom_1_opto; // in micro secs (Ch1)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      if (debugmode){
        Serial.print("New opto cycle 1 (us): ");
        Serial.println(cycletime_photom_1_opto);
      }
      break;

    case 56:
      // 56: Add Cycle 2 for optophotometry (cycle = cycle + n * 256 * 100 us);
      cycletime_photom_2_opto = cycletime_photom_2_opto + n * 100 * 256;
      cycletime_photom_2 = cycletime_photom_2_opto; // in micro secs (Ch1)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      if (debugmode){
        Serial.print("New opto cycle 2 (us): ");
        Serial.println(cycletime_photom_2_opto);
      }
      break;

    case 57:
      // 57: Add Cycle 1 for TCP (cycle = cycle + n * 256 * 100 us);
      cycletime_photom_1_tcp = cycletime_photom_1_tcp + n * 100 * 256;
      cycletime_photom_1 = cycletime_photom_1_tcp; // in micro secs (Ch1)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      
      if (debugmode){
        Serial.print("New tcp cycle 1 (us): ");
        Serial.println(cycletime_photom_1_tcp);
      }
      break;

    case 58:
      // 58: Add Cycle 2 for TCP (cycle = cycle + n * 256 * 100 us);
      cycletime_photom_2_tcp = cycletime_photom_2_tcp + n * 100 * 256;
      cycletime_photom_2 = cycletime_photom_2_tcp; // in micro secs (Ch2)
      fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
      
      if (debugmode){
        Serial.print("New tcp cycle 2 (us): ");
        Serial.println(cycletime_photom_2_tcp);
      }
      break;

    case 59:
      // 59: Opto-photometry overlap (n = 1 yes, 0 no)
      optophotooverlap = (n == 1);
      if (debugmode){
        Serial.print("Opto-photometry overlap: ");
        Serial.println(optophotooverlap);
      }
      break;

    case 60:
      // 60: Scan i2c addresses [l]
      i2c_scan();
      break;

    case 61:
      // 61: Test PCA9685 [m]
      testPCA9685();
      break;

    case 62:
      // 62: Max trial types (n = 1-4)[n]
      ntrialtypes = n;
      if (!usescheduler){
        // If no scheduler, roll RNG now to avoid beind stuck on type 1
        rng(rngvec_trialtype, freq_cumvec[ntrialtypes-1] , 0, maxrngind);
      }
      break;

    case 63:
      // 63: Current trialtype to edit (n = 0 - 3)[o]
      trialtype_edit = n;
      break;
    
    case 64:
      // 64: Report RNG (n = 0 ITI, 1 opto, 2 trialtype)[p]
      switch (n){
        case 0:
          Serial.write(rngvec, maxrngind);
          break;
        case 1:
          Serial.write(rngvec_ITI, maxrngind);
          break;
        case 2:
          Serial.write(rngvec_trialtype, maxrngind);
          break;
      }
      break;

    case 65:
      // 65: Trial IO upper byte (n = MSB)[q]
      switch (trialtype_edit){
        case 0:
          trialio0 = (n << 8) + (trialio0 & 0xFF);
          break;
        case 1:
          trialio1 = (n << 8) + (trialio1 & 0xFF);
          break;
        case 2:
          trialio2 = (n << 8) + (trialio2 & 0xFF);
          break;
        case 3:
          trialio3 = (n << 8) + (trialio3 & 0xFF);
          break;
      }
      break;
    case 66:
      // 66: Trial IO lower byte (n = LSB)[r]
      switch (trialtype_edit){
        case 0:
          trialio0 = n + (trialio0 & 0xFF00);
          break;
        case 1:
          trialio1 = n + (trialio1 & 0xFF00);
          break;
        case 2:
          trialio2 = n + (trialio2 & 0xFF00);
          break;
        case 3:
          trialio3 = n + (trialio3 & 0xFF00);
          break;
      }
      break;

    case 67:
      // 67: Trial frequency weight (n = weight)[s]
      freq_vec[trialtype_edit] = n;
      freq_cumvec[0] = freq_vec[0];
      freq_cumvec[1] = freq_vec[0] + freq_vec[1];
      freq_cumvec[2] = freq_vec[0] + freq_vec[1] + freq_vec[2];
      freq_cumvec[3] = freq_vec[0] + freq_vec[1] + freq_vec[2] + freq_vec[3];
      if (debugmode){
        Serial.print("Frequency updated to: ");
        Serial.println(n);
      }
      break;

    case 68:
      // 68: Test MCP23008 [t]
      testMCP23008();
      break;

    case 69:
      // 69: Turn on scheduler indicator (PCA9685) (n = 1 yes, 0 no)[u]
      useschedulerindicator = (n == 1);
      break;

    case 70:
      // 70: Preopto PCA9685 color [0 Shared_bit R R G G B B] [v]
      preoptocolor = n;
      break;

    case 71:
      // 71: Inopto PCA9685 color [0 Shared_bit R R G G B B] [w]
      inoptocolor = n;
      break;

    case 72:
      // 72: Postopto PCA9685 color [0 Shared_bit R R G G B B] [x]
      postoptocolor = n;
      break;

    case 73:
      // 73: Stop-recording PCA9685 color (0: off, 1: stay, 2: preopto, 3: inopto, 4: postopto)[y]
      switchoff_indicator = n;
      break;

    case 74:
      // 74: Test food TTLs (150-ms on/150-ms off, n = cycles) [z]
      testfoodttls(n);
      break;

     case 75:
      // 75: Turn on cam pulsing (send any serial to stop) [{]
      pulsing_test = true;
      while (pulsing_test){
        tnow = micros();
        camerapulse_test();
        if (Serial.available() >= 2){
          m = Serial.read();
          n = Serial.read();
          pulsing_test = false;
        }
        delayMicroseconds(100);
      }
      camerapulse_test();
      break;
  }

  #if (debugpins)
    if (!serialpinon){
      serialpinon = true;
      digitalWrite(serialpin, HIGH);
    }
    else{
      serialpinon = false;
      digitalWrite(serialpin, LOW);
    }
  #endif
}

// Show all parameters
void showpara(void){
  Serial.println("============== photometry ==============");
  if (optophotommode){
    Serial.println("Optophotometry mode.");
  }
  else if (tcpmode){
    Serial.println("Two-color photometry mode.");
  }
  else if (samecoloroptomode){
    Serial.println("Same color optophotometry mode.");
  }
  Serial.print("Ch1 (us): ");
  Serial.print(pulsewidth_1);
  Serial.print("/");
  Serial.println(cycletime_photom_1);
  Serial.print("Ch2 (us): ");
  Serial.print(pulsewidth_2);
  Serial.print("/");
  Serial.println(cycletime_photom_2);
  
  // Photometry
  Serial.println("============== TCP ==============");
  Serial.print("Ch1 TCP cycle time (us): ");
  Serial.println(cycletime_photom_1_tcp);
  Serial.print("Ch1 TCP on time (us): ");
  Serial.println(pulsewidth_1_tcp);
  Serial.print("Ch2 TCP cycle time (us): ");
  Serial.println(cycletime_photom_2_tcp);
  Serial.print("Ch2 TCP on time (us): ");
  Serial.println(pulsewidth_2_tcp);
  Serial.print("TCP behavior train cycle (s): ");
  Serial.println(tcptrain_cycle / fps);
  
  // Opto
  Serial.println("============== Optophoto ==============");
  Serial.print("Opto freq (Hz): ");
  Serial.println(fps / opto_per);
  Serial.print("Opto pulses per train: ");
  Serial.println(train_length);
  Serial.print("Opto train cycle (s): ");
  Serial.println(train_cycle / fps);
  Serial.print("Pulse width (ms): ");
  Serial.println(pulsewidth_2_opto/1000);
  Serial.print("Ch1 optophoto cycle time (us): ");
  Serial.println(cycletime_photom_1_opto);
  Serial.print("Ch2 optophoto cycle time (us): ");
  Serial.println(cycletime_photom_2_opto);
  Serial.print("Opto-photometry overlap: ");
  Serial.println(optophotooverlap);

  // Same color opto
  Serial.println("============== SCoptophoto ==============");
  Serial.print("Same-color opto freq (Hz): ");
  Serial.println(fps / scopto_per);
  Serial.print("Same-color opto train length (s): ");
  Serial.println(sctrain_length / (fps/scopto_per));
  Serial.print("Polarity for tristate enable (1 = active high, 0 = active low): ");
  Serial.println(tristatepinpol); // Polarity for the tristatepin (1 = active high, 0 = active low). Default active low.)
  Serial.print("Same-color opto train cycle (s): ");
  Serial.println(sctrain_cycle / fps);
  Serial.print("Pulse width (ms): ");
  Serial.println(pulsewidth_1_scopto/1000);
  Serial.print("Ch1 scoptophoto cycle time (us): ");
  Serial.println(cycletime_photom_1_scopto);
  Serial.print("Ch2 scoptophoto cycle time (us): ");
  Serial.println(cycletime_photom_2_scopto);
  
  // Scheduler
  Serial.println("============== Scheduler ==============");
  Serial.print("Scheduler enabled: ");
  Serial.println(usescheduler);
  Serial.print("Preopto time (s): ");
  Serial.println(preoptotime);
  Serial.print("Preopto pulses: ");
  Serial.println(npreoptopulse);
  Serial.print("Number of opto trains: ");
  Serial.println(ntrain);
  Serial.print("Manual Overrideable: ");
  Serial.println(manualscheduleoverride);
  Serial.print("Listen mode: ");
  Serial.println(listenmode);
  Serial.print("Listen polarity (1 - positive, 0 - negative): ");
  Serial.println(listenpol);
  Serial.print("Use opto RNG (Does not apply to listen mode): ");
  Serial.println(useRNG);
  Serial.print("Opto RNG pass rate is (%): ");
  Serial.println(threshrng);
  Serial.print("Randomize ITI (1 - yes, 0 - no): ");
  Serial.println(randomiti);
  Serial.print("Randomized ITI range (s): [");
  Serial.print(rng_cycle_min);
  Serial.print(",");
  Serial.print(rng_cycle_max);
  Serial.println(")");

  // Scheduler
  Serial.println("============== Schedule Indicator ==============");
  Serial.print("Use schedule indicator: ");
  Serial.println(useschedulerindicator);
  Serial.print("Pre-opto color: ");
  Serial.println(preoptocolor);
  Serial.print("In-opto color: ");
  Serial.println(inoptocolor);
  Serial.print("Post-opto color: ");
  Serial.println(postoptocolor);
  
  // Cam
  Serial.println("============== Camera ==============");
  Serial.print("Cam frequency (Hz): ");
  Serial.println(freq);
  Serial.print("Cam cycle time (ms): ");
  Serial.println(cycletime/1000);
  Serial.print("Cam on time (us): ");
  Serial.println(ontime);
  Serial.print("Audio sync: ");
  Serial.println(syncaudio);
  Serial.print("Audio sync tone frequency: ");
  Serial.println(audiofreq);

  // Encoder
  Serial.println("============== Encoder ==============");
  Serial.print("Encoder enabled: ");
  Serial.println(useencoder);
  Serial.print("Auto encoder serial: ");
  Serial.println(autoencserial);

  // Food TTL
  Serial.println("============= Food TTL =============");
  Serial.print("Food TTL: ");
  Serial.println(usefoodpulses);
  Serial.print("Delay from the opto start (s): ");
  Serial.println(nfoodpulsedelay / 1000);
  Serial.print("Food TTL on time (ms): ");
  Serial.println(foodpulse_ontime);
  Serial.print("Food TTL cycle time (ms): ");
  Serial.println(foodpulse_cycletime);
  Serial.print("Food TTLs per train: ");
  Serial.println(foodpulses);
  Serial.print("Food TTL trains: ");
  Serial.println("Same as opto");
  Serial.print("Opto then food TTLs: ");
  Serial.println(optothenfood);
  Serial.print("Complement delay before the opto start (s): ");
  Serial.println(nfoodpulsedelay_complement / fps);

  // Food TTL Cue
  Serial.println("========== Food TTL Cue ========");
  Serial.print("Food TTL cue: ");
  Serial.println(usecue);
  Serial.print("Food TTL cue delay (ms): ");
  Serial.println(cuedelay);
  Serial.print("Food TTL cue duration (ms): ");
  Serial.println(cuedur);

  // Food TTL Conditional
  Serial.println("====== Food TTL Conditional ======");
  Serial.print("Food TTL Conditional: ");
  Serial.println(foodttlconditional);
  Serial.print("Food TTL Action delay (ms): ");
  Serial.println(actiondelay);
  Serial.print("Food TTL Action duration (ms): ");
  Serial.println(actiondur);
  Serial.print("Food TTL Time out window (ms): ");
  Serial.println(deliverydur);

  // Multiple trial types
  Serial.println("====== Multi trialtypes ======");
  Serial.print("N trial types: ");
  Serial.println(ntrialtypes);
  Serial.print("Current trial type to edit: ");
  Serial.println(trialtype_edit);
  for (byte ip = 0; ip < maxtrialtypes; ip++){
    Serial.print("----------- ");
    Serial.print("Trial type: ");
    Serial.print(ip);
    Serial.println(" -----------");
    Serial.print("IO: ");
    switch (ip){
      case 0:
        Serial.println(trialio0);
        break;
      case 1:
        Serial.println(trialio1);
        break;
      case 2:
        Serial.println(trialio2);
        break;
      case 3:
        Serial.println(trialio3);
        break;
    }
    Serial.print("Frequency weight: ");
    Serial.println(freq_vec[ip]);
    Serial.print("Cue duration (ms): ");
    Serial.println(cuedur_vec[ip]);
    Serial.print("TTL on time (ms): ");
    Serial.println(foodpulse_ontime_vec[ip]);
    Serial.print("TTL cycle time (ms): ");
    Serial.println(foodpulse_cycletime_vec[ip]);
    Serial.print("TTLs per train: ");
    Serial.println(foodpulses_vec[ip]);
    Serial.print("TTL Time out window (ms): ");
    Serial.println(deliverydur_vec[ip]);
  }
  
  // System wide
  Serial.println("============= System =============");
  Serial.print("Auto echo trial info: ");
  Serial.println(autoechotrialinfo);
  Serial.print("Auto echo trial periodicity (ms): ");
  Serial.println(cycletime_slow);
}

void slowserialecho(void){
  // Echoing back trial info on a schedule
  // Only when pulsing
  if (pulsing && autoechotrialinfo){
    if ((tnowmillis - echotime_slow) >= cycletime_slow){
      // Enters here every once a slow cycle (1s default)
      echotime_slow = tnowmillis;

      // Echo back scheduler info
      echo[0] = 0;
      echo[1] = 0;
      echo[2] = 0;
      echo[3] = 1; // Don't change
      if (!usescheduler){
        echo[2] = 1; // Echo back [0 0 1 1] as no scheduler
      }
      if (inpreopto){
        echo[0] = itrain;
        echo[1] = ntrain; // first byte shows n train, second byte shows itrain
        echo[2] = 2; // Echo back [0 0 2 1] as preopto
      }
      else if (inpostopto){
        echo[0] = itrain;
        echo[1] = ntrain; // first byte shows n train, second byte shows itrain
        echo[2] = 3;; // Echo back [0 0 3 1] as postopto
      }
      else {
        echo[0] = itrain;
        echo[1] = ntrain; // first byte shows n train, second byte shows itrain
      }
      Serial.write(echo, 4);

      // Echo back RNG info
      echo[3] = 2; // Don't change
      echo[2] = useRNG; // Echo back [X X 0 X] as no RNG
      echo[1] = trialtype; 
      echo[0] = trainpass;
      Serial.write(echo, 4);
    }
  }
}
