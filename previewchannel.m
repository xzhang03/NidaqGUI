function previewchannel(nicfg)
% Preview channel data in matlab

% Number of analog channels (hardcoded for now)
nchannels = 8;

% Basic info
daqname = nicfg.NidaqDevice;

% Channels
channels = 0: nchannels-1;
channelcell = cell(nchannels, 1);
for i = 1 : nchannels
    channelcell{i} = sprintf('ai%i (%i)', channels(i), channels(i)+1);
end

% Modes
modes = {'Differential', 'SingleEnded', 'SingleEndedNonReferenced'};
%% Initialize figure
hfig = figure('Name', 'Preview');
subplot(1,6,2:6);
% plot(rand(100,1))

% UI elements
% Panel
hpan = uipanel(hfig,'Position',[0.0100 0.1000 0.180 0.850]);

% Display
toploc = [10 330 70 20];

% Channel
uicontrol(hpan, 'Style', 'text', 'Position', toploc, 'String',...
    'Channel', 'FontSize', 10);
channelmenu = uicontrol(hpan, 'Style', 'popupmenu', 'Position', toploc - [0 20 0 0],...
    'String', channelcell);

% Mode
uicontrol(hpan, 'Style', 'text', 'Position', toploc - [0 50 0 0], 'String',...
    'Mode', 'FontSize', 10);
modemenu = uicontrol(hpan, 'Style', 'popupmenu', 'Position', toploc - [0 70 0 0],...
    'String', modes);

% Rate
uicontrol(hpan, 'Style', 'text', 'Position', toploc - [0 100 0 0], 'String',...
    'Rate', 'FontSize', 10);
ratecontrol = uicontrol(hpan, 'Style', 'edit', 'Position', toploc - [0 120 0 0], 'String',...
    '1000', 'FontSize', 10);

% Samples
uicontrol(hpan, 'Style', 'text', 'Position', toploc - [0 150 0 0], 'String',...
    'Samples', 'FontSize', 10);
samplecontrol = uicontrol(hpan, 'Style', 'edit', 'Position', toploc - [0 170 0 0], 'String',...
    '1000', 'FontSize', 10);

% Displays
uicontrol(hpan, 'Style', 'text', 'Position', toploc - [0 200 0 0], 'String',...
    'Display', 'FontSize', 10);
displaycontrol = uicontrol(hpan, 'Style', 'edit', 'Position', toploc - [0 220 0 0], 'String',...
    '100', 'FontSize', 10);

% Buttons
startbutton = uicontrol(hpan, 'Style', 'pushbutton', 'Position', toploc - [0 275 0 0], 'String',...
    'Start', 'FontSize', 10, 'callback', @startpreview);
uicontrol(hpan, 'Style', 'pushbutton', 'Position', toploc - [0 300 0 0], 'String',...
    'Stop', 'FontSize', 10, 'callback', @stoppreview);

%% Start
    function startpreview(src,~)
        % Specify connection
        daq_connection = daq.createSession('ni'); %create a session in 64 bit
        channelmenu.UserData = channels(channelmenu.Value);
        ratecontrol.UserData = str2double(ratecontrol.String);
        samplecontrol.UserData = str2double(samplecontrol.String);
        displaycontrol.UserData = str2double(displaycontrol.String);
        mode = modes{modemenu.Value};
        daq_connection.Rate = ratecontrol.UserData;
        daq_connection.NumberOfScans = samplecontrol.UserData;
        daq_connection.NotifyWhenDataAvailableExceeds = displaycontrol.UserData;
        
        % Channel
        ai = daq_connection.addAnalogInputChannel(daqname, channelmenu.UserData , 'Voltage'); 
        ai.TerminalConfig = mode;
        
        % Listener
        lh = addlistener(daq_connection,'DataAvailable',@plotData);
        daq_connection.IsContinuous = true;
        daq_connection.startBackground;
        src.UserData = daq_connection;
    end
    
%% Stop
    function stoppreview(~,~)
        startbutton.UserData.stop;
        delete(startbutton.UserData);
    end

%% Plot
    function plotData(~, event)
        plot(event.TimeStamps,event.Data)
        
        % FFT 
        d = event.Data - mean(event.Data);
        y = fft(d);
        n = length(d);
        y2 = abs(y/ratecontrol.UserData);
        y3 = y2(1:n/2+1);
        y3(2:end-1) = 2*y3(2:end-1);
        x = ratecontrol.UserData*(0:(n/2))/n;
        [ymax, loc] = max(y3);
        title(sprintf('Dominant Freq = %i Hz, Power = %iE-5', round(x(loc)), round(ymax*10e5)));
    end

    
end