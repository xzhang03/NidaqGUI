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
    
    % TCP behavioral cycle
    fwrite(nicfg.arduino_serial, uint8([47 nicfg.tcp.behaviorcycle]));
    
    % Disp
    disp('Mode -> TCP');
end

%% Optophotometry
if nicfg.optophotometry.enable
    % Mode
    disp('Mode -> Optophotometry');
    
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
    % Mode
    disp('Mode -> Same-color optophotometry');
    
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
    
    % Tristate pin polarity (protected)
    % Tristate pin polarity (do not change once a box is made).
    if isfield(nicfg.scoptophotometry, 'tristatepol')
        fwrite(nicfg.arduino_serial, uint8([29 nicfg.scoptophotometry.tristatepol]));
    end
end

%% Scheduler
if nicfg.scheduler.enable
    % Mode
    disp('Scheduler -> On');
    
    % Scheduler
    fwrite(nicfg.arduino_serial, uint8([15 1]));
    
    % Delay (passing time as units of 10 seconds)
    fwrite(nicfg.arduino_serial, uint8([4 nicfg.scheduler.delay / 10]));
    if ceil(nicfg.scheduler.delay / 10) ~= (nicfg.scheduler.delay / 10)
        % Delay is updated
        fprintf('Delay is updated to %i instead.\n', floor(nicfg.scheduler.delay/10)*10);
    end
    
    % Number of trains (increments of 10)
    b = floor(nicfg.scheduler.ntrains / 256);
    a = nicfg.scheduler.ntrains - b * 256;
    fwrite(nicfg.arduino_serial, uint8([16 a]));
    fwrite(nicfg.arduino_serial, uint8([46 b]));
    
    % Manual override
    fwrite(nicfg.arduino_serial, uint8([17 nicfg.scheduler.manualoverride]));
    
    % Listen mode
    fwrite(nicfg.arduino_serial, uint8([27 nicfg.scheduler.listenmode]));
    if (nicfg.scheduler.listenmode && ~nicfg.optodelayTTL.optothenTTL)
        % Trying to do listen mode and food->opto
        nicfg.optodelayTTL.optothenTTL = true;
        fprintf('Listen mode forces the seuqnce of opto -> TTL.\n');
    end
    
    % Listen mode polarity (protected)
    % Listen mode polarity (true = active high, false = active low). Do not change unless you know what you are doing.
    if isfield(nicfg.scheduler, 'listenpol')
        fwrite(nicfg.arduino_serial, uint8([28 nicfg.scheduler.listenpol]));
    end
    
    % Use RNG to determine if opto goes through or not
    if nicfg.scheduler.control
        % Control experiment
        % use opto RNG
        fwrite(nicfg.arduino_serial, uint8([38 1]));

        % RNG pass chance
        fwrite(nicfg.arduino_serial, uint8([39 0]));
    else
        % use opto RNG
        fwrite(nicfg.arduino_serial, uint8([38 nicfg.scheduler.useRNG]));

        % RNG pass chance
        fwrite(nicfg.arduino_serial, uint8([39 nicfg.scheduler.passchance]));
    end
    
    % Randomize ITI
    fwrite(nicfg.arduino_serial, uint8([40 nicfg.scheduler.randomITI]));
    
    % Randomize ITI min seconds
    fwrite(nicfg.arduino_serial, uint8([41 nicfg.scheduler.randomITI_min]));
    
    % Randomize ITI max seconds
    fwrite(nicfg.arduino_serial, uint8([42 nicfg.scheduler.randomITI_max]));
else
    % No Scheduler
    fwrite(nicfg.arduino_serial, uint8([15 0]));
    disp('Scheduler -> Off');
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
    
    % Sequence of opto and TTL (default: opto-> TTL)
    if isfield(nicfg.optodelayTTL, 'optothenTTL')
        fwrite(nicfg.arduino_serial, uint8([48 nicfg.optodelayTTL.optothenTTL])); % True: opto->TTL, TTL->opto
        
        if (~nicfg.optodelayTTL.optothenTTL)
            % Lead
            % How many seconds is the food TTL armed before an opto train.
            % The other delays above still stands. This just sets when the
            % sequence of cue/action/reward starts.
            fwrite(nicfg.arduino_serial, uint8([49 nicfg.optodelayTTL.lead])); 
        end
    end
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

% Auto echo
fwrite(nicfg.arduino_serial, uint8([43 nicfg.encoder.autoecho]));

%% Audio sync
if nicfg.audiosync.enable
    % Audio enable
    fwrite(nicfg.arduino_serial, uint8([25 1]));
    
    % Frequency
    fwrite(nicfg.arduino_serial, uint8([26 nicfg.audiosync.freq]));
else
    fwrite(nicfg.arduino_serial, uint8([25 0]));
end

%% Auto-echo of trial and RNG info
if nicfg.onlineecho.enable
    % Enable
    fwrite(nicfg.arduino_serial, uint8([44 1]));
    
    % Periodicity
    fwrite(nicfg.arduino_serial, uint8([45 nicfg.onlineecho.periodicity]));
end

end

