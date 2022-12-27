// Performance check functions go here
void perfc(void){
  #if perfcheck
    if (pulsing) {
      if ((ipreoptopulse == 0) && !pc_going){
        // Very first cycle during pulsing
        ix_pc = 0;
        tnow_pc = tnow;
        pc_going = true;
      }
      else if ((ix_pc < cycles_pc) && pc_going){
        ix_pc++;
      }
      else if (pc_going){
        tpc = tnow - tnow_pc;
        pc_going = false;
      }
    }
  #endif
}
