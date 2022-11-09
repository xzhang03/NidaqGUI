function nidaqfixlog(defaultpath, recordingmode)
% Converts log files into standard photometry mat files. 
% nidaqfixlog(defaultpath, recordingmode)
% *recordingmode could be 'lab' or 'sz'
if nargin < 2
    recordingmode = 'sz';
    if nargin < 1
        defaultpath = '\\anastasia\data\photometry\';
    end
end

% Get config
switch recordingmode
    case 'lab'
        nidaq_config;
    case 'sz'
        [fn_config, fp_config] = uigetfile(fullfile(defaultpath,'.m'), 'Select config file.');
        configfp = fullfile(fp_config,fn_config);
        run(configfp);
        
        % Mixed channels
        if iscell(nicfg.AImode)
            nanalogch = size(nicfg.AImode,1);
        else
            nanalogch = nicfg.NidaqChannels;
        end
end

% Get file
[fn, fp] = uigetfile(fullfile(defaultpath,'.bin'), 'Select data file.');

% Output file
fn_out = sprintf('%s.mat',fn(1:end-8));

% Read file
f1 = fopen(fullfile(fp,fn), 'r');
[data, ~] = fread(f1, [(nanalogch + nicfg.NidaqDigitalChannels +1), inf], 'double'); %#ok<ASGLU>
fclose(f1);


% Restructure data
timestamps = data(1, :);
data = data(2:end, :);

% Fill
Fs = nicfg.NidaqFrequency;
frequency = nicfg.NidaqFrequency;
channelnames = nicfg.ChannelNames;
omnisetting = nicfg;


% Save
disp('Saving...');
if ~exist(fullfile(fp,fn_out), 'file') || ...
        input('Nidaq file already exist. Overwrite? (1 = Yes, 0 = No): ') == 1
    
    switch recordingmode
        case 'lab'
            save(fullfile(fp,fn_out), 'data', 'Fs', 'frequency', 'timestamps', 'channelnames');
        case 'sz'
            save(fullfile(fp,fn_out), 'data', 'Fs', 'frequency', 'timestamps', 'channelnames', 'omnisetting', 'configfp');
    end
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