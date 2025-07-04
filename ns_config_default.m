global nicfg

% Set the up the nidaq recording
nicfg.BasePath         = 'C:\Users\andermannlab\Documents\MATLAB\temp\';       % Set the path in which data will be saved
nicfg.ArduinoCOM       = 21;%5;             % Set the COM port for the Arduino, < 0 means off
nicfg.RecordRunning    = true;         % Use quad encoder or not
nicfg.baumrate         = 19200;         % Baumrate 9600 for v1, 19200 for v3
nicfg.NidaqDevice      = 'Dev1';        % Device name
nicfg.NidaqChannels    = 8;             % Set the number of NIDAQ channels to record (e.g. 6 means 0:5)
nicfg.NidaqDigitalChannels = 0;         % Set the number of digital channels on Port0 to record, starting at Line0
nicfg.NidaqFrequency   = 2500;          % Set the recording frequency for the nidaq
nicfg.useMLlibrary     = false;          % Use monkeylogic library
nicfg.usepicoDAQ       = false;          % Use picoDAQ
nicfg.picoDAQparams     = {'daqcom', 'COM28', 'frequency', nicfg.NidaqFrequency}; % Parameters to be parsed
nicfg.RunningFrequency = 30;         % Set the frequency at which running is recorded
nicfg.DigitalString    = 'Port0/Line'; % Set a digital channel to be recorded, blank means no digital channels
nicfg.AImode           = 'SingleEnded'; % 'Differential', 'SingleEnded', 'SingleEndedNonReferenced'. Nidaq only
nicfg.ChannelNames     = { ...
                            'PD1', 1, ...
                            'Ch1in', 2, ...
                            'camera', 3, ...
                            'ensure', 4, ...
                            'PD2', 5, ...
                            'lick', 6,...
                            'Ch2in', 7,...
                            'Buzz', 8
                         };             % Set the names of each channel to be saved
nicfg.DigitalChannelNames = { ...
                         };

nicfg.serverupload = false;
nicfg.mousedate = true;                 % Set the order of mouse vs date
nicfg.serveradd = {...
    'SZ', '\\anastasia\data\photometry';...
    'AL', '\\sweetness\Fiber Photometry';...
    'KF', '\\sweetness\Fiber Photometry';...
    'JR', '\\atlas\NIDAQ_run_LFP_etc\Ephys_Data';...
    'MB', '\\atlas\NIDAQ_run_LFP_etc\Ephys_Data';...
    'IS', '\\anastasia\data\photometry';...
    'RE', '\\sweetness\Fiber Photometry';...
    'KE', '\\sweetness\Fiber Photometry'};

% =========================================================================
nicfg.MouseName = 'SZ00';
nicfg.Rig = '';

%% Version 3 Omnibox
% optophotometry, same-color-optophotometry, encoder-setup, Scheduler, opto-delayed TTL
% Values must be integers 0-255:

% Master enable
nicfg.omnibox.enable = true;

% Modes
% Two-color photometry
nicfg.tcp.enable = true; % Default true.
nicfg.tcp.behaviorcycle = 30; % Train cycle in seconds. E.g., 30 means 30 seconds from start to start. Default 30.

% Change pulse cycles (CAUTION)
nicfg.tcp.pulsecycle1 = 100; % Pulse cycle 1 in X * 100 us. E.g., 100 means 10 ms. Default 100 (10 ms)
nicfg.tcp.pulsecycle2 = 100; % Pulse cycle 1 in X * 100 us. E.g., 100 means 10 ms. Default 100 (10 ms)

% Optophotometry (two colors)
% Variable pulse width. Variable pulse cycle (only use for pure optomode)
nicfg.optophotometry.enable = false; % Default false
nicfg.optophotometry.freqmod = 5; % Frequency is actually 50/X. E.g., 5 means 10 Hz. Default 5 (10 Hz).
nicfg.optophotometry.trainlength = 10; % Opto pulses per train. E.g., 10 means 10 pulses per train. Default 10.
nicfg.optophotometry.cycle = 30; % Train cycle in seconds. E.g., 30 means 30 seconds from start to start. Default 30.
nicfg.optophotometry.pulsewidth = 10; % Pulth width in ms. E.g., 10 means 10 ms pulses. Default 10. Going above 19 may cause issues.
nicfg.optophotometry.overlap = false; % If overlap is true, opto and photometry pulses start at the same time with no offset. Pulsewidth is now pulsewidth + pulsecycle1

% Change pulse cycles (CAUTION)
% Generally only change these values for pure optomode
% 1. To change pulse cycle for pure opto experiments, change pulsecycle1 to 0
% and pulse cycle 2 to match the pulse frequency. It is the sum of the two
% that matters.
% 2. The final pulse frequency is determined by these values as well as freq
% mod above
nicfg.optophotometry.pulsecycle1 = 65; % Pulse cycle 1 in X * 100 us. E.g., 65 means 6.5 ms. Default 65 (6.5 ms)
nicfg.optophotometry.pulsecycle2 = 135; % Pulse cycle 1 in X * 100 us. E.g., 135 means 13.5 ms. Default 135 (13.5 ms)

% Same-color optophotometry
% Variable pulse width. 20 ms means always on
nicfg.scoptophotometry.enable = false; % Default false
nicfg.scoptophotometry.freqmod = 5; % Frequency is actually 50/X. E.g., 5 means 10 Hz. Default 5 (10 Hz).
nicfg.scoptophotometry.trainlength = 10; % Opto pulses per train. E.g., 10 means 10 pulses per train. Default 10.
nicfg.scoptophotometry.cycle = 30; % Train cycle in seconds. E.g., 30 means 30 seconds from start to start. Default 30.
nicfg.scoptophotometry.pulsewidth = 10; % Pulth width in ms. E.g., 10 means 10 ms pulses. Default 10. Going above 19 may cause issues.

% Scheduler
nicfg.scheduler.enable = false; % Default false
nicfg.scheduler.delay = 120; % Delayed opto start in seconds. E.g., 120 means 2 min delay. Default 120s. Max 65535s.
nicfg.scheduler.ntrains = 10; % Number of trains. Default 10.
nicfg.scheduler.manualoverride = false; % Allow for manual swichingoverride. Default false (don't leave floating if true).
nicfg.scheduler.listenmode = false; % Enable listenmode, which makes each tran triggered by external input. This will enable manualoverride above.

% Scheduler indicator
nicfg.scheduler.indicator.enable = true;
nicfg.scheduler.indicator.preopto = [0 0 3 1]; % [R G B doubler], 0-3 for R, B, G
nicfg.scheduler.indicator.inopto = [0 3 0 1];
nicfg.scheduler.indicator.postopto = [3 0 0 1];
nicfg.scheduler.indicator.switchoffmode = 0; % 0: off, 1: stay, 2: preopto, 3: inopto, 4: postopto

% Hardware Opto RNG (determines a train goes through or not)
% (Scheduler only and does not apply to the listening mode)
% May implement RNG values imported from Matlab in the future
nicfg.scheduler.useRNG = false; % Default false
nicfg.scheduler.passchance = 30; % Pass chance in percentage (30 = 30% pass)

% Control experiments (no stim)
% Basically sets RNG mode and pass chance = 0
nicfg.scheduler.control = false;

% Randomized ITI
% (Scheduler only and does not apply to the listening mode)
% May implement RNG values imported from Matlab in the future
nicfg.scheduler.randomITI = false; % Default false
nicfg.scheduler.randomITI_min = 30; % Lowest value of ITI (inclusive, in seconds). Applies to both optophotometry and scoptophotometry
nicfg.scheduler.randomITI_max = 40; % Highest value of ITI (exclusive, in seconds). Applies to both optophotometry and scoptophotometry

% Behavior
% TTL pulses that happen X seconds after each opto train onset (for food
% delivery or synchronizing).
% Delivery (unconditional): Opto start => delivery delay => delivery
nicfg.optodelayTTL.enable = false; % Default false
nicfg.optodelayTTL.delay = 20; % Delay in 100 ms. E.g., 20 means 2 seconds. Default 20 (2s). Max 65535 (6553.5s).
nicfg.optodelayTTL.pulsewidth = [15 15 15 15]; % Pulsewidth in X * 10 ms. E.g., 15 means 150 ms pulses. Default is 15 (150 ms).
nicfg.optodelayTTL.cycle = [30 30 30 30]; % Pulse cycle in X * 10 ms. E.g., 30 means 300 ms pulses. Default is 30 (300 ms).
nicfg.optodelayTTL.trainlength = [5 5 5 5]; % Pulse train length. E.g., 5 means 5 pulses. Default is 5.
nicfg.optodelayTTL.optothenTTL = true; % Sets the sequence: opto->food or food->opto. Has to be true in listen mode. Default true (opto => food).
nicfg.optodelayTTL.lead = 4; % How many seconds is the food TTL armed before an opto train. This does not affect the cue/actoin/reward delays above. Default 4s. Max 65535s.

% Opto-delayed TTL buzzer (cue)
% Cue: Opto start => buzzer delay => buzzer duration
nicfg.optodelayTTL.cueenable = false; % Buzzer or not (default false)
nicfg.optodelayTTL.cuedelay = 20; % Delay in 100 ms. E.g., 20 means 2 seconds. Default 20 (2s)
nicfg.optodelayTTL.cuedur = [10 10 10 10]; % Delay in 100 ms. E.g., 10 means 1 seconds. Default 10 (1s)

% Action: Opto start => action delay => action window
% Delivery (conditional): Opto start => delivery delay => delivery window start => delivery (if action) => timeout (if no delivery)
% If action and devliveries have the same delay, that means food devlivery happens as soon as a lick
nicfg.optodelayTTL.conditional = [false, false, false, false]; % TTL delivery is conditional or not. If so, pin 10 must be hooked up with an active-high input. Input comes during the delay window will allow for subsequent pulse output.
nicfg.optodelayTTL.actiondelay = 20; % Delay in 100 ms. E.g., 20 means 2 seconds. Default 20 (2s)
nicfg.optodelayTTL.actiondur = 50; % Duration in 100 ms. E.g., 50 means 5 seconds. Default 50 (5s)
nicfg.optodelayTTL.deliverydur = [50 50 50 50]; % Duration in 100 ms. E.g., 50 means 5 seconds. Default 50 (5s)

% Multiple trial types
nicfg.optodelayTTL.ntrialtypes = 1; % Multiple trial types (Max is 4)
nicfg.optodelayTTL.trialfreq = [3 0 0 0]; % Relative weights of trial frequency
nicfg.optodelayTTL.determtrialtypes = false;

nicfg.optodelayTTL.type1.cuetype = 'Buzzer'; % 'Buzzer' (native PWM), 'DIO', 'PWMRGB'
nicfg.optodelayTTL.type1.RGB = [0 0 0]; % [R G B] Only used in DIO or PWMRGB. Values 0-7 for intensity PWM and 0-1 for DIO.
nicfg.optodelayTTL.type1.rewardtype = 'Native'; % 'Native', 'DIO'
nicfg.optodelayTTL.type1.DIOport = 0; % Only used in DIO

nicfg.optodelayTTL.type2.cuetype = 'PWMRGB'; % 'Buzzer' (native PWM), 'DIO', 'PWMRGB'
nicfg.optodelayTTL.type2.RGB = [7 0 0]; % [R G B] Only used in DIO or PWMRGB. Values 0-7 for intensity PWM and 0-1 for DIO.
nicfg.optodelayTTL.type2.rewardtype = 'DIO'; % 'Native', 'DIO'
nicfg.optodelayTTL.type2.DIOport = 0; % Only used in DIO

nicfg.optodelayTTL.type3.cuetype = 'PWMRGB'; % 'Buzzer' (native PWM), 'DIO', 'PWMRGB'
nicfg.optodelayTTL.type3.RGB = [0 7 0]; % [R G B] Only used in DIO or PWMRGB. Values 0-7 for intensity PWM and 0-1 for DIO.
nicfg.optodelayTTL.type3.rewardtype = 'DIO'; % 'Native', 'DIO'
nicfg.optodelayTTL.type3.DIOport = 1; % Only used in DIO

nicfg.optodelayTTL.type4.cuetype = 'PWMRGB'; % 'Buzzer' (native PWM), 'DIO', 'PWMRGB'
nicfg.optodelayTTL.type4.RGB = [0 0 7]; % [R G B] Only used in DIO or PWMRGB. Values 0-7 for intensity PWM and 0-1 for DIO.
nicfg.optodelayTTL.type4.rewardtype = 'DIO'; % 'Native', 'DIO'
nicfg.optodelayTTL.type4.DIOport = 2; % Only used in DIO

% Encoder
nicfg.encoder.enable = true;
nicfg.encoder.autoecho = true; % Using auto echo (turn off for debugging)

% Trial and RNG info echo
nicfg.onlineecho.enable = true; % Auto trial and rng echo (n = 1 yes, 0 no)
nicfg.onlineecho.periodicity = 10; % Periodicity in 100 ms. E.g., 10 means 1 second. Default 10 (1s)

% Audio sync
nicfg.audiosync.enable = false; % Default false
nicfg.audiosync.freq = 20; % Frequency in X * 100 Hz . E.g., 10 means 1000 Hz. Default is 20 (2000 Hz)