function [runningdata, runningindex, schedulerinfo, rnginfo, updateui] =...
    omniserialparse(d, runningdata, runningindex, schedulerinfo, rnginfo)
% Function for parsing serial data (4 bytes from omnibox);

if isempty(d)
    updateui = false;
    return;
end

d4 = typecast(int32(d), 'uint8');

if d4(4) == 1
    % Scheduler (always sent before RNG)
    schedulerinfo = d4;
    updateui = false;

elseif d4(4) == 2
    % RNG (always sent following scheduler)
    rnginfo = d4;
    updateui = true;
    
else
    % Running
    runningindex = runningindex + 1;
    runningdata(runningindex) = d;
    updateui = false;
end

end