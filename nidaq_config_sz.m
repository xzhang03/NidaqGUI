global nicfg

% Set the up the nidaq recording
nicfg.BasePath         = 'C:\Users\andermannlab\Documents\MATLAB\temp\';       % Set the path in which data will be saved
% nicfg.BasePath         = 'C:\Users\steph\OneDrive\Documents\MATLAB\temp';       % Set the path in which data will be saved
nicfg.ArduinoCOM       = 21; %5;             % Set the COM port for the Arduino, < 0 means off
% nicfg.ArduinoCOM       = 8;             % Set the COM port for the Arduino, < 0 means off
nicfg.RecordRunning    = true;         % Use quad encoder or not
nicfg.baumrate         = 19200;         % Baumrate 9600 for v1, 19200 for v3
nicfg.NidaqDevice      = 'Dev1';        % Device name
nicfg.useMLlibrary     = false;          % Use monkeylogic library
nicfg.NidaqChannels    = 4;             % Set the number of NIDAQ channels to record (e.g. 6 means 0:5)
nicfg.NidaqDigitalChannels = 1;         % Set the number of digital channels on Port0 to record, starting at Line0
nicfg.NidaqFrequency   = 2500;          % Set the recording frequency for the nidaq
nicfg.RunningFrequency = 30;         % Set the frequency at which running is recorded
nicfg.DigitalString    = 'Port0/Line'; % Set a digital channel to be recorded, blank means no digital channels
nicfg.ChannelNames     = { ...
                            'visual_stimulus', 1, ...
                            'monitor_refresh', 2, ...
                            'licking', 3, ...
                            'ensure', 4, ...
                            'quinine', 5, ...
                            'camera_pulses', 6 ...
                         };             % Set the names of each channel to be saved
                                        % Incremented by 1 relative to Nidaq board because Nidaq is 0-indexed
nicfg.DigitalChannelNames = { ...
                         };
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
%% Version 3 Omnibox
% optophotometry, same-color-optophotometry, encoder-setup, Scheduler, opto-delayed TTL
% Values must be integers 0-255:

% Master enable
nicfg.omnibox.enable = true;

% Modes
% Two-color photometry
nicfg.tcp.enable = true; % Default true.

% Optophotometry (two colors)
% Pulse width is fixed at 10 ms
nicfg.optophotometry.enable = false; % Default false
nicfg.optophotometry.freqmod = 5; % Frequency is actually 50/X. E.g., 5 means 10 Hz. Default 5 (10 Hz).
nicfg.optophotometry.trainlength = 10; % Opto pulses per train. E.g., 10 means 10 pulses per train. Default 10.
nicfg.optophotometry.cycle = 30; % Train cycle in seconds. E.g., 30 means 30 seconds from start to start. Default 30.

% Same-color optophotometry
% Variable pulse width. 20 ms means always on
nicfg.scoptophotometry.enable = false; % Default false
nicfg.scoptophotometry.freqmod = 5; % Frequency is actually 50/X. E.g., 5 means 10 Hz. Default 5 (10 Hz).
nicfg.scoptophotometry.trainlength = 10; % Opto pulses per train. E.g., 10 means 10 pulses per train. Default 10.
nicfg.scoptophotometry.cycle = 30; % Train cycle in seconds. E.g., 30 means 30 seconds from start to start. Default 30.
nicfg.scoptophotometry.pulsewidth = 10; % Pulth width in ms. E.g., 10 means 10 ms pulses. Default 10.

% Scheduler
nicfg.scheduler.enable = false; % Default false
nicfg.scheduler.delay = 120; % Delayed opto start in seconds. E.g., 120 means 2 min delay. Default 120s.
nicfg.scheduler.ntrains = 10; % Number of trains. Default 10.
nicfg.scheduler.manualoverride = false; % Allow for manual swichingoverride (experiemental). Default false.

% Opto-delayed TTL
% TTL pulses that happen X seconds after each opto train onset (for food
% delivery or synchronizing).
nicfg.optodelayTTL.enable = false; % Default false
nicfg.optodelayTTL.delay = 2; % Delay in seconds. E.g., 2 means 2 seconds. Default 2s.
nicfg.optodelayTTL.pulsewidth = 15; % Pulsewidth in X * 10 ms. E.g., 15 means 150 ms pulses. Default is 15 (150 ms).
nicfg.optodelayTTL.cycle = 30; % Pulse cycle in X * 10 ms. E.g., 30 means 300 ms pulses. Default is 30 (300 ms).
nicfg.optodelayTTL.trainlength = 5; % Pulse train length. E.g., 5 means 5 pulses. Default is 5.
nicfg.optodelayTTL.conditional = false; % TTL delivery is conditional or not. If so, pin 10 must be hooked up with an active-high input. Input comes during the delay window will allow for subsequent pulse output.

% Encoder
nicfg.encoder.enable = true;

% Audio sync
nicfg.audiosync.enable = false; % Default false
nicfg.audiosync.freq = 20; % Frequency in X * 100 Hz . E.g., 10 means 1000 Hz. Default is 20 (2000 Hz)