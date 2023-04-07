function omniboxreboot(nicfg)
  % Briefly open up arduino communication to setup omni box

%% Open
% Set serial and open
nicfg.arduino_serial = serialinitial(sprintf('COM%i', nicfg.ArduinoCOM), nicfg.baumrate);

% Wait
pause(2);

%% Parse
arduinoWrite(nicfg.arduino_serial, [253 104]);

%% Close
arduinoClose(nicfg.arduino_serial);

end