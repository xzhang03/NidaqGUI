// Mode switching functions go here

// Switch parameters
void modeswitch(void){
  if (optophotommode){
    pulsewidth_1 = pulsewidth_1_tcp;
    pulsewidth_2 = pulsewidth_2_opto; // Switch out the parameters
    cycletime_photom_1 = cycletime_photom_1_opto; // in micro secs (Ch1)
    cycletime_photom_2 = cycletime_photom_2_opto; // in micro secs (Ch2)
    digitalWrite(tristatepin, tristatepinpol); // Let ch2 go through. Write false if active low (false)
        
    if ((debugmode || serialdebug)){
      Serial.println("Optophotometry mode.");
    }
  }
  
  else if (tcpmode){
    pulsewidth_1 = pulsewidth_1_tcp;
    pulsewidth_2 = pulsewidth_2_tcp; // Switch out the parameters
    cycletime_photom_1 = cycletime_photom_1_tcp; // in micro secs (Ch1)
    cycletime_photom_2 = cycletime_photom_2_tcp; // in micro secs (Ch2)
    digitalWrite(tristatepin, tristatepinpol); // Let ch2 go through; Write false if active low (false)
    
    if ((debugmode || serialdebug)){
      Serial.println("Two-color photometry mode.");
      Serial.println("Scheduler is disabled");
    }
  }

  else if (samecoloroptomode){
    pulsewidth_1 = pulsewidth_1_tcp; // Switch out the parameters. Use tcp mode until scopto is on
    pulsewidth_2 = pulsewidth_2_tcp; // Switch out the parameters
    cycletime_photom_1 = cycletime_photom_1_scopto; // in micro secs (Ch1)
    cycletime_photom_2 = cycletime_photom_2_scopto; // in micro secs (Ch2)
    digitalWrite(AOpin, LOW);
    digitalWrite(tristatepin, !tristatepinpol); // DC Ch2. Write true if active low (false)
    
    if ((debugmode || serialdebug)){
      Serial.println("Same-color optophotometry mode.");
    }
  }
  
  // Update fps
  fps = 1000000 / (cycletime_photom_1 + cycletime_photom_2);
}
