daqname = 'Dev2';

daq_connection = daq.createSession('ni'); %create a session in 64 bit
daq_connection.Rate = 1000; 
daq_connection.IsContinuous = true;
ai = daq_connection.addAnalogInputChannel(daqname, 0:7 , 'Voltage'); 

for i = 1:8
    ai(i).Range = [-10 10];
    ai(i).TerminalConfig = 'SingleEnded';
end

daq_connection.addDigitalChannel(daqname, 'Port0/Line0:3', 'InputOnly');
daq_connection.stop;

%% Triggered
daqname = 'Dev2';
daq_connection = daq.createSession('ni'); %create a session in 64 bit
daq_connection.Rate = 1000; 
ai = daq_connection.addAnalogInputChannel(daqname, 0:7 , 'Voltage'); 
daq_connection.addDigitalChannel(daqname, 'Port0/Line0:3', 'InputOnly');
daq_connection.startForeground;

% s.addTriggerConnection('Dev1/PFI4','Dev2/PFI0','StartTrigger')

%% Previous
daqname = 'Dev2';
daq_connection = daq.createSession('ni'); %create a session in 64 bit
ai = daq_connection.addAnalogInputChannel(daqname, 0 , 'Voltage'); 
% di = daq_connection.addDigitalChannel(daqname, 'Port0/Line0', 'InputOnly');
% ci = daq_connection.addCounterInputChannel(daqname, 'ctr0', 'EdgeCount');
lh = addlistener(daq_connection,'DataAvailable',@(src,event)...
            plot(event.TimeStamps,event.Data));
daq_connection.IsContinuous = true;
daq_connection.startBackground;
pause(1)
daq_connection.stop;
delete(lh)

%% external triggered
daqname = 'Dev2';
daq_connection = daq.createSession('ni');
di = daq_connection.addDigitalChannel(daqname, 'Port0/Line2', 'InputOnly');
ci = daq_connection.addClockConnection('external', 'Dev2/PFI0', 'Scanclock');
daq_connection.startForeground;