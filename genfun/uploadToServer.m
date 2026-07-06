function uploadToServer(nicfg, nidaqpath, arduinopath)
% Copy this run's saved data file(s) to the mouse's registered server
% address, if one exists and upload is enabled.
% Pass '' for nidaqpath/arduinopath if that data was not acquired this run
% (e.g. NidaqChannels == 0, or RecordRunning == false).

if ~((sum(strcmpi(nicfg.serveradd(:,1), nicfg.MouseName(1:2))) > 0) && nicfg.serverupload)
    disp('Did not copy files to server');
    return;
end

% Determine owner
serveradd = nicfg.serveradd{strcmpi(nicfg.serveradd(:,1), nicfg.MouseName(1:2)), 2};

disp(['Copying files to server address: ', serveradd]);

% Make server file address
if nicfg.mousedate
    % Mouse_Date
    daystamp = sprintf('%s_%s', nicfg.MouseName, datestamp());
else
    % Date_Mouse
    daystamp = sprintf('%s_%s', datestamp(), nicfg.MouseName);
end

% Make mouse folder if needed
if exist(fullfile(serveradd, nicfg.MouseName), 'dir') ~= 7
    mkdir(serveradd, nicfg.MouseName);
end

% Make day folder if needed
if exist(fullfile(serveradd, nicfg.MouseName, daystamp), 'dir') ~= 7
    mkdir(fullfile(serveradd, nicfg.MouseName), daystamp);
end

% Copy nidaq file, if one was acquired
if ~isempty(nidaqpath)
    [~, nidaqfn, ~] = fileparts(nidaqpath);
    nidaqpath_server = fullfile(serveradd, nicfg.MouseName, daystamp, [nidaqfn, '.mat']);

    if ~exist(nidaqpath_server, 'file')
        copyfile(nidaqpath, nidaqpath_server);
    elseif input('Nidaq file already exist. Overwrite? (1 = Yes, 0 = No): ') == 1
        copyfile(nidaqpath, nidaqpath_server);
    end
end

% Copy arduino running file, if one was acquired
if ~isempty(arduinopath)
    [~, arduinofn, ~] = fileparts(arduinopath);
    arduinopath_server = fullfile(serveradd, nicfg.MouseName, daystamp, [arduinofn, '.mat']);

    if ~exist(arduinopath_server, 'file')
        copyfile(arduinopath, arduinopath_server);
    elseif input('Running file already exist. Overwrite? (1 = Yes, 0 = No): ') == 1
        copyfile(arduinopath, arduinopath_server);
    end
end

end
