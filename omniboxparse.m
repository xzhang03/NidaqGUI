function omniboxparse(nicfg)
% This function communicates with teensy to setup omni box functions

%% Enable
if ~isfield(nicfg, 'omnibox')
    return;
else
    if ~nicfg.omnibox.enable
        return;
    end
end

%% Check conflict
if (nicfg.tcp.enable + nicfg.optophotometry.enable + nicfg.scoptophotometry.enable) ~= 1
    msgbox('Must select 1 and only 1 from Tcp, Optophotometry, and Same-color optophotometry modes.')
end

if (nicfg.audiosync.enable && nicfg.optodelayTTL.buzzerenable)
    msgbox('Cannot use audio sync and buzzer cue at the same time.')
end

%% TCP
if nicfg.tcp.enable
    % TCP
    fwrite(nicfg.arduino_serial, uint8([3 0]));
end

%% Optophotometry
if nicfg.optophotometry.enable
    % Optophotometry
    fwrite(nicfg.arduino_serial, uint8([3 1]));
    
    % Frequency
    fwrite(nicfg.arduino_serial, uint8([6 nicfg.optophotometry.freqmod]));
    
    % Train length
    fwrite(nicfg.arduino_serial, uint8([7 nicfg.optophotometry.trainlength]));
    
    % Train cycle
    fwrite(nicfg.arduino_serial, uint8([8 nicfg.optophotometry.cycle]));
    
    % Pulse width
    fwrite(nicfg.arduino_serial, uint8([14 nicfg.optophotometry.pulsewidth]));
    
    % Pulse cycle 1
    fwrite(nicfg.arduino_serial, uint8([36 nicfg.optophotometry.pulsecycle1]));
    
    % Pulse cycle 2
    fwrite(nicfg.arduino_serial, uint8([37 nicfg.optophotometry.pulsecycle2]));
end

%% Same-color optophotometry
if nicfg.scoptophotometry.enable
    % SC Optophotometry
    fwrite(nicfg.arduino_serial, uint8([3 2]));
    
    % Frequency
    fwrite(nicfg.arduino_serial, uint8([10 nicfg.scoptophotometry.freqmod]));
    
    % Train length
    fwrite(nicfg.arduino_serial, uint8([12 nicfg.scoptophotometry.trainlength]));
    
    % Train cycle
    fwrite(nicfg.arduino_serial, uint8([11 nicfg.scoptophotometry.cycle]));
    
    % Pulse width
    fwrite(nicfg.arduino_serial, uint8([13 nicfg.scoptophotometry.pulsewidth]));
    
    % Tristate pin polarity
    fwrite(nicfg.arduino_serial, uint8([29 nicfg.scoptophotometry.tristatepol]));
end

%% Scheduler
if nicfg.scheduler.enable
    % Scheduler
    fwrite(nicfg.arduino_serial, uint8([15 1]));
    
    % Delay
    fwrite(nicfg.arduino_serial, uint8([4 nicfg.scheduler.delay]));
    
    % Number of trains
    fwrite(nicfg.arduino_serial, uint8([16 nicfg.scheduler.ntrains]));
    
    % Manual override
    fwrite(nicfg.arduino_serial, uint8([17 nicfg.scheduler.manualoverride]));
    
    % Listen mode
    fwrite(nicfg.arduino_serial, uint8([27 nicfg.scheduler.listenmode]));
    
    % Listen mode polarity
    fwrite(nicfg.arduino_serial, uint8([28 nicfg.scheduler.listenpol]));
else
    % No Scheduler
    fwrite(nicfg.arduino_serial, uint8([15 0]));
end

%% Opto locked TTL
if nicfg.optodelayTTL.enable
    % Opto locked TTL
    fwrite(nicfg.arduino_serial, uint8([24 1]));
    
    % Delay after opto train onsets
    fwrite(nicfg.arduino_serial, uint8([18 nicfg.optodelayTTL.delay]));
    
    % Pulse width
    fwrite(nicfg.arduino_serial, uint8([19 nicfg.optodelayTTL.pulsewidth]));
    
    % Pulse cycle
    fwrite(nicfg.arduino_serial, uint8([20 nicfg.optodelayTTL.cycle]));
    
    % Train length
    fwrite(nicfg.arduino_serial, uint8([21 nicfg.optodelayTTL.trainlength]));
    
    % Buzzer
    fwrite(nicfg.arduino_serial, uint8([30 nicfg.optodelayTTL.buzzerenable]));
    
    % Buzzer delay
    fwrite(nicfg.arduino_serial, uint8([31 nicfg.optodelayTTL.buzzerdelay]));
    
    % Buzzer duration
    fwrite(nicfg.arduino_serial, uint8([32 nicfg.optodelayTTL.buzzerdur]));
    
    % Conditional
    fwrite(nicfg.arduino_serial, uint8([22 nicfg.optodelayTTL.conditional]));
    
    % Action period delay
    fwrite(nicfg.arduino_serial, uint8([33 nicfg.optodelayTTL.actiondelay]));
    
    % Action period duration
    fwrite(nicfg.arduino_serial, uint8([34 nicfg.optodelayTTL.actiondur]));
    
    % Delivery period duration
    fwrite(nicfg.arduino_serial, uint8([35 nicfg.optodelayTTL.deliverydur]));
else
    fwrite(nicfg.arduino_serial, uint8([24 0]));
end

%% Encoder
if nicfg.encoder.enable
    % Encoder
    fwrite(nicfg.arduino_serial, uint8([23 1]));
else
    fwrite(nicfg.arduino_serial, uint8([23 0]));
end

%% Audio sync
if nicfg.audiosync.enable
    % Audio enable
    fwrite(nicfg.arduino_serial, uint8([25 1]));
    
    % Frequency
    fwrite(nicfg.arduino_serial, uint8([26 nicfg.audiosync.freq]));
else
    fwrite(nicfg.arduino_serial, uint8([25 0]));
end

end

