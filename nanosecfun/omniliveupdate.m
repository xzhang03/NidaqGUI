function str = omniliveupdate(echoscheduler, echoRNG)
    % Parse value from teensy for live update on UI
    itrain = echoscheduler(1);
    ntrain = echoscheduler(2);
    if echoscheduler(3) == 1
        str = 'No scheduler';
    elseif echoscheduler(3) == 2
        str = sprintf('Scheduler: Pre-opto %i/%i', itrain, ntrain);
    elseif echoscheduler(3) == 3
        str = sprintf('Scheduler: Post-opto %i/%i', itrain, ntrain);
    else
        str = sprintf('Scheduler: %i/%i', itrain, ntrain);
    end
    
    if echoRNG(3) == 0
        strRNG = 'RNG: off';
    else
        strRNG = sprintf("RNG: %i", echoRNG(1));
    end
    
    str = sprintf('%s. %s', str, strRNG);
end