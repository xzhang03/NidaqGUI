%% Read config
% Reset daq
daqreset;

% Remove previous arduino session if needed
if exist('nicfg', 'var')
    if isfield(nicfg, 'arduino_serial')
    	fclose(nicfg.arduino_serial);
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
%     arduinoReadQuad(nicfg.arduino_serial);
end

if nicfg.NidaqChannels > 0
    nidaqpath = fullfile(nicfg.BasePath, sprintf('%s-%s-%03i-nidaq.mat', nicfg.MouseName, datestamp(), nicfg.Run));
    nicfg.nidaq_session = startNidaq(nidaqpath, nicfg.NidaqFrequency, nicfg.NidaqChannels, nicfg.DigitalString, nicfg.NidaqDigitalChannels);
end

% Pause for nidaq background to catch up (seriously)
disp('Pre-bufer...')
pause(1)

% Iterating and stopping
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
textprogressbar('Done');

% Buffer before stop
% disp('Post-bufer...')
% pause(2)
%%
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

if ~isempty(nicfg.serveradd)
    disp('Moving files to server...');
    
    % Grab file names
    [~,nidaqfn,~] = fileparts(nidaqpath);
    [~,arduinofn,~] = fileparts(arduinopath);
    
    % Make server file address
    nidaqpath_server = fullfile(nicfg.serveradd, nicfg.MouseName, ...
            sprintf('%s_%s', datestamp(),nicfg.MouseName), [nidaqfn, '.mat']);
    arduinopath_server = fullfile(nicfg.serveradd, nicfg.MouseName, ...
            sprintf('%s_%s', datestamp(),nicfg.MouseName), [arduinofn, '.mat']);
    
    % Make mouse folder if needed
    if exist(fullfile(nicfg.serveradd, nicfg.MouseName), 'dir') ~= 7
        mkdir(nicfg.serveradd, nicfg.MouseName);
    end
    
    % Make day folder if needed
    if exist(fullfile(nicfg.serveradd, nicfg.MouseName, ...
            sprintf('%s_%s', datestamp(),nicfg.MouseName)), 'dir') ~= 7
        mkdir(fullfile(nicfg.serveradd, nicfg.MouseName),...
            sprintf('%s_%s', datestamp(),nicfg.MouseName));
    end
    
    % Copy nidaq file if does not exist
    if ~exist(nidaqpath_server, 'file')
        copyfile(nidaqpath, nidaqpath_server);
    elseif input('Nidaq file already exist. Overwrite? (1 = Yes, 0 = No): ') == 1
        copyfile(nidaqpath, nidaqpath_server);
    end
    
    % Copy arduino file if does not exist
    if ~exist(arduinopath_server, 'file')
        copyfile(arduinopath, arduinopath_server);
    elseif input('Running file already exist. Overwrite? (1 = Yes, 0 = No): ') == 1
        copyfile(arduinopath, arduinopath_server);
    end
end
disp('Finished');
