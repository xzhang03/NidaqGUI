function stopNidaq_ML(open_daq, nidaqpath)
% stopNidaq_ML uses the MonkeyLogic DAQ toolbox to stop nidaq

% Stop recording
stop(open_daq);

% Collect data
[data,time,~,~] = getdata(open_daq, open_daq.SamplesAvailable);

% Rearrange info
data = data';
timestamps = time';

% Common analysis variables
Fs = open_daq.SampleRate;
frequency = open_daq.SampleRate;

save(nidaqpath, 'data', 'timestamps', 'Fs', 'frequency');
end