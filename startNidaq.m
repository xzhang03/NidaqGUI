function open_daq = startNidaq(path, frequency, nchannels, digitalchannelstr, ndigital, daqname)
%UNTITLED11 Summary of this function goes here
%   Detailed explanation goes here

    if nargin < 2, frequency = 2000; end
    if nargin < 3, nchannels = 6; end
    if nargin < 6, daqname = 'Dev1'; end
        
    [basepath, file, ~] = fileparts(path);
    logpath = fullfile(basepath, [file '-log.bin']);
    logfile = fopen(logpath, 'w');

    daq_connection = daq.createSession('ni'); %create a session in 64 bit
    daq_connection.Rate = frequency; 
    daq_connection.IsContinuous = true;

    ai = daq_connection.addAnalogInputChannel(daqname, 0:(nchannels-1) , 'Voltage'); 
    for i = 1:nchannels
        ai(i).Range = [-10 10];
        ai(i).TerminalConfig = 'SingleEnded';
    end
    
    if ndigital > 0
        port_string = sprintf('%s%i', digitalchannelstr, 0);
        if ndigital > 1
            port_string = sprintf('%s:%i', port_string, ndigital - 1);
        end
        
        daq_connection.addDigitalChannel(daqname, port_string, 'InputOnly');
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
        'ndigitals', ndigital, ...
        'path', path ...
    );
end

