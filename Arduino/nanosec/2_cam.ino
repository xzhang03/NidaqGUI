// Cam/audio buzz functions go here

void camerapulse(void){
  if (pulsing){
    if ((tnow - pulsetime) >= cycletime){
      if (!onoff){
        pulsetime = micros();
        if (autoencserial){
          pos = myEnc.read();
          Serial.write((byte *) &pos, 4); // Send position
        }
        
        // Serial.println(pos);
        digitalWrite(cam_pin, HIGH);
        digitalWrite(led_pin, HIGH);

        // Audio
        if (syncaudio){
          tone(audiopin, audiofreq);
        }
        
        onoff = true;

        // Advance pulse counter
        pcount++;
      }
    }
    else if ((tnow - pulsetime) >= ontime){
      if (onoff){
        digitalWrite(cam_pin, LOW);
        digitalWrite(led_pin, LOW);
        onoff = false;

        // Audio
        if (syncaudio){
          noTone(audiopin);
        }
      }
    }
  }
  else {
    if (onoff){
      digitalWrite(cam_pin, LOW);
      digitalWrite(led_pin, LOW);
      onoff = false;

      // Audio
      if (syncaudio){
        noTone(audiopin);
      }
    }
  }
}
