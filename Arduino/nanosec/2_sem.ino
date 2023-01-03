// m table
// ========== Operational ==========
// 2: new cycle time (1000000/n, n in Hz)
// 1: start pulse
// 0: stop pulse
// 5: Quad encoder position
// 9: Show all parameters
// 253: reboot (n = 104)
// 254: version
// 255: status update (n = variable)

// ========== Opto & photom ==========
// 3: TCP mode (n = 0 TCP, n = 1 optophotometry, n = 2 samecolor optophotometry)
// 6: Change opto frequency (opto_per = n, f = 50/n)
// 7: Change opto pulse per train (train_length = n)
// 8: Change opto train cycle (train_cycle = n * 50, n in seconds)
// 59: Opto-photometry overlap (n = 1 yes, 0 no)
// 10: Change scopto frequency (scopto_per = n, f = 50/n)
// 11: Change scopto train cycle (train_cycle = n * 50)
// 12: Change scopto pulse per train (train_length = n)
// 13: Change scopto pulse width (n * 1000 us)
// 14: Change Opto pulse width (n * 1000 us)
// 29: Tristate pin polarity (1 = active high, 0 = active low);
// 47: Change tcp behavioral train cycle (train_cycle = n * 50)
// 36: Cycle 1 for optophotometry (only changed for pure optogenetic experiments; cycle = n * 100 us)
// 37: Cycle 2 for optophotometry (only changed for pure optogenetic experiments; cycle = n * 100 us)
// 55: Add Cycle 1 for optophotometry (cycle = cycle + n * 256 * 100 us);
// 56: Add Cycle 2 for optophotometry (cycle = cycle + n * 256 * 100 us);
// 53: Cycle 1 for TCP (only changed for pure TCP experiments; cycle = n * 100 us)
// 54: Cycle 2 for TCP (only changed for pure TCP experiments; cycle = n * 100 us)
// 57: Add Cycle 1 for TCP (cycle = cycle + n * 256 * 100 us);
// 58: Add Cycle 2 for TCP (cycle = cycle + n * 256 * 100 us);


// ============ Scheduler ============
// 15: Use scheduler (n = 1 yes, 0 no) 
// 4: Set delay (delay_cycle = n * 50, n = 1 means 1 second). Only relevant when scheduler is on
// 52: Adding delay (n * 256 * 50, n = 1 means 256 second). Only relevant when scheduler is on
// 16: Number of trains (n trains)
// 46: Adding trains (trains = trains + n * 256)
// 17: Enable manual scheduler override
// 27: Listening mode on or off (n = 1 yes, 0 no). Will turn on manual override.
// 28: Listen mode polarity (1 = active high, 0 = active low)

// ============ Hardware RNG ============
// 38: use RNG or not (n = 1 yes, 0 no)
// 39: Pass chance in percent (30 = 30% pass chance)
// 40: RNG ITI or not (n = 1 yes, 0 no)
// 41: min randomized ITI (n * 1 s)
// 42: max randomized ITI (n * 2 s)

// ============ Food TTL ============
// 24: Use Food TTL or not (n = 1 yes, 0 no) 
// 18: Delay time after opto (n * 100 ms)
// 50: Adding delay time after opto (n * 256 * 100 ms)
// 19: Pulse on time (n * 10 ms)
// 20: Pulse cycle time (n * 10 ms)
// 21: Pulses per train (n)
// 30: Use buzzer cue or not (n = 1 yes, 0 no)
// 31: Buzzer delay (n * 100 ms)
// 32: Buzzer duration (n * 100 ms)
// 48: Opto then Food (n = 1 yes, 0 no)
// 49: Lead time before opto (n s)
// 51: Adding lead time before opto (+ n * 256 s) 

// ====== Food TTL Conditional ======
// 22: Conditional or not (n = 1 yes, 0 no) 
// 33: Action period delay (n * 100 ms)
// 34: Action period duration (n * 100 ms)
// 35: Delivery period duration (n * 100 ms);

// ============= Encoder =============
// 23: Encoder useage (n = 1 yes, 0 no) 
// 43: Auto encoder serial transmission (n = 1 yes, 0 no)

// ============ Audio sync ============
// 25: Audio sync (n = 1 yes, 0 no)
// 26: Audio sync tone frequency (n * 100 Hz)

// ========== Trial and RNG Echo ==========
// 44: Auto trial and rng echo (n = 1 yes, 0 no)
// 45: Auto trial and rng echo periodicity (n * 100 ms)
