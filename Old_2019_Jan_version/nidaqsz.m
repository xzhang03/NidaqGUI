%% Read config
% Remove previous arduino session if needed
if exist('nicfg', 'var')
    if isfield(nicfg, 'arduino_serial')
    	fclose(nicfg.arduino_serial);
    end
    if isfield(nicfg, 'nidaq_session')
    	daqreset;
    end

else
    nidaq_config_sz
end


%% Mouse run info
% How long the run is (in minutes)
run_length = 0.2;

nicfg.MouseName = 'SZ00';
 

disp(['Mouse name: ', nicfg.MouseName])
disp(['Sesion length (in min): ', num2str(run_length)])
disp(['Previous run number is: ', num2str(nicfg.Run)])
RunNum = input('Please enter new run number: ');


nicfg.Run = RunNum;
%% Starting

disp('Starting...');    

if nicfg.ArduinoCOM > -1
    nicfg.arduino_data = [];
    nicfg.arduino_serial = arduinoOpen(nicfg.ArduinoCOM);
    arduinoReadQuad(nicfg.arduino_serial);
end
%%
if nicfg.NidaqChannels > 0
    nidaqpath = fullfile(nicfg.BasePath, sprintf('%s-%s-%03i-nidaq.mat', nicfg.MouseName, datestamp(), nicfg.Run));
    nicfg.nidaq_session = startNidaqsz(nidaqpath, nicfg.NidaqFrequency, nicfg.NidaqChannels, nicfg.DigitalString);
end

% Pause for nidaq background to catch up (seriously)
disp('Pre-bufer...')
pause(2)
%% Iterating and stopping
% Iterating
disp('Iterating');
tic;
% Initialize waitbar
textprogressbar('Recording progress: ');

% Initialize all the timing variables
tnow = clock;       
tstart = clock;
tseconds = 0;
tic;
while (tnow(5) - tstart(5) + (tnow(6) - tstart(6)) / 60) <= run_length
    
    % Update waitbar every x seconds
    if floor(tnow(6) - tstart(6)) > tseconds
        textprogressbar((tnow(5) - tstart(5) + (tnow(6) - tstart(6)) / 60) / run_length *100);
        tseconds = floor(tnow(6) - tstart(6));
    end

    if nicfg.ArduinoCOM > -1 && toc > 1.0/nicfg.RunningFrequency
        tic;
        nicfg.arduino_data = [nicfg.arduino_data arduinoReadQuad(nicfg.arduino_serial)];
    end
    
    tnow = clock;
end
textprogressbar('done');

% Buffer before stop
disp('Post-bufer...')
pause(2)

% Stopping
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