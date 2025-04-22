function previewchannel(nicfg)
% Preview channel data in matlab

if nicfg.usepicoDAQ
    % Basic info
    daqname = 'PicoDAQ';

    % Number of analog channels (hardcoded for now)
    nchannels = 20;

    % Channels
    channels = 0: nchannels-1;
    channelcell = cell(nchannels, 1);
    for i = 1 : 4
        channelcell{i} = sprintf('ai%i (%i)', channels(i), channels(i)+1);
    end
    for i = 5 : nchannels
        channelcell{i} = sprintf('di%i (%i)', channels(i)-4, channels(i)+1);
    end

    % Modes
    modes = {'Auto'};
else
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
end



%% Initialize figure
hfig = figure('Name', 'Preview', 'CloseRequestFcn',@myCloseReq);
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
if nicfg.usepicoDAQ
    modemenu.Enable = 'inactive';
    modemenu.ForegroundColor = [0.5 0.5 0.5];
end

% Rate
uicontrol(hpan, 'Style', 'text', 'Position', toploc - [0 100 0 0], 'String',...
    'Rate', 'FontSize', 10);
ratecontrol = uicontrol(hpan, 'Style', 'edit', 'Position', toploc - [0 120 0 0], 'String',...
    '2000', 'FontSize', 10);

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
        % Specifics
        channelmenu.UserData = channels(channelmenu.Value);
        ratecontrol.UserData = str2double(ratecontrol.String);
        samplecontrol.UserData = str2double(samplecontrol.String);
        displaycontrol.UserData = str2double(displaycontrol.String);
    
        if nicfg.usepicoDAQ
            % Serial initialize
            picodaq_serial = previewpicoDAQ(nicfg.picoDAQparams);
            
            % Over-write frequency
            write(picodaq_serial, [2 ratecontrol.UserData/100], 'uint8');
    
            % Add callback
            configureCallback(picodaq_serial, 'byte', samplecontrol.UserData * 6, @plotData_picodaq);

            % Start
            write(picodaq_serial, [1 0], 'uint8');

            src.UserData = picodaq_serial;

        else
            % Specify connection
            daq_connection = daq.createSession('ni'); %create a session in 64 bit
            
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
    end
    
%% Stop
    function stoppreview(~,~)
        if nicfg.usepicoDAQ
            write(startbutton.UserData, [0 0], 'uint8');
            flush(startbutton.UserData, "input");
            configureCallback(startbutton.UserData, 'off');
            delete(startbutton.UserData);
        else
            startbutton.UserData.stop;
            delete(startbutton.UserData);
        end
    end

    function myCloseReq(src,event)
        if isvalid(startbutton.UserData)
            try
                stoppreview();
            catch
                disp('Unable to stop preview. Please check COM.')
            end
        end
        delete(src);
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

    function plotData_picodaq(src, ~)
        try
            data = read(src, src.BytesAvailableFcnCount, 'int32');
        catch
            return;
        end

        data = reshape(data, 6, []);

        if channelmenu.UserData < 4
            % Analog
            data = data(channelmenu.UserData+3, :)';
            data = data / 2^23 * 1.2 * 8;
        else
            % Digital
            data = bitget(data(2,:), 16 - channelmenu.UserData + 4);
            data = data';
        end
        
        % Plot
        x = (1 : displaycontrol.UserData)' / ratecontrol.UserData;
        data2show = data(1 : displaycontrol.UserData);
        plot(x, data2show)

        % FFT 
        d = data2show - mean(data2show);
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