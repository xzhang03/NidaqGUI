%% Open
clear
nidaq_config_sz;
nicfg.arduino_serial = serial(sprintf('COM%i', nicfg.ArduinoCOM), 'BaudRate', nicfg.baumrate);
fopen(nicfg.arduino_serial);
disp('Opened.')

%% Parse
nidaq_config_sz;
omniboxparse(nicfg)

%% run
fwrite(nicfg.arduino_serial, uint8([1 0]));

%% read
fwrite(nicfg.arduino_serial, uint8([5 0]));
val = fread(nicfg.arduino_serial, 1, 'int32');
disp(val);


%% stop
fwrite(nicfg.arduino_serial, uint8([0 0]));

%% Close
fclose(nicfg.arduino_serial);
disp('Closed.')

%% Reset
fwrite(nicfg.arduino_serial, uint8([253 104]));
fclose(nicfg.arduino_serial);
disp('Reset and closed.')