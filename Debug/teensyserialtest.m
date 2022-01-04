%% Open
clear
nidaq_config_sz_test;
nicfg.arduino_serial = serial(nicfg.ArduinoCOM, 'BaudRate', nicfg.baumrate);
fopen(nicfg.arduino_serial);


%% Parse
nicfg.arduino_serial = s1;
omniboxparse(nicfg)

%% run
fwrite(nicfg.arduino_serial, uint8([1 0]));

%% stop
fwrite(nicfg.arduino_serial, uint8([0 0]));

%% Close
fclose(nicfg.arduino_serial);
