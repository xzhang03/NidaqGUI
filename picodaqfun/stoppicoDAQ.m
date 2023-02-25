function stoppicoDAQ(openDAQ, channelnames, omniboxsetting, configfp)
% Stop a picoDAQ session

if nargin < 4
    configfp = '';
    if nargin < 3
        omniboxsetting = [];
    end
end

%% Stop running
write(openDAQ.picodaq_serial, [0 0], 'uint8');
flush(openDAQ.picodaq_serial,"input");
configureCallback(openDAQ.picodaq_serial, 'off');
delete(openDAQ.picodaq_serial);
fclose(openDAQ.logfile);


%% Read the whole log file in
fp = fopen(openDAQ.logpath, 'r');
[data, ~] = fread(fp, [6, inf], 'int32'); %#ok<ASGLU>
fclose(fp);


%% Parse
% Get timestamps out
timestamps = data(1, :) / openDAQ.frequency;
datad = uint32(data(2, :));
dataa = data(3:end,:);
n = length(timestamps);

% Initialize
data = zeros((openDAQ.nchannels + openDAQ.ndigitals), n);

% Analog
adivider = 2 ^ 23;
for i = 1 : openDAQ.nchannels
    data(i,:) =  dataa(i,:) / adivider * 1.2 * openDAQ.pidaq_params.fda;
end

% Digital
for i = 1 : openDAQ.ndigitals
    data(i+openDAQ.nchannels,:) =  bitget(datad, openDAQ.ndigitals - i + 1);
end

%% Save
Fs = openDAQ.frequency;
frequency = openDAQ.frequency;
save(openDAQ.path, 'data', 'Fs', 'frequency', 'timestamps', 'channelnames', 'omniboxsetting', 'configfp');

delete(openDAQ.logpath);
end

