function omniboxpreset(nicfg)
  % Briefly open up arduino communication to setup omni box

%% Open
% Set serial and open
if ischar(nicfg.ArduinoCOM)
    com = nicfg.ArduinoCOM;
else
    com = sprintf('COM%i', nicfg.ArduinoCOM);
end

nicfg.arduino_serial = serialinitial(com, nicfg.baumrate);

% nicfg.arduino_serial = serial(com, 'BaudRate', nicfg.baumrate);
% fopen(nicfg.arduino_serial);

% Wait
pause(2);

%% Parse
try omniboxparse(nicfg)
catch me
    disp(me.message);
    disp(me.stack(1))
    arduinoClose(nicfg.arduino_serial);
    error('Parsing config file failed, most likely due to missing fields. See above.')
end

%% Close
arduinoClose(nicfg.arduino_serial);

end