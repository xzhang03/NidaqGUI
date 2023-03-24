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
// 59: Opto-photometry overlap (n = 1 yes, 0 no) [k]
// 10: Change scopto frequency (scopto_per = n, f = 50/n) [:]
// 11: Change scopto train cycle (train_cycle = n * 50) [;]
// 12: Change scopto pulse per train (train_length = n) [<]
// 13: Change scopto pulse width (n * 1000 us) [=]
// 14: Change Opto pulse width (n * 1000 us) [>]
// 29: Tristate pin polarity (1 = active high, 0 = active low) [M]
// 47: Change tcp behavioral train cycle (train_cycle = n * 50) [_]
// 36: Cycle 1 for optophotometry (only changed for pure optogenetic experiments; cycle = n * 100 us) [T]
// 37: Cycle 2 for optophotometry (only changed for pure optogenetic experiments; cycle = n * 100 us) [U]
// 55: Add Cycle 1 for optophotometry (cycle = cycle + n * 256 * 100 us) [g]
// 56: Add Cycle 2 for optophotometry (cycle = cycle + n * 256 * 100 us) [h]
// 53: Cycle 1 for TCP (only changed for pure TCP experiments; cycle = n * 100 us) [e]
// 54: Cycle 2 for TCP (only changed for pure TCP experiments; cycle = n * 100 us) [f]
// 57: Add Cycle 1 for TCP (cycle = cycle + n * 256 * 100 us) [i]
// 58: Add Cycle 2 for TCP (cycle = cycle + n * 256 * 100 us) [j]

// ============ Scheduler ============
// 15: Use scheduler (n = 1 yes, 0 no) [?]
// 4: Set delay (delay_cycle = n * 50, n = 1 means 1 second). Only relevant when scheduler is on
// 52: Adding delay (n * 256 * 50, n = 1 means 256 second). Only relevant when scheduler is on [d]
// 16: Number of trains (n trains) [@]
// 46: Adding trains (trains = trains + n * 256) [^]
// 17: Enable manual scheduler override [A]
// 27: Listening mode on or off (n = 1 yes, 0 no). Will turn on manual override [K]
// 28: Listen mode polarity (1 = active high, 0 = active low) [L]

// ============ Hardware RNG ============
// 38: use RNG or not (n = 1 yes, 0 no) [V]
// 39: Pass chance in percent (30 = 30% pass chance) [W]
// 40: RNG ITI or not (n = 1 yes, 0 no) [X]
// 41: min randomized ITI (n * 1 s) [Y]
// 42: max randomized ITI (n * 2 s) [Z]
// 64: Report RNG (n = 0 ITI, 1 opto, 2 trialtype)[p]

// ============ Food TTL ============
// 24: Use Food TTL or not (n = 1 yes, 0 no)  [H]
// 18: Delay time after opto (n * 100 ms) [B]
// 50: Adding delay time after opto (n * 256 * 100 ms) [b]
// 19: Pulse on time (n * 10 ms) [C]
// 20: Pulse cycle time (n * 10 ms) [D]
// 21: Pulses per train (n) [E]
// 30: Use food cue or not (n = 1 yes, 0 no) [N]
// 31: Cue delay (n * 100 ms) [O]
// 32: Cue duration (n * 100 ms) [P]
// 48: Opto then Food (n = 1 yes, 0 no) [`]
// 49: Lead time before opto (n s) [a]
// 51: Adding lead time before opto (+ n * 256 s) [c]

// ====== Food TTL Conditional ======
// 22: Conditional or not (n = 1 yes, 0 no) [F]
// 33: Action period delay (n * 100 ms) [Q]
// 34: Action period duration (n * 100 ms) [R]
// 35: Delivery period duration (n * 100 ms) [S]

// ====== Multi trial type ======
// 62: Max trial types (n = 1-4)[n]
// 63: Current trialtype to edit (n = 0 - 3)[o]
// 65: Trial IO upper byte (n = MSB)[q]
// 66: Trial IO lower byte (n = LSB)[r]
// 67: Trial frequency weight (n = weight)[s]

// ============= Encoder =============
// 23: Encoder useage (n = 1 yes, 0 no) [G]
// 43: Auto encoder serial transmission (n = 1 yes, 0 no) [[]

// ============ Audio sync ============
// 25: Audio sync (n = 1 yes, 0 no) [I]
// 26: Audio sync tone frequency (n * 100 Hz) [J]

// ========== Trial and RNG Echo ==========
// 44: Auto trial and rng echo (n = 1 yes, 0 no) [\]
// 45: Auto trial and rng echo periodicity (n * 100 ms) []]

// ============== i2c ==============
// 60: Scan i2c addresses [l]
// 61: Test PCA9685 [m]
// 68: Test MCP23008 [t]

// 69: [u]
// 70: [v]
// 71: [w]
// 72: [x]
// 73: [y]
// 74: [z]
// 75: [{]
// 76: [|]
// 77: [}]
// 78: [~]
