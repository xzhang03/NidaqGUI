function openDAQ = startpicoDAQ(path, varargin)
% Initialize a picodaq recording

%% Parse
if nargin < 2
    varargin = {};
end

% Parse input
p  = inputParser;

% DAQ
addOptional(p, 'daqcom', 'COM1');
addOptional(p, 'frequency', 2500); % DAQ sampling frequency, must be multiples of 100;
addOptional(p, 'databuffer', 7500); % Save to harddrive every X bytes of data recorded (must be multiples of 6). Each time point is 6 bytes.
addOptional(p, 'fda', 8); % Signal amplitude suppression by fully-diff amplifier, multiply this value at the end.
addOptional(p, 'timeout', 2); % Timeout default 10 s

% ADC
addOptional(p, 'adcfreqmod', 2); % ADC raw freq = 32K / 2 ^ n. E.g., 2 means 8 kSPS.
addOptional(p, 'adcsmooth', 2); % ADC smoothing (divide the raw freq by this number).
addOptional(p, 'adcgain', 0); % Gain = 2 ^ n. Leave as is unless you know what you are doing.

% Book keeping (do not change)
addOptional(p, 'nchannels', 4);
addOptional(p, 'ndigitals', 16);
addOptional(p, 'ndaqs', 1);

% Debug
addOptional(p, 'maxsamples', []); % Mostly for testing. Can be 1 - 65535 or empty (unlimited).
addOptional(p, 'echofirm', true);
addOptional(p, 'echosetting', false); % Ping setting at the start

% Unpack if needed
if size(varargin,1) == 1 && size(varargin,2) == 1
    varargin = varargin{:};
end

% Parse
parse(p, varargin{:});
p = p.Results;

%% Communicate with daq
% Open
picodaq_serial = serialport(p.daqcom, 500000);

% Ping firmware
if (p.echofirm)
    write(picodaq_serial, [38 0], 'uint8');

    pause(0.1);
    vec = [];
    ind = 0;
    while picodaq_serial.NumBytesAvailable > 0
        ind = ind + 1;
        vec(ind) = read(picodaq_serial, 1, 'uint8');
    end
    disp(erase(char(vec), char(10)));
end

% Set parameters
% ADC gain
write(picodaq_serial, [4 p.adcgain], 'uint8');

% Hz
write(picodaq_serial, [2 p.frequency/100], 'uint8');

% ADC freq = 2 (8 kSPS)
write(picodaq_serial, [3 p.adcfreqmod], 'uint8');

% Analog buffer Depth = 3
write(picodaq_serial, [10 p.adcsmooth], 'uint8');

% Cmax
if ~isempty(p.maxsamples)
    cmax1 = floor(p.maxsamples / 256);
    cmax2 = p.maxsamples - cmax1 * 256;
    write(picodaq_serial, [41 cmax1], 'uint8');
    write(picodaq_serial, [42 cmax2], 'uint8');
    write(picodaq_serial, [43 1], 'uint8');
else
    write(picodaq_serial, [43 0], 'uint8');
end

% Ping Setting
if (p.echosetting)
    write(picodaq_serial, [9 0], 'uint8');

    pause(0.1);
    vec = [];
    ind = 0;
    while picodaq_serial.NumBytesAvailable > 0
        ind = ind + 1;
        vec(ind) = read(picodaq_serial, 1, 'uint8');
    end
    disp(erase(char(vec), char(10)));
end

%% Logger
% Set file up
[basepath, file, ~] = fileparts(path);
logpath = fullfile(basepath, [file '-log.bin']);
logfile = fopen(logpath, 'w');

% Add callback
configureCallback(picodaq_serial, 'byte', p.databuffer, @(src, event)picocallback(src, event, logfile));

%% Set reading frequency
m = 2.5;
picodaq_serial.UserData = struct('bytecheck', p.databuffer * m);

%% Set timeout
picodaq_serial.Timeout = p.timeout;

%% Start
write(picodaq_serial, [1 0], 'uint8');

%% Output
openDAQ = struct( ...
        'logfile', logfile, ...
        'picodaq_serial', picodaq_serial, ...
        'logpath', logpath, ...
        'frequency', p.frequency, ...
        'nchannels', p.nchannels, ...
        'ndigitals', p.ndigitals, ...
        'path', path ,...
        'ndevs', p.ndaqs,...
        'pidaq_params', p...
    );

end