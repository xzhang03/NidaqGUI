%% Open
clear
picodaq_serial = serialport(sprintf('COM%i', 28), 500000);
disp('Opened.')

%% Set DAQ
% 2500 Hz
write(picodaq_serial, [2 25], 'uint8');

% Analog buffer Depth = 3
write(picodaq_serial, [10 3], 'uint8');

% ADC freq = 2 (8 kSPS)
write(picodaq_serial, [3 2], 'uint8');

% Cmax
cmax = 7500;
cmax1 = floor(cmax / 256);
cmax2 = cmax - cmax1 * 256;
write(picodaq_serial, [41 cmax1], 'uint8');
write(picodaq_serial, [42 cmax2], 'uint8');
write(picodaq_serial, [43 1], 'uint8');

%% Show parameters
write(picodaq_serial, [9 0], 'uint8');

pause(0.1);
vec = [];
ind = 0;
while picodaq_serial.NumBytesAvailable > 0
    ind = ind + 1;
    vec(ind) = read(picodaq_serial, 1, 'uint8');
end
char(vec)

%% Setup
n = 5000;
chunk = 1;
k = zeros(n/chunk, 6 * chunk);

%% Start
i = 0;
disp('Start');
write(picodaq_serial, [1 0], 'uint8');
tic
while i < (n / chunk)
        i = i + 1;        
        k(i,:) = read(picodaq_serial, 6 * chunk, 'int32');       
end
toc
write(picodaq_serial, [0 0], 'uint8');
disp('End');

pause(0.1)
picodaq_serial.NumBytesAvailable
flush(picodaq_serial,"input")

%% Start callback
disp('Start');
configureCallback(picodaq_serial, 'byte', 2400, @picocallback)
write(picodaq_serial, [1 0], 'uint8');
pause(0.1)
write(picodaq_serial, [0 0], 'uint8');
disp('End');
picodaq_serial.NumBytesAvailable
flush(picodaq_serial,"input")
configureCallback(picodaq_serial, 'off')

%% Close
delete(picodaq_serial);
disp('Closed.')