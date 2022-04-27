channel2look = 3;

pulses = chainfinder(data(channel2look,:) > 1);

lpulse = size(pulses,1);
lrun = length(speed);
lpulse/lrun

