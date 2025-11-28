function str = omniliveupdate(echoscheduler, echoRNG)
    % Parse value from teensy for live update on UI
    itrain = echoscheduler(1);
    ntrain = echoscheduler(2);
    if echoscheduler(3) == 1
        str = 'No scheduler';
    elseif echoscheduler(3) == 2
        str = sprintf('Pre-opto %i/%i', itrain, ntrain);
    elseif echoscheduler(3) == 3
        str = sprintf('Post-opto %i/%i', itrain, ntrain);
    else
        str = sprintf('Trial: %i/%i', itrain, ntrain);
    end
    
    % Multi Trial type
    strType = sprintf("T%i", echoRNG(2)+1);
    
    % RNG
    if echoRNG(3) == 0 % No RNG
        strRNG = '';
    elseif echoRNG(3) == 1 % opto RNG
        strRNG = sprintf("RNG: %i", echoRNG(1));
    else
        % echoRNG(3)== 2 means food RNG
        strRNG = sprintf("FoodRNG: %i", echoRNG(1));
    end
    
    str = sprintf('%s. %s. %s', str, strType, strRNG);
end