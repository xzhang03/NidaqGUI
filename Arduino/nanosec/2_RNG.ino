// RNG functions goes here

void rng_init(void){
  // Initialize RNG
  Entropy.Initialize();
}


// RNG
void rng(byte *arraylc, byte maxrnglc, byte minrnglc, int l){
  for (int rngind = 0; rngind < l; rngind++){
    arraylc[rngind] = Entropy.random(minrnglc, maxrnglc); // SLOW 1 ms per
//    result[rngind] = random(0, maxrnglc); // Fast 0.1 us per
  }
  if ((debugmode || serialdebug)){
    Serial.print("Generated ");
    Serial.print(l);
    Serial.print(" RNG numbers [");
    Serial.print(minrnglc);
    Serial.print(",");
    Serial.print(maxrnglc);
    Serial.println(").");
  }
}

// Deterministic trial type
void overwrite_ttype(byte *arraydet, byte inddet, byte types){
  // Get types
  byte type0, type1, type2, type3;
  type0 = (types >> 6) & 0B11;
  type1 = (types >> 4) & 0B11;
  type2 = (types >> 2) & 0B11;
  type3 = types & 0B11;

  // Write
  arraydet[inddet*4] = type0;
  arraydet[inddet*4+1] = type1;
  arraydet[inddet*4+2] = type2;
  arraydet[inddet*4+3] = type3;

  if (debugmode || serialdebug){
    Serial.println("RNG overwrite:");
    Serial.print("Trial ");
    Serial.print(inddet*4);
    Serial.print(" => ");
    Serial.println(type0);
    Serial.print("Trial ");
    Serial.print(inddet*4+1);
    Serial.print(" => ");
    Serial.println(type1);
    Serial.print("Trial ");
    Serial.print(inddet*4+2);
    Serial.print(" => ");
    Serial.println(type2);
    Serial.print("Trial ");
    Serial.print(inddet*4+3);
    Serial.print(" => ");
    Serial.println(type3);
  }
}
