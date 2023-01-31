function open_daq = startNidaq(path, frequency, nchannels, digitalchannelstr, ndigital, daqnames, RecordingMode)
%UNTITLED11 Summary of this function goes here
%   Detailed explanation goes here
% Consider addTriggerConnection in the future
    
    modes = {'Differential', 'SingleEnded', 'SingleEndedNonReferenced'};
    
    if nargin < 2, frequency = 2000; end
    if nargin < 3, nchannels = 6; end
    if nargin < 6, daqnames = 'Dev1'; end
    if nargin < 7, RecordingMode = modes{2}; end
    if iscell(RecordingMode)
        mixedch = true;
        chvec = cell2mat(RecordingMode(:,1));
    else
        mixedch = false;
    end
        
    [basepath, file, ~] = fileparts(path);
    logpath = fullfile(basepath, [file '-log.bin']);
    logfile = fopen(logpath, 'w');

    daq_connection = daq.createSession('ni'); %create a session in 64 bit
    daq_connection.Rate = frequency; 
    daq_connection.IsContinuous = true;
    
    % Multidaq
    if ischar(daqnames)
        daqnames = {daqnames};
    end
    ndaqs = length(daqnames);

    for idaq = 1 : ndaqs
        % Current daq name
        daqname = daqnames{idaq};

        if mixedch
            ai = daq_connection.addAnalogInputChannel(daqname, chvec , 'Voltage'); % 0 indexed here
        else
            ai = daq_connection.addAnalogInputChannel(daqname, 0:(nchannels-1) , 'Voltage'); % 0 indexed here
        end
        for i = 1:nchannels
            % Setting up channel range and modes
            if mixedch
                j = chvec == (i - 1);
                if any(j)
                    ai(j).Range = [-10 10];
                    ai(j).TerminalConfig = RecordingMode{j,2};
                end
            else
                ai(i).Range = [-10 10];
                ai(i).TerminalConfig = RecordingMode;
            end
        end
        
        if ndigital > 0
            port_string = sprintf('%s%i', digitalchannelstr, 0);
            if ndigital > 1
                port_string = sprintf('%s:%i', port_string, ndigital - 1);
            end
            
            daq_connection.addDigitalChannel(daqname, port_string, 'InputOnly');
        end
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

