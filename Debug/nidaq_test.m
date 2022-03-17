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