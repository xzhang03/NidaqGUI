classdef serialport < dynamicprops
    properties (SetAccess = protected)
        Port
    end
    properties
        Opened
        BaudRate
        ByteSize
        StopBits
        Parity
        Timeout
    end
    properties (Constant)
        Type = 'Serial Port'
    end
    properties (Hidden = true)
        hwInfo
    end
    properties (Access = protected)
        AdaptorName
        DeviceID
        TaskID
    end
    properties (Access = protected, Constant)
        StopBitsSet = {'OneStopBit','One5StopBits','TwoStopBits'};
        ParitySet = {'NoParity','OddParity','EvenParity','MarkParity','SpaceParity'};
        SubsystemType = 3  % 1: AI, 2: AO, 3: DIO
    end
    
    methods (Access = protected, Static)
        function id = taskid()
            persistent counter;
            if isempty(counter), n = now; counter = floor((n - floor(n))*10^9); end
            counter = counter + 1;
            id = counter;
        end
    end
    methods (Access = protected)
        function val = numchk(obj,val,varargin) %#ok<*INUSL>
            if any(~isnumeric(val)), error('Parameter must be numeric.'); end
            switch nargin
                case 2
                    if ~isscalar(val), error('Parameters must be a scalar.'); end
                case 3
                    len = varargin{1};
                    if len < numel(val), error('Parameters greater than %d elements are currently unsupported.',len); end
                case 4
                    minval = varargin{1}; maxval = varargin{2};
                    if ~isscalar(val), error('Parameters must be a scalar.'); end
                    if val < minval, error('Property value can not be set below the minimum value constraint.'); end
                    if maxval < val, error('Property value can not be set above the maximum value constraint.'); end
                otherwise
                    error('Too many input arguments.');
            end
        end
        function [val,idx] = validateStringSet(obj,prop,val)
            if isempty(val) || ~ischar(val), error('Paramter must be a non-empty string.'); end
            idx = 0;
            propset = [prop 'Set'];
            if isprop(obj,propset)
                idx = find(strncmpi(obj.(propset),val,length(val)));
                if 1~=length(idx), error('There is no enumerated value named ''%s'' for the ''%s'' property',val,prop); end
                val = obj.(propset){idx};
            end
        end
    end        
    
    methods
        function obj = serialport(DeviceID)
            InstalledAdaptors = mdqmex(50,2);
            adaptor = 'serial';
            idx = strncmpi(InstalledAdaptors,adaptor,length(adaptor));
            if ~any(idx), error('Failure to find requested communication device: %s.',adaptor); end
            adaptor = InstalledAdaptors{idx};
            hw = daqhwinfo(adaptor);
            if isscalar(DeviceID), DeviceID = sprintf('COM%d',DeviceID); end
            idx = strcmpi(hw.InstalledBoardIds,DeviceID);
            if ~any(idx), error('Constructors require a device ID, e.g. serialport(''COM1'').'); end
            DeviceID = hw.InstalledBoardIds{idx};
            
            obj.Port = DeviceID;
            obj.AdaptorName = adaptor;
            obj.DeviceID = DeviceID;
            obj.TaskID = obj.taskid();
            mdqmex(53,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID);
            obj.hwInfo = about(obj);
        end
        function delete(obj)
            for m=1:length(obj)
                if isempty(obj(m).AdaptorName), continue, end
                mdqmex(54,obj(m).AdaptorName,obj(m).DeviceID,obj(m).SubsystemType,obj(m).TaskID);
            end
        end
        function info = about(obj)
            info = mdqmex(52,obj.AdaptorName,obj.DeviceID,obj.SubsystemType);
        end
        
        function open(obj)  % start
            for m=1:length(obj)
                mdqmex(57,obj(m).AdaptorName,obj(m).DeviceID,obj(m).SubsystemType,obj(m).TaskID);
            end
        end
        function close(obj)  % stop
            for m=1:length(obj)
                mdqmex(58,obj(m).AdaptorName,obj(m).DeviceID,obj(m).SubsystemType,obj(m).TaskID);
            end
        end
        function tf = isopen(obj)  % isrunning
            nobj = length(obj);
            tf = false(1,nobj);
            for m=1:nobj
                tf(m) = mdqmex(59,obj(m).AdaptorName,obj(m).DeviceID,obj(m).SubsystemType,obj(m).TaskID);
            end
        end
        function register(~,~), end
        
        function write(obj,val)  % putvalue
            for m=1:length(obj)
                if isnumeric(val), val = num2str(val); end
                if ~ischar(val), error('Please write char strings only.'); end
                mdqmex(89,obj(m).AdaptorName,obj(m).DeviceID,obj(m).TaskID,0,val);
            end                
        end
        function val = available(obj)
            val = mdqmex(89,obj.AdaptorName,obj.DeviceID,obj.TaskID,1);
        end
        function val = read(obj)  % getvalue
            val = mdqmex(89,obj.AdaptorName,obj.DeviceID,obj.TaskID,2);
        end
        function val = readString(obj)
            val = mdqmex(89,obj.AdaptorName,obj.DeviceID,obj.TaskID,3);
        end
        function val = readStringUntil(obj,ch)
            if ~ischar(ch) || 1~=numel(ch), error('The terminator must be a char.'); end
            val = mdqmex(89,obj.AdaptorName,obj.DeviceID,obj.TaskID,4,ch);
        end
        function val = readln(obj,timeout)  % sec
            if ~exist('timeout','var'), timeout = 10; end
            timeout_timer = tic;
            while ~available(obj) && toc(timeout_timer)<timeout, end
            val = strtrim(mdqmex(89,obj.AdaptorName,obj.DeviceID,obj.TaskID,4,char(10)));
        end

        function set.Opened(obj,val) %#ok<*INUSD>
            error('Attempt to modify read-only property: ''Opened''.');
        end
        function val = get.Opened(obj)
            val = isopen(obj);
        end
        function set.BaudRate(obj,val)
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'BaudRate',numchk(obj,val)); %#ok<*MCSUP>
        end
        function val = get.BaudRate(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'BaudRate');
        end
        function set.ByteSize(obj,val)
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'ByteSize',numchk(obj,val));
        end
        function val = get.ByteSize(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'ByteSize');
        end
        function set.StopBits(obj,val)
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'StopBits',validateStringSet(obj,'StopBits',val));
        end
        function val = get.StopBits(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'StopBits');
        end
        function set.Parity(obj,val)
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Parity',validateStringSet(obj,'Parity',val));
        end
        function val = get.Parity(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Parity');
        end
        function set.Timeout(obj,val)
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Timeout',numchk(obj,val));
        end
        function val = get.Timeout(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Timeout');
        end
        
        function out = set(obj,varargin)
            switch nargin
                case 1
                    out = [];
                    fields = properties(obj(1));
                    for m=1:length(fields)
                        propset = [fields{m} 'Set'];
                        if isprop(obj(1),propset), out.(fields{m}) = obj(1).(propset); else out.(fields{m}) = {}; end
                    end
                    return;
                case 2
                    if ~isscalar(obj), error('Object array must be a scalar when using SET to retrieve information.'); end
                    fields = varargin(1);
                    vals = {{}};
                case 3
                    if iscell(varargin{1})
                        fields = varargin{1};
                        vals = varargin{2};
                        [a,b] = size(vals);
                        if length(obj) ~= a || length(fields) ~= b, error('Size mismatch in Param Cell / Value Cell pair.'); end
                    else
                        fields = varargin(1);
                        vals = varargin(2);
                    end
                otherwise
                    if 0~=mod(nargin-1,2), error('Invalid parameter/value pair arguments.'); end
                    fields = varargin(1:2:end);
                    vals = varargin(2:2:end);
            end
            for m=1:length(obj)
                proplist = properties(obj(m));
                for n=1:length(fields)
                    field = fields{n};
                    if ~ischar(field), error('Invalid input argument type to ''set''.  Type ''help set'' for options.'); end
                    if 1==size(vals,1), val = vals{1,n}; else val = vals{m,n}; end
                    
                    idx = strncmpi(proplist,field,length(field));
                    if 1~=sum(idx), error('The property, ''%s'', does not exist.',field); end
                    prop = proplist{idx};
                    
                    if ~isempty(val)
                        obj(m).(prop) = val;
                    else
                        propset = [prop 'Set'];
                        if isprop(obj(m),propset)
                            out = obj(m).(propset)(:);
                        else
                            fprintf('The ''%s'' property does not have a fixed set of property values.\n',prop);
                        end
                    end
                end
            end
        end
        function out = get(obj,fields)
            if ischar(fields), fields = {fields}; end
            out = cell(length(obj),length(fields));
            for m=1:length(obj)
                proplist = properties(obj(m));
                for n=1:length(fields)
                    field = fields{n};
                    idx = strncmpi(proplist,field,length(field));
                    if 1~=sum(idx), error('The property, ''%s'', does not exist.',field); end
                    prop = proplist{idx};
                    out{m,n} = obj(m).(prop);
                end
            end
            if isscalar(out), out = out{1}; end
        end
    end
end
