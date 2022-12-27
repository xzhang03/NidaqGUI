// General LED functions go here
void turn_ch1_on(void){
  digitalWrite(ch1_pin, HIGH);
  ch1_on = true;
  // Serial.println(t1); Confirm only once every 20 ms
}

void turn_ch1_off(void){
  digitalWrite(ch1_pin, LOW);
  ch1_on = false;

  // debug ch1 on to ch1 off for opto
  if ((debugmode || serialdebug) && showopto){
    if (ftest1){
      Serial.print("Opto ");
      Serial.print(opto_counter);
      Serial.print(". Pulse width (us): ");
      Serial.print(tnow - ttest1);
      Serial.print(". At (ms): ");
      Serial.println(tnowmillis);
      ftest1 = false;
    }
  }
}

void turn_ch2_on(void){
  digitalWrite(ch2_pin, HIGH);
  ch2_on = true; 
}

void turn_ch2_off(void){
  digitalWrite(ch2_pin, LOW);
  ch2_on = false; 

  // debug ch2 on to ch2 off
  if ((debugmode || serialdebug) && showopto){
    if (ftest1 && train){
      Serial.print("Opto ");
      Serial.print(opto_counter);
      Serial.print(". Pulse width (us): ");
      Serial.print(tnow - ttest1);
      Serial.print(". At (ms): ");
      Serial.println(tnowmillis);
      ftest1 = false;
    }
  }
}
