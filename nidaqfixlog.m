function nidaqfixlog(defaultpath, recordingmode)
% Converts log files into standard photometry mat files. 
% nidaqfixlog(defaultpath, recordingmode)
% *recordingmode could be 'lab' or 'sz'
if nargin < 2
    recordingmode = 'lab';
    if nargin < 1
        defaultpath = '\\anastasia\data\photometry\';
    end
end

% Get config
switch recordingmode
    case 'lab'
        nidaq_config;
    case 'sz'
        nidaq_config_sz;
end

% Get file
[fn, fp] = uigetfile(fullfile(defaultpath,'.bin'), 'Select data file.');

% Output file
fn_out = sprintf('%s.mat',fn(1:end-8));

% Read file
f1 = fopen(fullfile(fp,fn), 'r');
[data, ~] = fread(f1, [(nicfg.NidaqChannels + nicfg.NidaqDigitalChannels +1), inf], 'double'); %#ok<ASGLU>
fclose(f1);

% Restructure data
timestamps = data(1, :);
data = data(2:end, :);

% Fill
Fs = nicfg.NidaqFrequency;
frequency = nicfg.NidaqFrequency;
channelnames = nicfg.ChannelNames;

% Save
disp('Saving...');
if ~exist(fullfile(fp,fn_out), 'file')
    save(fullfile(fp,fn_out), 'data', 'Fs', 'frequency', 'timestamps', 'channelnames');
    saved = true;
elseif input('Nidaq file already exist. Overwrite? (1 = Yes, 0 = No): ') == 1
    save(fullfile(fp,fn_out), 'data', 'Fs', 'frequency', 'timestamps', 'channelnames');
    saved = true;
else
    saved = false;
end
if saved && strcmp(fp(1:2),'\\')
    disp('Done. Data saved to server.')
elseif saved
    disp('Done. Please copy data to server.');
else
    disp('Done. Data did not save.')
end
end