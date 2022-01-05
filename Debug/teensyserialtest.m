%% Open
clear
nidaq_config_sz;
nicfg.arduino_serial = serial(sprintf('COM%i', nicfg.ArduinoCOM), 'BaudRate', nicfg.baumrate);
fopen(nicfg.arduino_serial);


%% Parse
omniboxparse(nicfg)

%% run
fwrite(nicfg.arduino_serial, uint8([1 0]));

%% stop
fwrite(nicfg.arduino_serial, uint8([0 0]));

%% Close
fclose(nicfg.arduino_serial);
