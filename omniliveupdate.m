function str = omniliveupdate(echoscheduler, echoRNG)
    % Parse value from teensy for live update on UI
    if echoscheduler == 65536
        str = 'No scheduler';
    elseif echoscheduler == 65537
        str = 'Scheduler: Pre-opto';
    elseif echoscheduler == 65538
        str = 'Scheduler: Post-opto';
    else
        itrain = mod(echoscheduler, 255);
        ntrain = (echoscheduler - itrain) / 255;
        str = sprintf("Scheduler: %i/%i", itrain, ntrain);
    end
    
    if echoRNG == 65536
        strRNG = 'RNG: off';
    else
        strRNG = sprintf("RNG: %i", itrain);
    end
    
    str = sprintf('%s. %s', str, strRNG);
end