function varargout = nidaqgui(varargin)
% nidaqgui MATLAB code for nidaqgui.fig
%      nidaqgui, by itself, creates a new nidaqgui or raises the existing
%      singleton*.
%
%      H = nidaqgui returns the handle to a new nidaqgui or the handle to
%      the existing singleton*.
%
%      nidaqgui('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in nidaqgui.M with the given input arguments.
%
%      nidaqgui('Property','Value',...) creates a new nidaqgui or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before nidaqgui_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to nidaqgui_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help nidaqgui

% Last Modified by GUIDE v2.5 14-Jul-2017 13:01:22

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @nidaqgui_OpeningFcn, ...
                   'gui_OutputFcn',  @nidaqgui_OutputFcn, ...
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


% --- Executes just before nidaqgui is made visible.
function nidaqgui_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to nidaqgui (see VARARGIN)

% Choose default command line output for nidaqgui
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

nidaq_config;
global nicfg


% UIWAIT makes nidaqgui wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = nidaqgui_OutputFcn(hObject, eventdata, handles) 
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
    set(handles.TimeElapsedNumber, 'String', 'WAIT');
    drawnow();
    
    nicfg.MouseName = get(handles.MouseName, 'String'); % returns contents of Enter_ROI as a double
    nicfg.Run = str2double(get(handles.Runs, 'String')); % returns contents of Enter_ROI as a double
    % nicfg
    
    if nicfg.ArduinoCOM > -1
        nicfg.arduino_data = [];
        nicfg.arduino_serial = arduinoOpen(nicfg.ArduinoCOM);
        arduinoReadQuad(nicfg.arduino_serial);
    end
    if nicfg.NidaqChannels > 0
        nidaqpath = fullfile(nicfg.BasePath, sprintf('%s-%s-%03i-nidaq.mat', nicfg.MouseName, datestamp(), nicfg.Run));
        nicfg.nidaq_session = startNidaq(nidaqpath, nicfg.NidaqFrequency, nicfg.NidaqChannels, nicfg.DigitalString, nicfg.NidaqDigitalChannels);
    end
    
    tic;
    disp('Iterating');
    % Use clock to grab system time, instead of CPU time. 6th entry is
    % seconds
    tstart = clock;
    tnow = clock;
    tseconds = 0;
    while get(hObject, 'Value') == 1
        if floor(tnow(6) - tstart(6)) > tseconds
            tseconds = floor(tnow(6) - tstart(6));
            elapsedmins = floor(tseconds/60.0);
            set(handles.TimeElapsedNumber, 'String', sprintf('%3i:%02i', elapsedmins, tseconds - elapsedmins*60));
        end

        drawnow();
        
        if nicfg.ArduinoCOM > -1 && toc > 1.0/nicfg.RunningFrequency
            tic; % SZ moved tic up 4/12/2019 to ensure acurate cycle times
            nicfg.arduino_data = [nicfg.arduino_data arduinoReadQuad(nicfg.arduino_serial)];
        end
        
        tnow = clock;

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
