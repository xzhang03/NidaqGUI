function open_daq = startNidaq(path, frequency, nchannels, digitalchannelstr)
%UNTITLED11 Summary of this function goes here
%   Detailed explanation goes here

    if nargin < 2, frequency = 2000; end
    if nargin < 3, nchannels = 6; end
    
    [basepath, file, ~] = fileparts(path);
    logpath = fullfile(basepath, [file '-log.bin']);
    logfile = fopen(logpath, 'w');

    daq_connection = daq.createSession('ni'); %create a session in 64 bit
    daq_connection.Rate = frequency; 
    daq_connection.IsContinuous = true;

    ai = daq_connection.addAnalogInputChannel('Dev1', 0:(nchannels-1) , 'Voltage'); 
    for i = 1:nchannels
        ai(i).Range = [-10 10];
    end
    
    if ~isempty(digitalchannelstr) 
        daq_connection.addDigitalChannel('Dev1', digitalchannelstr, 'InputOnly');
    end
        
    listener = daq_connection.addlistener('DataAvailable', @(src, event)logData(src, event, logfile));
    daq_connection.startBackground;
    
    open_daq = struct( ...
        'logfile', logfile, ...
        'listener', listener, ...
        'daq_connection', daq_connection, ...
        'logpath', logpath, ...
        'frequency', frequency, ...
        'nchannels', nchannels, ...
        'path', path ...
    );

    if ~isempty(digitalchannelstr) 
        open_daq.nchannels = open_daq.nchannels + 1;
    end 
end

