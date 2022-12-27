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
