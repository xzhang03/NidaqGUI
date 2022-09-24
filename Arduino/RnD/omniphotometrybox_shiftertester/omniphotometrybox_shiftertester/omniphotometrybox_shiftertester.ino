byte m, m1, m2;
#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 1){
    Serial.read();
    Serial.read();

    Serial.print("Sending: 2 3... ");
    Wire.beginTransmission(0x11);
    m1 = 2 + 0b00000000;
    m2 = 3 + 0b00010000;
    Wire.write(m1);
    Wire.write(m2);
    Wire.endTransmission(0x11);
    Serial.println("Done");

    delay(1000);
    Serial.println("Requesting... ");
    Wire.requestFrom(0x11, 1);

    delay(10);
    if (Wire.available() > 0){
      m = Wire.read();
      Serial.print("Got ");
      Serial.print(m, BIN);

      m1 = (m >> 4) & 0b00001111;
      m2 = m & 0b00001111;

      Serial.print(" = ");
      Serial.print(m1);
      Serial.print(" + ");
      Serial.println(m2);
    }
    
    
  }
  
}
