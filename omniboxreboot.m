function omniboxreboot(nicfg)
  % Briefly open up arduino communication to setup omni box

%% Open
% Set serial and open
nicfg.arduino_serial = serial(sprintf('COM%i', nicfg.ArduinoCOM), 'BaudRate', nicfg.baumrate);
fopen(nicfg.arduino_serial);

% Wait
pause(2);

%% Parse
fwrite(nicfg.arduino_serial, uint8([253 104]));

%% Close
fclose(nicfg.arduino_serial);

end