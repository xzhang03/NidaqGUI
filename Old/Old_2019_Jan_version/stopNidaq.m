function stopNidaq(open_daq, channelnames)
%UNTITLED12 Summary of this function goes here
%   Detailed explanation goes here

    open_daq.daq_connection.stop;
    delete(open_daq.listener);
    fclose(open_daq.logfile);
    
    fp = fopen(open_daq.logpath, 'r');
    [data, ~] = fread(fp, [open_daq.nchannels+1, inf], 'double'); %#ok<ASGLU>
    fclose(fp);
    
    timestamps = data(1, :);
    data = data(2:end, :);
    
    Fs = open_daq.frequency;
    frequency = open_daq.frequency;
    save(open_daq.path, 'data', 'Fs', 'frequency', 'timestamps', 'channelnames');
    
    delete(open_daq.logpath);
end

