%% Open
clear
ns_config_debug;
nicfg.arduino_serial = serialport(sprintf('COM%i', nicfg.ArduinoCOM), nicfg.baumrate);
disp('Opened.')

%% Parse
ns_config_debug;
omniboxparse(nicfg)

%% Get summary (run serial debug after)
write(nicfg.arduino_serial, [9 0], 'uint8');

%% Serial debug
vec = [];
ind = 0;
while nicfg.arduino_serial.NumBytesAvailable > 0
    ind = ind + 1;
    vec(ind) = read(nicfg.arduino_serial, 1, 'uint8');
end
char(vec)

%% Bytes
nicfg.arduino_serial.NumBytesAvailable

%% Type cast
typecast(int32(val),'uint8')

%% run
write(nicfg.arduino_serial, [1 0], 'uint8');
disp('Running')

%% read
fwrite(nicfg.arduino_serial, uint8([5 0]));
val = fread(nicfg.arduino_serial, 1, 'int32');
disp(val);

%% Scheduler
fwrite(nicfg.arduino_serial, uint8([255 0]));
val = fread(nicfg.arduino_serial, 1, 'int32');
typecast(int32(val), 'uint8')

%% RNG
fwrite(nicfg.arduino_serial, uint8([255 38]));
val = fread(nicfg.arduino_serial, 1, 'int32');
typecast(int32(val), 'uint8')

%% RNG pass chance
fwrite(nicfg.arduino_serial, uint8([255 39]));
val = fread(nicfg.arduino_serial, 1, 'int32');
typecast(int32(val), 'uint8')


%% stop
write(nicfg.arduino_serial, [0 0], 'uint8');
% val = read(nicfg.arduino_serial, 1, 'int32');
val = read(nicfg.arduino_serial, nicfg.arduino_serial.NumBytesAvailable, 'uint8');
% disp(val);
char(val)

%% Close
delete(nicfg.arduino_serial);
disp('Closed.')

%% Reset
fwrite(nicfg.arduino_serial, uint8([253 104]));
fclose(nicfg.arduino_serial);
disp('Reset and closed.')

%% I2c ping
% Run serial debug after
fwrite(nicfg.arduino_serial, [60 0]); 

%% RNG dump
% Run serial debug after
fwrite(nicfg.arduino_serial, [64 0]); 

%% test
fwrite(nicfg.arduino_serial, [68 0]); 

%% Performance (speed test)
fwrite(nicfg.arduino_serial, uint8([1 0]));
disp('Running')
tic;
while toc < 15
    d = arduinoReadQuad(nicfg.arduino_serial);
end
fwrite(nicfg.arduino_serial, uint8([0 0]));
disp('Done.')

d2 = [0 0 0 0];
while d2(1) ~= 128
    d = arduinoReadQuad(nicfg.arduino_serial);
    if isempty(d)
        d2 = [0 0 0 0];
    else
        d2 = typecast(int32(d),'uint8');
    end
end

cycles = arduinoReadQuad(nicfg.arduino_serial);
time = arduinoReadQuad(nicfg.arduino_serial);
time/cycles
