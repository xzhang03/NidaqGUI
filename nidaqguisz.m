function varargout = nidaqguisz(varargin)
% nidaqguisz MATLAB code for nidaqguisz.fig
%      nidaqguisz, by itself, creates a new nidaqguisz or raises the existing
%      singleton*.
%
%      H = nidaqguisz returns the handle to a new nidaqguisz or the handle to
%      the existing singleton*.
%
%      nidaqguisz('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in nidaqguisz.M with the given input arguments.
%
%      nidaqguisz('Property','Value',...) creates a new nidaqguisz or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before nidaqguisz_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to nidaqguisz_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help nidaqguisz

% Last Modified by GUIDE v2.5 11-Feb-2022 10:50:33

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @nidaqguisz_OpeningFcn, ...
                   'gui_OutputFcn',  @nidaqguisz_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before nidaqguisz is made visible.
function nidaqguisz_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to nidaqguisz (see VARARGIN)

% Choose default command line output for nidaqguisz
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);


nidaq_config_sz;
global nicfg

% Can't load if omnibox is not on
if nicfg.omnibox.enable
    handles.togglebutton2.Enable = 'on';
    handles.settinguploaded.String = 'Omnibox enabled';
else
    handles.togglebutton2.Enable = 'off';
    handles.settinguploaded.String = 'Omnibox disabled';
end

handles.MouseName.String = nicfg.MouseName;

if isfield(nicfg, 'arduino_serial')
    nicfg = rmfield(nicfg, 'arduino_serial');
end

if ~isfield(nicfg, 'RecordRunning')
    nicfg.RecordRunning = false;
end





% UIWAIT makes nidaqguisz wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = nidaqguisz_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;



function MouseName_Callback(hObject, eventdata, handles)
% hObject    handle to MouseName (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of MouseName as text
%        str2double(get(hObject,'String')) returns contents of MouseName as a double

global nicfg
MouseName = (get(hObject,'String')); % returns contents of Enter_ROI as a double
nicfg.MouseName = MouseName

% --- Executes during object creation, after setting all properties.
function MouseName_CreateFcn(hObject, eventdata, handles)
% hObject    handle to MouseName (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function Runs_Callback(hObject, eventdata, handles)
% hObject    handle to Runs (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of Runs as text
%        str2double(get(hObject,'String')) returns contents of Runs as a double

global nicfg
RunsEntered = str2num(get(hObject,'String')); % returns contents of Enter_ROI as a double
nicfg.Run = RunsEntered

% --- Executes during object creation, after setting all properties.
function Runs_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Runs (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end





% --- Executes on button press in togglebutton1.
function togglebutton1_Callback(hObject, eventdata, handles)
% hObject    handle to togglebutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of togglebutton1

global nicfg
    
nicfg.active = get(hObject, 'Value');

if nicfg.active
    disp('Starting...');
    nidaq_config_sz;
    
    set(handles.TimeElapsedNumber, 'String', 'WAIT');
    drawnow();
    
    disp('Resetting nidaq...');
    
    if nicfg.useMLlibrary % Using MonkeyLogic DAQ library or not
        daqtoolbox.daqreset;
    else
        daqreset;
    end
    
    nicfg.MouseName = get(handles.MouseName, 'String'); % returns contents of Enter_ROI as a double
    nicfg.Run = str2double(get(handles.Runs, 'String')); % returns contents of Enter_ROI as a double
    % nicfg
    
    % Reset arduino
    if nicfg.ArduinoCOM > -1 && ~isfield(nicfg, 'arduino_serial')
        disp('Starting Arduino...');
        nicfg.arduino_data = [];
        nicfg.arduino_serial = arduinoOpen(nicfg.ArduinoCOM, nicfg.baumrate);
        
        % 2 Seconds to let arduino catch up
        pause(2);
        
        % Set arduino frequency
        fwrite(nicfg.arduino_serial, uint8([2 nicfg.RunningFrequency]));
        
        % Ping arduino
        fwrite(nicfg.arduino_serial, uint8([5 0]));
        fread(nicfg.arduino_serial, 1, 'int32');
        
        % Parse for omnibox v3
        omniboxparse(nicfg)
    end
    
    % Start nidaq
    if nicfg.NidaqChannels > 0
        nidaqpath = fullfile(nicfg.BasePath, sprintf('%s-%s-%03i-nidaq.mat', nicfg.MouseName, datestamp(), nicfg.Run));
        
        if nicfg.useMLlibrary % Using MonkeyLogic DAQ library or not
            nicfg.nidaq_session = startNidaq_ML(nicfg.NidaqFrequency,...
                nicfg.NidaqChannels, nicfg.NidaqDevice);
        else
            nicfg.nidaq_session = startNidaq(nidaqpath,...
                nicfg.NidaqFrequency, nicfg.NidaqChannels,...
                nicfg.DigitalString, nicfg.NidaqDigitalChannels,...
                nicfg.NidaqDevice);
        end
    end
    
    tic;
    disp('Iterating');
    tstart = clock;
    tnow = clock;
    tseconds = 0;
    
    timeconv = [0 0 86400 3600 60 1]';
    
    % Start camera pulsing
    if nicfg.ArduinoCOM > -1
        fwrite(nicfg.arduino_serial, [1 0]);
    end
    
    while get(hObject, 'Value') == 1
        if floor((tnow - tstart) * timeconv) > tseconds
            tseconds = floor((tnow - tstart) * timeconv);
            elapsedmins = floor(tseconds/60.0);
            set(handles.TimeElapsedNumber, 'String', sprintf('%3i:%02i', elapsedmins, tseconds - elapsedmins*60));
        end

        drawnow();
        
        if nicfg.RecordRunning && nicfg.ArduinoCOM > -1 && toc > 1.0/nicfg.RunningFrequency
            tic;
            fwrite(nicfg.arduino_serial, [5 0]); % Request position info
            nicfg.arduino_data(end+1) = arduinoReadQuad(nicfg.arduino_serial);
        end
        
        tnow = clock;
    end
    
    % Stop camera pulsing
    if nicfg.ArduinoCOM > -1
        fwrite(nicfg.arduino_serial, [0 0]);
    end
    
    disp('Saving...');
    
    if nicfg.ArduinoCOM > -1
        fclose(nicfg.arduino_serial);
        nicfg = rmfield(nicfg, 'arduino_serial');
        if nicfg.RecordRunning
            arduinopath = fullfile(nicfg.BasePath, sprintf('%s-%s-%03i-running.mat', nicfg.MouseName, datestamp(), nicfg.Run));
            position = nicfg.arduino_data;
            speed = runningSpeed(position, nicfg.RunningFrequency);
            save(arduinopath, 'position', 'speed');
        end
    end
    
    if isfield(nicfg, 'nidaq_session')
        if nicfg.useMLlibrary % Using MonkeyLogic DAQ library or not
            stopNidaq_ML(nicfg.nidaq_session, nidaqpath);
        else
            stopNidaq(nicfg.nidaq_session, nicfg.ChannelNames);
        end
    end
    
    if sum(strcmpi(nicfg.serveradd(:,1), nicfg.MouseName(1:2))) > 0 % If has a registered owner
        % Determine owner
        serveradd = nicfg.serveradd{strcmpi(nicfg.serveradd(:,1),...
            nicfg.MouseName(1:2)),2};
        
        disp(['Copying files to server address: ', serveradd]);

        % Grab file names
        [~,nidaqfn,~] = fileparts(nidaqpath);
        if nicfg.RecordRunning
            [~,arduinofn,~] = fileparts(arduinopath); 
        end

        % Make server file address
        nidaqpath_server = fullfile(serveradd, nicfg.MouseName, ...
                sprintf('%s_%s', datestamp(),nicfg.MouseName), [nidaqfn, '.mat']);
        if nicfg.RecordRunning    
            arduinopath_server = fullfile(serveradd, nicfg.MouseName, ...
                    sprintf('%s_%s', datestamp(),nicfg.MouseName), [arduinofn, '.mat']);
        end

        % Make mouse folder if needed
        if exist(fullfile(serveradd, nicfg.MouseName), 'dir') ~= 7
            mkdir(serveradd, nicfg.MouseName);
        end

        % Make day folder if needed
        if exist(fullfile(serveradd, nicfg.MouseName, ...
                sprintf('%s_%s', datestamp(),nicfg.MouseName)), 'dir') ~= 7
            mkdir(fullfile(serveradd, nicfg.MouseName),...
                sprintf('%s_%s', datestamp(),nicfg.MouseName));
        end

        % Copy nidaq file if does not exist
        if ~exist(nidaqpath_server, 'file')
            copyfile(nidaqpath, nidaqpath_server);
        elseif input('Nidaq file already exist. Overwrite? (1 = Yes, 0 = No): ') == 1
            copyfile(nidaqpath, nidaqpath_server);
        end

        % Copy arduino file if does not exist
        if nicfg.RecordRunning && (nicfg.ArduinoCOM > -1)
            if ~exist(arduinopath_server, 'file')
                copyfile(arduinopath, arduinopath_server);
            elseif input('Running file already exist. Overwrite? (1 = Yes, 0 = No): ') == 1
                copyfile(arduinopath, arduinopath_server);
            end
        end
    else
        disp('Did not copy files to server');
    end
    
    disp('Finished');
    
    
end


% --- Executes on button press in togglebutton2.
function togglebutton2_Callback(hObject, eventdata, handles)
% hObject    handle to togglebutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of togglebutton2
% Preset
nidaq_config_sz;
global nicfg
handles.settinguploaded.String = 'Uploading...';
omniboxpreset(nicfg)
handles.settinguploaded.String = 'Setting Uploaded';


% Plot
% omniboxplot(nicfg)

