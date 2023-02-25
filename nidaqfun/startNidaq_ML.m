function open_daq = startNidaq_ML(frequency, nchannels, daqname)
% startNidaq_ML uses the MonkeyLogic DAQ toolbox to start nidaq

% Default settings
if nargin < 3
    daqname = 'Dev1';
    if nargin < 2
        nchannels = 8;
        if nargin < 1
            frequency = 2500;
        end
    end
end

% Initialize analog inputs
open_daq = daqtoolbox.analoginput('nidaq', daqname);
addchannel(open_daq, 0: (nchannels-1));

% Set channel parameters
open_daq.TriggerType = 'Immediate';
open_daq.SampleRate = frequency;
open_daq.SamplesPerTrigger = Inf;

% start
start(open_daq);

end