function nidaqguisz_mini

nidaq_config_sz;
    
run_length = 0.2;

disp('Starting...');


nicfg.ArduinoCOM = -1;


nicfg.MouseName = 'SZ00'; % returns contents of Enter_ROI as a double
nicfg.Run = 0; % returns contents of Enter_ROI as a double
% nicfg

if nicfg.ArduinoCOM > -1
    nicfg.arduino_data = [];
    nicfg.arduino_serial = arduinoOpen(nicfg.ArduinoCOM);
%     arduinoReadQuad(nicfg.arduino_serial);
end
if nicfg.NidaqChannels > 0
    nidaqpath = fullfile(nicfg.BasePath, sprintf('%s-%s-%03i-nidaq.mat', nicfg.MouseName, datestamp(), nicfg.Run));
    nicfg.nidaq_session = startNidaq(nidaqpath, nicfg.NidaqFrequency, nicfg.NidaqChannels, nicfg.DigitalString, nicfg.NidaqDigitalChannels);
end

pause(1)

tic;
disp('Iterating');
accumulator = 0;
%     tstart = cputime();
%     tseconds = 0;

while accumulator <= (run_length * 60 * 30)
%         if floor(cputime() - tstart) > tseconds
%             tseconds = floor(cputime() - tstart);
%             elapsedmins = floor(tseconds/60.0);
%             set(handles.TimeElapsedNumber, 'String', sprintf('%3i:%02i', elapsedmins, tseconds - elapsedmins*60));
%         end



    if nicfg.ArduinoCOM > -1 && toc > 1.0/nicfg.RunningFrequency
        tic;
        nicfg.arduino_data = [nicfg.arduino_data arduinoReadQuad(nicfg.arduino_serial)];
        
    end
    accumulator = accumulator+1;
end

disp('Saving...');

if nicfg.ArduinoCOM > -1
    fclose(nicfg.arduino_serial);
    arduinopath = fullfile(nicfg.BasePath, sprintf('%s-%s-%03i-running.mat', nicfg.MouseName, datestamp(), nicfg.Run));
    position = nicfg.arduino_data;
    speed = runningSpeed(position, nicfg.RunningFrequency);
    save(arduinopath, 'position', 'speed');
end

if isfield(nicfg, 'nidaq_session')
    stopNidaq(nicfg.nidaq_session, nicfg.ChannelNames);
end
disp('Finished');
end