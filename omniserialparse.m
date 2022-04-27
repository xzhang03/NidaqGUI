function [runningdata, runningindex, schedulerinfo, rnginfo] =...
    omniserialparse(d, runningdata, runningindex, schedulerinfo, rnginfo)
% Function for parsing serial data (4 bytes from omnibox);

if isempty(d)
    return;
end

d4 = typecast(int32(d), 'uint8');

if d4(4) == 1
    % Scheduler
    schedulerinfo = d4;
    
elseif d4(4) == 2
    % RNG
    rnginfo = d4;
else
    % Running
    runningindex = runningindex + 1;
    runningdata(runningindex) = d;
end

end