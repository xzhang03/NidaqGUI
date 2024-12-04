// Pin initialize funtions go here
void pin_ini(void){
  // Essential pin
  pinMode(cam_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(ch1_pin, OUTPUT);
  pinMode(ch2_pin, OUTPUT);
  pinMode(tristatepin, OUTPUT);
  pinMode(foodTTLpin, OUTPUT);
  pinMode(audiopin, OUTPUT);

  if (listenpol){
    pinMode(switchpin, INPUT_PULLDOWN); // active high
  }
  else {
    pinMode(switchpin, INPUT_PULLUP); // active low
  }
  
  pinMode(foodTTLinput, INPUT_PULLDOWN);
  
  digitalWrite(cam_pin, LOW);
  digitalWrite(led_pin, LOW);
  digitalWrite(ch1_pin, LOW);
  digitalWrite(ch2_pin, LOW);
  digitalWrite(foodTTLpin, LOW);

  // Debug pin
  pinMode(serialpin, OUTPUT); // Parity signal for serial pin
  pinMode(schedulerpin, OUTPUT); // On when scheduler is used
  pinMode(preoptopin, OUTPUT); // preopto
  pinMode(inoptopin, OUTPUT); // preopto
  pinMode(postoptopin, OUTPUT); // preopto

  digitalWrite(serialpin, LOW);
  digitalWrite(schedulerpin, LOW);
  digitalWrite(preoptopin, LOW);
  digitalWrite(inoptopin, LOW);
  digitalWrite(postoptopin, LOW);

  if (usescheduler){
    #if debugpins
      digitalWrite(schedulerpin, HIGH);
      digitalWrite(preoptopin, HIGH);
    #endif

    if (useschedulerindicator){
      shedulerindicator(preoptocolor);
    }
  }
}
