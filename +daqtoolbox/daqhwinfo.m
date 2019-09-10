function hw = daqhwinfo(varargin)

switch nargin
    case 0
        [hw.ToolboxName,hw.ToolboxVersion] = daqtoolbox.daq.getToolboxInfo;
        v = ver('MATLAB');
        hw.MATLABVersion = [v.Version ' ' v.Release];
        hw.InstalledAdaptors = daqtoolbox.mdqmex(50,1);
    case 1
        switch class(varargin{1})
            case 'char'
                if strcmpi(varargin{1},'all'), hw = mdqmex(50); return, end
                
                InstalledAdaptors = daqtoolbox.mdqmex(50);
                idx = strncmpi(InstalledAdaptors,varargin{1},length(varargin{1}));
                if ~any(idx), error('Failure to find requested data acquisition device: %s',varargin{1}); end
                AdaptorName = InstalledAdaptors{idx};
                
                hw.AdaptorName = AdaptorName;
                [hw.BoardNames,hw.InstalledBoardIds,Subsystem] = daqtoolbox.mdqmex(51,AdaptorName);
                for m=1:length(hw.InstalledBoardIds)
                    for n=1:3
                        if isempty(Subsystem{m,n}), continue; end
                        Subsystem{m,n} = [Subsystem{m,n} '('''  hw.AdaptorName ''',''' hw.InstalledBoardIds{m} ''')'];
                    end
                end
                hw.ObjectConstructorName = Subsystem;
            case {'analoginput','analogoutput','digitalio','pointingdevice','eyetracker','videocapture'}
                hw = about(varargin{1});
            otherwise
                error('Unknown device type!!!');
        end
end
