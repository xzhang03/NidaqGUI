%% Open
clear
picodaq_serial = serial(sprintf('COM%i', 9), 'BaudRate', 500000);
picodaq_serial.InputBufferSize = 2 ^ 20;
fopen(picodaq_serial);
disp('Opened.')

%% Set DAQ
% 2500 Hz
fwrite(picodaq_serial, uint8([2 25]));

% Analog buffer Depth = 3
fwrite(picodaq_serial, uint8([10 3]));

% ADC freq = 2 (8 kSPS)
fwrite(picodaq_serial, uint8([3 2]));

% Cmax
cmax = 7500;
cmax1 = floor(cmax / 256);
cmax2 = cmax - cmax1 * 256;
fwrite(picodaq_serial, uint8([41 cmax1]));
fwrite(picodaq_serial, uint8([42 cmax2]));
fwrite(picodaq_serial, uint8([43 1]));


%% Show parameters
fwrite(picodaq_serial, uint8([9 0]));
pause(0.1);
vec = [];
ind = 0;
while picodaq_serial.BytesAvailable > 0
    ind = ind + 1;
    vec(ind) = fread(picodaq_serial, 1, 'uint8');
end
char(vec)

%% Setup
n = 5000;
chunk = 1;
k = zeros(n/chunk, 6 * chunk);

%% Start
i = 0;
disp('Start');
fwrite(picodaq_serial, uint8([1 0]));
tic
while i < (n / chunk)
        i = i + 1;
        
        k(i,:) = fread(picodaq_serial, 6 * chunk, 'int32');       
end
toc
fwrite(picodaq_serial, uint8([0 0]));
disp('End');
picodaq_serial.BytesAvailable

% Flush
vec = [];
ind = 0;
while picodaq_serial.BytesAvailable >= 4
    ind = ind + 1;
    vec(ind) = fread(picodaq_serial, 1, 'int32');
end

while picodaq_serial.BytesAvailable > 0
    fread(picodaq_serial, 1, 'uint8');
end

%% Close
fclose(picodaq_serial);
disp('Closed.')
