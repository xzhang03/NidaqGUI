function stopNidaq(open_daq, channelnames, omniboxsetting, configfp, starttime)
%UNTITLED12 Summary of this function goes here
%   Detailed explanation goes here
    if nargin < 5
        starttime = '';
        if nargin < 4
            configfp = '';
            if nargin < 3
                omniboxsetting = [];
            end
        end
    end
    
    open_daq.daq_connection.stop;
    delete(open_daq.listener);
    fclose(open_daq.logfile);
    
    % Mixed channels
    if ~isempty(omniboxsetting) && iscell(omniboxsetting.AImode)
        nanalogch = size(omniboxsetting.AImode,1);
    else
        nanalogch = open_daq.nchannels;
    end
            
    fp = fopen(open_daq.logpath, 'r');
    [data, ~] = fread(fp, [(nanalogch + open_daq.ndigitals)*open_daq.ndevs + 1, inf], 'double'); %#ok<ASGLU>
    fclose(fp);
    
    timestamps = data(1, :);
    data = data(2:end, :);
    
    Fs = open_daq.frequency;
    frequency = open_daq.frequency;
    save(open_daq.path, 'data', 'Fs', 'frequency', 'timestamps', 'channelnames', 'omniboxsetting', 'configfp', 'starttime');
    
    delete(open_daq.logpath);
end

