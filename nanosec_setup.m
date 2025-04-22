function nanosec_setup()
% Update order for new nanosec versions
% 1. Update firm ware in arduino
% 2. Update hardware connections as needed
% 3. Remove old nanosec paths from Matlab
% 4. Navigate matlab to the new nanosec folder. Run this once to setup
% nanosec (no need to run until next update)
% 5. New config explanations can be found in: https://github.com/xzhang03/NidaqGUI/tree/master/Configs


%% Firm ware version
fver = 3.51;
fprintf('Expected firmware version v%0.1f\n', fver);

%% Use Picodaq
usepicodaq = questdlg('Use PicoDAQ?', ...
	'PicoDAQ', ...
	'Yes','No', 'Yes');

%% Ping serials
pingserials = questdlg('Ping serial ports?', ...
	'Ping Serial', ...
	'Yes','No', 'Yes');
if strcmpi(pingserials, 'Yes')
    pingCOMs();
end

%% Path
% Find nanosec
nanosecpath = dir('nanosec.m');

if isempty(nanosecpath)
    disp('Nanosec not found. Please navigate to the nanosec folder.');
    return;
elseif length(nanosecpath) > 1
    disp('More than 1 Nanosec found. Please remove extra ones from Matlab search path.');
    for i = 1 : length(nanosecpath)
        disp(fullfile(nanosecpath(i).folder, nanosecpath(i).name));
    end
    return;
end

% Navigate over
cd(nanosecpath.folder);

%% Check matlab version
old = isempty(which('serialport'));
nanosec_settings.oldmatlab = old;

if old
    disp('Matlab is old. Please copy "nanosecfun\old" to overwrite "nanosecfun".');
end

%% Adding folders to path
fprintf('Adding folders to path: \n');
switch usepicodaq
    case 'Yes'
        folderlist = {'nanosecfun', 'nidaqfun', 'picodaqfun', 'genfun'};
        msgbox('Please be sure that nicfg.usepicoDAQ is set to true in config files', 'picoDAQ');
    case 'No'
        folderlist = {'nanosecfun', 'nidaqfun', 'genfun'};
end
addpath(nanosecpath.folder);
fprintf('Parent folder, ');
for i = 1 : length(folderlist)
    addpath(folderlist{i});
    if i < length(folderlist)
        fprintf('%s, ', folderlist{i});
    else
        fprintf('%s\n', folderlist{i});
    end
end

%% Save path
savepath;
fprintf('Paths saved.\n');

%% Settings
% Firmware
nanosec_settings.firmware_ver = fver;

% Home path
nanosec_settings.nanosec_path = nanosecpath.folder;

% Config path
if exist('ns_config.m', 'file')
    default_config = dir('ns_config.m');
else
    default_config = dir('nidaq_config_sz.m');
end

% Config
[~,nanosec_settings.nanosec_config_name,~] = fileparts(default_config.name);
nanosec_settings.nanosec_config_path = fullfile(default_config.folder, default_config.name);

save(fullfile(nanosec_settings.nanosec_path, 'nanosec_settings.mat'), '-struct', 'nanosec_settings');
fprintf('Nanosec settings saved.\n');

end