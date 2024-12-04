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

%% Firmware version
arduinoWrite(nicfg.arduino_serial, [254 0]);
pause(0.3);
if arduinoGetBytes(nicfg.arduino_serial) > 20
    ver = arduinoRead(nicfg.arduino_serial, arduinoGetBytes(nicfg.arduino_serial), 'uint8');
end
fprintf('Nanosec firmware version: %s\n', char(ver'));

%% Check conflict
if (nicfg.tcp.enable + nicfg.optophotometry.enable + nicfg.scoptophotometry.enable) ~= 1
    msgbox('Must select 1 and only 1 from Tcp, Optophotometry, and Same-color optophotometry modes.')
end

if (nicfg.audiosync.enable && nicfg.optodelayTTL.buzzerenable)
    msgbox('Cannot use audio sync and buzzer cue at the same time.')
end

if (nicfg.optodelayTTL.ntrialtypes > 1 && nicfg.scheduler.manualoverride)
    msgbox('External trigger and multi trialtypes are not compatible at the moment.')
end

%% Backward compatibility
% Buzzer => Cue
if ~isfield(nicfg.optodelayTTL, 'cueenable')
    nicfg.optodelayTTL.cueenable = nicfg.optodelayTTL.buzzerenable;
    nicfg.optodelayTTL.cuedelay = nicfg.optodelayTTL.buzzerdelay;
    nicfg.optodelayTTL.cuedur = nicfg.optodelayTTL.buzzerdur;
    nicfg.optodelayTTL = rmfield(nicfg.optodelayTTL, {'buzzerenable', 'buzzerdelay', 'buzzerdur'});
end

% Multiple trial types
if ~isfield(nicfg.optodelayTTL, 'ntrialtypes')
   nicfg.optodelayTTL.ntrialtypes = 1;
end

%% TCP
if nicfg.tcp.enable
    % Pulse cycle 1
    if isfield(nicfg.tcp, 'pulsecycle1')
        b = floor(nicfg.tcp.pulsecycle1 / 256);
        a = nicfg.tcp.pulsecycle1 - b * 256;
        arduinoWrite(nicfg.arduino_serial, [53 a]);
        if b > 0
            arduinoWrite(nicfg.arduino_serial, [57 b]);
        end
    end
    
    % Pulse cycle 2
    if isfield(nicfg.tcp, 'pulsecycle2')
        b = floor(nicfg.tcp.pulsecycle2 / 256);
        a = nicfg.tcp.pulsecycle2 - b * 256;
        arduinoWrite(nicfg.arduino_serial, [54 a]);
        if b > 0
            arduinoWrite(nicfg.arduino_serial, [58 b]);
        end
    end
    
    % TCP behavioral cycle
    arduinoWrite(nicfg.arduino_serial, [47 nicfg.tcp.behaviorcycle]);
        
    % TCP mode switch (do this at the end to modeswitch to correct
    % timings)
    arduinoWrite(nicfg.arduino_serial, [3 0]);

    % Disp
    disp('Mode -> TCP');
end

%% Optophotometry
if nicfg.optophotometry.enable
    % Mode
    disp('Mode -> Optophotometry');
    
    % Pulse cycle 1
    if isfield(nicfg.optophotometry, 'pulsecycle1')
        b = floor(nicfg.optophotometry.pulsecycle1 / 256);
        a = nicfg.optophotometry.pulsecycle1 - b * 256;
        arduinoWrite(nicfg.arduino_serial, [36 a]);
        if b > 0
            arduinoWrite(nicfg.arduino_serial, [55 b]);
        end
    end
    
    % Pulse cycle 2
    if isfield(nicfg.optophotometry, 'pulsecycle2')
        b = floor(nicfg.optophotometry.pulsecycle2 / 256);
        a = nicfg.optophotometry.pulsecycle2 - b * 256;
        arduinoWrite(nicfg.arduino_serial, [37 a]);
        if b > 0
            arduinoWrite(nicfg.arduino_serial, [56 b]);
        end
    end
    
    % Frequency
    arduinoWrite(nicfg.arduino_serial, [6 nicfg.optophotometry.freqmod]);
    
    % Train length
    arduinoWrite(nicfg.arduino_serial, [7 nicfg.optophotometry.trainlength]);
    
    % Train cycle
    arduinoWrite(nicfg.arduino_serial, [8 nicfg.optophotometry.cycle]);
    
    % Pulse width
    arduinoWrite(nicfg.arduino_serial, [14 nicfg.optophotometry.pulsewidth]);
    
    % Overlap
    if isfield(nicfg.optophotometry, 'overlap')
        arduinoWrite(nicfg.arduino_serial, [59 nicfg.optophotometry.overlap]);
    end
    
    % Optophotometry mode switch (do this at the end to modeswitch to correct
    % timings)
    arduinoWrite(nicfg.arduino_serial, [3 1]);
end

%% Same-color optophotometry
if nicfg.scoptophotometry.enable
    % Mode
    disp('Mode -> Same-color optophotometry');
    
    % Frequency
    arduinoWrite(nicfg.arduino_serial, [10 nicfg.scoptophotometry.freqmod]);
    
    % Train length
    arduinoWrite(nicfg.arduino_serial, [12 nicfg.scoptophotometry.trainlength]);
    
    % Train cycle
    arduinoWrite(nicfg.arduino_serial, [11 nicfg.scoptophotometry.cycle]);
    
    % Pulse width
    arduinoWrite(nicfg.arduino_serial, [13 nicfg.scoptophotometry.pulsewidth]);
    
    % Tristate pin polarity (protected)
    % Tristate pin polarity (do not change once a box is made).
    if isfield(nicfg.scoptophotometry, 'tristatepol')
        arduinoWrite(nicfg.arduino_serial, [29 nicfg.scoptophotometry.tristatepol]);
    end

    % SC Optophotometry mode switch (do this at the end to modeswitch to correct
    % timings)
    arduinoWrite(nicfg.arduino_serial, [3 2]);
end

%% Scheduler
if nicfg.scheduler.enable
    % Mode
    disp('Scheduler -> On');
    
    % Delay (passing time in seconds)
    b = floor(nicfg.scheduler.delay / 256);
    a = nicfg.scheduler.delay - b * 256;
    arduinoWrite(nicfg.arduino_serial, [4 a]);
    arduinoWrite(nicfg.arduino_serial, [52 b]);
    
    % Number of trains (increments of 10)
    b = floor(nicfg.scheduler.ntrains / 256);
    a = nicfg.scheduler.ntrains - b * 256;
    arduinoWrite(nicfg.arduino_serial, [16 a]);
    arduinoWrite(nicfg.arduino_serial, [46 b]);
    
    % Manual override
    arduinoWrite(nicfg.arduino_serial, [17 nicfg.scheduler.manualoverride]);
    
    % Listen mode
    arduinoWrite(nicfg.arduino_serial, [27 nicfg.scheduler.listenmode]);
    if (nicfg.scheduler.listenmode && ~nicfg.optodelayTTL.optothenTTL)
        % Trying to do listen mode and food->opto
        nicfg.optodelayTTL.optothenTTL = true;
        fprintf('Listen mode forces the seuqnce of opto -> TTL.\n');
    end
    
    % Listen mode polarity (protected)
    % Listen mode polarity (true = active high, false = active low). Do not change unless you know what you are doing.
    if isfield(nicfg.scheduler, 'listenpol')
        arduinoWrite(nicfg.arduino_serial, [28 nicfg.scheduler.listenpol]);
    end
    
    % Use RNG to determine if opto goes through or not
    if nicfg.scheduler.control
        % Control experiment
        % use opto RNG
        arduinoWrite(nicfg.arduino_serial, [38 1]);

        % RNG pass chance
        arduinoWrite(nicfg.arduino_serial, [39 0]);
    else
        % use opto RNG
        arduinoWrite(nicfg.arduino_serial, [38 nicfg.scheduler.useRNG]);

        % RNG pass chance
        arduinoWrite(nicfg.arduino_serial, [39 nicfg.scheduler.passchance]);
    end
    
    % Randomize ITI
    arduinoWrite(nicfg.arduino_serial, [40 nicfg.scheduler.randomITI]);
    
    % Randomize ITI min seconds
    arduinoWrite(nicfg.arduino_serial, [41 nicfg.scheduler.randomITI_min]);
    
    % Randomize ITI max seconds
    arduinoWrite(nicfg.arduino_serial, [42 nicfg.scheduler.randomITI_max]);
    
    % Scheduler indicator
    if isfield(nicfg.scheduler, 'indicator')
        if nicfg.scheduler.indicator.enable
            % Enable
            arduinoWrite(nicfg.arduino_serial, [69 1]);
    
            % Colors
            preopto_color = uint8(nicfg.scheduler.indicator.preopto);
            inopto_color = uint8(nicfg.scheduler.indicator.inopto);
            postopto_color = uint8(nicfg.scheduler.indicator.postopto);
    
            preopto_color_byte = ...
                bitshift(preopto_color(1),4) + bitshift(preopto_color(2),2) + preopto_color(3) + bitshift(preopto_color(4),6);
            inopto_color_byte = ...
                bitshift(inopto_color(1),4) + bitshift(inopto_color(2),2) + inopto_color(3) + bitshift(inopto_color(4),6);
            postopto_color_byte = ...
                bitshift(postopto_color(1),4) + bitshift(postopto_color(2),2) + postopto_color(3) + bitshift(postopto_color(4),6);
            
            arduinoWrite(nicfg.arduino_serial, [70 preopto_color_byte]);
            arduinoWrite(nicfg.arduino_serial, [71 inopto_color_byte]);
            arduinoWrite(nicfg.arduino_serial, [72 postopto_color_byte]);
            arduinoWrite(nicfg.arduino_serial, [73 nicfg.scheduler.indicator.switchoffmode]);
        else
            % Disable
            arduinoWrite(nicfg.arduino_serial, [69 0]);
        end
    else
        % Disable
        arduinoWrite(nicfg.arduino_serial, [69 0]);
    end

    % Scheduler
    arduinoWrite(nicfg.arduino_serial, [15 1]);
else
    % No Scheduler
    arduinoWrite(nicfg.arduino_serial, [15 0]);
    disp('Scheduler -> Off');
end

%% Behavior
if nicfg.optodelayTTL.enable
    % Mode
    disp('Behavior -> On');
    
    % Opto locked TTL
    arduinoWrite(nicfg.arduino_serial, [24 1]);
    
    % Delay after opto train onsets
    b = floor(nicfg.optodelayTTL.delay / 256);
    a = nicfg.optodelayTTL.delay - b * 256;
    arduinoWrite(nicfg.arduino_serial, [18 a]);
    arduinoWrite(nicfg.arduino_serial, [50 b]);
    
    % Multi trial type
    arduinoWrite(nicfg.arduino_serial, [62 nicfg.optodelayTTL.ntrialtypes]);
    fprintf('Trial types -> %i\n', nicfg.optodelayTTL.ntrialtypes);
    
    % Iterating trial types
    for i = 1 : nicfg.optodelayTTL.ntrialtypes
        % Current trial to edit
        arduinoWrite(nicfg.arduino_serial, [63 i-1]);
        
        % Trial frequency weight
        arduinoWrite(nicfg.arduino_serial, [67 nicfg.optodelayTTL.trialfreq(i)]);
        
        % Pulse width
        arduinoWrite(nicfg.arduino_serial, [19 nicfg.optodelayTTL.pulsewidth(i)]);
        
        % Pulse cycle
        arduinoWrite(nicfg.arduino_serial, [20 nicfg.optodelayTTL.cycle(i)]);
        
        % Train length
        arduinoWrite(nicfg.arduino_serial, [21 nicfg.optodelayTTL.trainlength(i)]);
        
        % Buzzer duration
        arduinoWrite(nicfg.arduino_serial, [32 nicfg.optodelayTTL.cuedur(i)]);
        
        % Delivery period duration
        arduinoWrite(nicfg.arduino_serial, [35 nicfg.optodelayTTL.deliverydur(i)]);
        
        % Conditional
        arduinoWrite(nicfg.arduino_serial, [22 nicfg.optodelayTTL.conditional(i)]);
        
        % Constructing Trial IO (a single uint16 integer to specify input/output pins a trial type)
        trialinfo = nicfg.optodelayTTL.(sprintf('type%i', i));
        trialio = uint16(0);
        switch trialinfo.cuetype
            case 'Buzzer'
                trialio = bitset(trialio, 16);
                
            case 'DIO'
                trialio = bitset(trialio, 15);
                if any(trialinfo.RGB > 1)
                    trialinfo.RGB(trialinfo.RGB > 1) = 1;
                    msgbox('DIO color intensity is capped at 1');
                end
                trialio = ... % 3 or 4 Channel binary info (DO7, DO6, DO5, DO4)
                    trialio + bitshift(trialinfo.RGB(1), 12)...
                    + bitshift(trialinfo.RGB(2), 11) + bitshift(trialinfo.RGB(3), 10); 
                if length(trialinfo.RGB) == 4
                    trialio = trialio + bitshift(trialinfo.RGB(4), 9);
                end
                
            case 'PWMRGB'
                trialio = bitset(trialio, 14);
                if any(trialinfo.RGB > 7)
                    trialinfo.RGB(trialinfo.RGB > 7) = 7;
                    msgbox('RGB color intensity is capped at 7');
                end
                trialio = trialio + bitshift(trialinfo.RGB(1), 10)...
                    + bitshift(trialinfo.RGB(2), 7) + bitshift(trialinfo.RGB(3), 4); % RGB
        end
        
        % Polarity of action detection (normally active high). Only not set
        % if actionpol is false
        if ~isfield(trialinfo, 'actionpol')
            trialio = bitset(trialio, 4);
        elseif trialinfo.actionpol
            trialio = bitset(trialio, 4);
        end
        
        % Reward delivery
        switch trialinfo.rewardtype
            case 'Native'
                % Nothing
            case 'DIO'
                trialio = bitset(trialio, 3);
        end
        trialio = trialio + trialinfo.DIOport;
        
        % Write upper byte and then lower byte
        arduinoWrite(nicfg.arduino_serial, [65 bitshift(trialio, -8)]);
        arduinoWrite(nicfg.arduino_serial, [66 bitand(trialio, 255)]);
    end
    
    % Variables that are unchanged in multi trialtypes
    % Cue
    arduinoWrite(nicfg.arduino_serial, [30 nicfg.optodelayTTL.cueenable]);
    
    % Cue delay
    arduinoWrite(nicfg.arduino_serial, [31 nicfg.optodelayTTL.cuedelay]);
    
    % Action period delay
    arduinoWrite(nicfg.arduino_serial, [33 nicfg.optodelayTTL.actiondelay]);
    
    % Action period duration
    arduinoWrite(nicfg.arduino_serial, [34 nicfg.optodelayTTL.actiondur]);
    
    % Sequence of opto and TTL (default: opto-> TTL)
    if isfield(nicfg.optodelayTTL, 'optothenTTL')
        arduinoWrite(nicfg.arduino_serial, [48 nicfg.optodelayTTL.optothenTTL]); % True: opto->TTL, TTL->opto

        if (~nicfg.optodelayTTL.optothenTTL)
            % Lead
            % How many seconds is the food TTL armed before an opto train.
            % The other delays above still stands. This just sets when the
            % sequence of cue/action/reward starts.
            b = floor(nicfg.optodelayTTL.lead / 256);
            a = nicfg.optodelayTTL.lead - b * 256;
            arduinoWrite(nicfg.arduino_serial, [49 a]);
            arduinoWrite(nicfg.arduino_serial, [51 b]);
        end
    else
        nicfg.optodelayTTL.optothenTTL = true;
        arduinoWrite(nicfg.arduino_serial, [48 nicfg.optodelayTTL.optothenTTL]); % True: opto->TTL, TTL->opto
    end
else
    % Mode
    disp('Behavior -> Off');
    
    arduinoWrite(nicfg.arduino_serial, [24 0]);
end

%% Encoder
if nicfg.encoder.enable
    % Encoder
    arduinoWrite(nicfg.arduino_serial, [23 1]);
else
    arduinoWrite(nicfg.arduino_serial, [23 0]);
end

% Auto echo
arduinoWrite(nicfg.arduino_serial, [43 nicfg.encoder.autoecho]);

%% Audio sync
if nicfg.audiosync.enable
    % Audio enable
    arduinoWrite(nicfg.arduino_serial, [25 1]);
    
    % Frequency
    arduinoWrite(nicfg.arduino_serial, [26 nicfg.audiosync.freq]);
else
    arduinoWrite(nicfg.arduino_serial, [25 0]);
end

%% Auto-echo of trial and RNG info
if nicfg.onlineecho.enable
    % Enable
    arduinoWrite(nicfg.arduino_serial, [44 1]);
    
    % Periodicity
    arduinoWrite(nicfg.arduino_serial, [45 nicfg.onlineecho.periodicity]);
else
    % Disable
    arduinoWrite(nicfg.arduino_serial, [44 0]);
end

end

