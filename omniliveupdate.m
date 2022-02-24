function str = omniliveupdate(echo)
% Parse value from teensy for live update on UI
if echo == 65536
    str = 'No scheduler';
elseif echo == 65537
    str = 'Scheduler: Pre-opto';
elseif echo == 65538
    str = 'Scheduler: Post-opto phase';
else
    itrain = mod(echo, 255);
    ntrain = (echo - itrain) / 255;
    str = sprintf("Scheduler: %i/%i", itrain, ntrain);
end
end