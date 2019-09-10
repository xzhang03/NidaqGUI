classdef videocapture < dynamicprops
    properties
        FrameRate
        Logging
        Name
        Running
        SamplesAcquired
        SamplesAvailable
        SupportedFormat
        SelectedFormat
        TriggerType
        VerticalFlip
        Brightness
        Contrast
        Hue
        Saturation
        Sharpness
        Gamma
        ColorEnable
        WhiteBalance
        BacklightCompensation
        Gain
        Pan
        Tilt
        Roll
        Zoom
        Exposure
        Iris
        Focus
    end
    properties (SetAccess = protected, Hidden)
        Width
        Height
        FrameRateRange
        BrightnessRange
        ContrastRange
        HueRange
        SaturationRange
        SharpnessRange
        GammaRange
        ColorEnableRange
        WhiteBalanceRange
        BacklightCompensationRange
        GainRange
        PanRange
        TiltRange
        RollRange
        ZoomRange
        ExposureRange
        IrisRange
        FocusRange
    end
    properties (Constant)
        Type = 'Webcam'
    end
    properties (Access = protected)
        hwInfo
        AdaptorName
        DeviceID
        TaskID
    end
    properties (Access = protected, Constant)
        TriggerTypeSet = {'Immediate','Manual'}
        SubsystemType = 1  % 1: AI, 2: AO, 3: DIO
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
        function val = numchk2(obj,val,varargin)
            if any(~isnumeric(val)), error('Parameter must be numeric.'); end
            if 2 < numel(val), error('Parameter must be a scalar or a two-element vector.'); end
            switch nargin
                case 3
                    if ~isempty(varargin{1})
                        minval = varargin{1}(1); maxval = varargin{1}(2);
                        if val < minval, error('Property value can not be set below the minimum value constraint.'); end
                        if maxval < val, error('Property value can not be set above the maximum value constraint.'); end
                    end
                    if 1==numel(val), val = [val 2]; elseif val(2) < 1 || 2 < val(2), error('The flag must be either 1 or 2.'); end
                otherwise
                    error('There must be 3 input arguments.');
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
        function val = formatoutput(obj,val,range)
%             if isempty(val), return, end
%             if isempty(range)
%                 switch val(2)
%                     case 1, val = sprintf('%d (auto)',val(1));
%                     case 2, val = sprintf('%d (manual)',val(1));
%                 end
%             else
%                 switch val(2)
%                     case 1, val = sprintf('Auto (%d - %d)',range(1:2));
%                     case 2, val = sprintf('%d (%d - %d)',val(1),range(1:2));
%                     otherwise, error('Unknown flag value!!!');
%                 end
%             end
        end
    end
    
    methods
        function field = settables(~)
            field = {'SelectedFormat','VerticalFlip','FrameRate','TriggerType','Brightness','Contrast','Hue','Saturation','Sharpness', ...
                'Gamma','ColorEnable','WhiteBalance','BacklightCompensation','Gain','Pan','Tilt','Roll','Zoom','Exposure','Iris','Focus'};
        end
        function property = export(obj)
            property = struct; for m=settables(obj), property.(m{1}) = obj.(m{1}); end
        end
        function import(obj,property)
            if isstruct(property), for m=settables(obj), try obj.(m{1}) = property.(m{1}); catch, end, end, end
        end
        function obj = videocapture(adaptor,DeviceID)
            if ~exist('DeviceID','var'), DeviceID = '0'; end
            InstalledAdaptors = mdqmex(50,2);
            idx = strncmpi(InstalledAdaptors,adaptor,length(adaptor));
            if ~any(idx), error('Failure to find requested data acquisition device: %s.',adaptor); end
            adaptor = InstalledAdaptors{idx};
            hw = daqhwinfo(adaptor);
            if isscalar(DeviceID), DeviceID = num2str(DeviceID); end
            idx = strcmpi(hw.InstalledBoardIds,DeviceID);
            if ~any(idx), error('The specified device ID is invalid. Use DAQHWINFO(adaptorname) to determine valid device IDs.'); end
            if isempty(hw.ObjectConstructorName{idx,obj.SubsystemType}), error('This device does not support the subsystem requested.  Use DAQHWINFO(adaptorname) to determine valid constructors.'); end
            DeviceID = hw.InstalledBoardIds{idx};

            obj.AdaptorName = adaptor;
            obj.DeviceID = DeviceID;
            obj.TaskID = obj.taskid();
            mdqmex(53,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID);
            obj.hwInfo = about(obj);
            obj.Name = [DeviceID ': ' hw.BoardNames{idx}];
        end
        function delete(obj)
            for m=1:length(obj)
                mdqmex(54,obj(m).AdaptorName,obj(m).DeviceID,obj(m).SubsystemType,obj(m).TaskID);
            end
        end
        function info = about(obj)
            info = mdqmex(52,obj.AdaptorName,obj.DeviceID,obj.SubsystemType);
        end
        
        function start(obj)
            for m=1:length(obj)
                if isrunning(obj(m)), error('OBJ has already started.'); end
                mdqmex(57,obj(m).AdaptorName,obj(m).DeviceID,obj(m).SubsystemType,obj(m).TaskID);
            end
        end
        function stop(obj)
            for m=1:length(obj)
                mdqmex(58,obj(m).AdaptorName,obj(m).DeviceID,obj(m).SubsystemType,obj(m).TaskID);
            end
        end
        function tf = islogging(obj)
            nobj = length(obj);
            tf = false(1,nobj);
            for m=1:nobj
                tf(m) = mdqmex(71,obj(m).AdaptorName,obj(m).DeviceID,obj(m).SubsystemType,obj(m).TaskID);
            end
        end
        function tf = isrunning(obj)
            nobj = length(obj);
            tf = false(1,nobj);
            for m=1:nobj
                tf(m) = mdqmex(59,obj(m).AdaptorName,obj(m).DeviceID,obj(m).SubsystemType,obj(m).TaskID);
            end
        end
        function trigger(obj)
            for m=1:length(obj)
                if ~strcmp(obj(m).TriggerType,'Manual'), error('TRIGGER is only valid when TriggerType is set to ''Manual''.'); end
                if ~isrunning(obj(m)), error('OBJ must be running before TRIGGER is called. Call START.'); end
                if islogging(obj(m)), error('TRIGGER cannot be called when OBJ is logging.'); end
                mdqmex(70,obj(m).AdaptorName,obj(m).DeviceID,obj(m).SubsystemType,obj(m).TaskID);
            end
        end
        
        function marker_position = flushmarker(obj)
            marker_position = mdqmex(77,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,1);
        end
        function flushdata(obj,mode) % Since we don't do triggering for this type of devices, the mode doesn't matter.
            for m=1:length(obj)
                mdqmex(72,obj(m).AdaptorName,obj(m).DeviceID,obj(m).SubsystemType,obj(m).TaskID,false);
            end
        end
        function data = getdata(obj,nsamples)
            if 1 < length(obj), error('OBJ must be a 1-by-1 analog input object.'); end
            switch nargin
                case 1, data = mdqmex(82,obj.AdaptorName,obj.DeviceID,obj.TaskID,1);
                otherwise, data = mdqmex(82,obj.AdaptorName,obj.DeviceID,obj.TaskID,1,nsamples);
            end
            data.Frame = permute(data.Frame,[2 1 3]);
        end
        function data = peekdata(obj,nsamples)
            if 1 < length(obj), error('OBJ must be a 1-by-1 analog input object.'); end
            switch nargin
                case 1, data = mdqmex(82,obj.AdaptorName,obj.DeviceID,obj.TaskID,2);
                otherwise, data = mdqmex(82,obj.AdaptorName,obj.DeviceID,obj.TaskID,2,nsamples);
            end
            data.Frame = permute(data.Frame,[2 1 3]);
        end
        function [data,SampleTime] = getsample(obj)
            if 1 < length(obj), error('OBJ must be a 1-by-1 pointing device object.'); end
            data = mdqmex(82,obj.AdaptorName,obj.DeviceID,obj.TaskID,3);
            data.Frame = permute(data.Frame,[2 1 3]);
        end
        function marker_position = frontmarker(obj)
            marker_position = mdqmex(77,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,2);
        end
        function data = peekfront(obj)
            if 1 < length(obj), error('OBJ must be a 1-by-1 pointing device object.'); end
            data = mdqmex(82,obj.AdaptorName,obj.DeviceID,obj.TaskID,4);
            data.Frame = permute(data.Frame,[2 1 3]);
        end
        function marker_position = backmarker(obj)
            marker_position = mdqmex(77,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,3);
        end
        function data = getback(obj)
            if 1 < length(obj), error('OBJ must be a 1-by-1 pointing device object.'); end
            data = mdqmex(82,obj.AdaptorName,obj.DeviceID,obj.TaskID,5);
            data.Frame = permute(data.Frame,[2 1 3]);
        end
        function register(obj,name)
            if ~exist('name','var'), name = ''; end
            for m=1:length(obj)
                mdqmex(90,obj(m).AdaptorName,obj(m).DeviceID,5,obj(m).TaskID,name);
            end
        end
        function nsamps = TotalAcquired(obj)
            nsamps = mdqmex(80,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID);
        end
        
        function set.FrameRate(obj,val)
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'FrameRate',numchk(obj,val,obj.FrameRateRange(1),obj.FrameRateRange(2)));
        end
        function val = get.FrameRate(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'FrameRate');
        end
        function set.Logging(obj,val)
            error('Attempt to modify read-only property: ''Logging''.');
        end
        function val = get.Logging(obj)
            val = islogging(obj);
        end
        function set.Running(obj,val) %#ok<*INUSD>
            error('Attempt to modify read-only property: ''Running''.');
        end
        function val = get.Running(obj)
            val = isrunning(obj);
        end
        function set.SamplesAcquired(obj,val)
            error('Attempt to modify read-only property: ''SamplesAcquired''.');
        end
        function val = get.SamplesAcquired(obj)
            val = mdqmex(64,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID);
        end
        function set.SamplesAvailable(obj,val)
            error('Attempt to modify read-only property: ''SamplesAvailable''.');
        end
        function val = get.SamplesAvailable(obj)
            val = mdqmex(65,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID);
        end
        function set.SupportedFormat(obj,val)
            error('Attempt to modify read-only property: ''SamplesAvailable''.');
        end
        function val = get.SupportedFormat(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'SupportedFormat');
        end
        function set.SelectedFormat(obj,val)
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'SelectedFormat',numchk(obj,val)); %#ok<*MCSUP>
        end
        function val = get.SelectedFormat(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'SelectedFormat');
        end
        function set.TriggerType(obj,val)
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'TriggerType',validateStringSet(obj,'TriggerType',val));
        end
        function val = get.TriggerType(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'TriggerType');
        end
        function set.VerticalFlip(obj,val)
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'VerticalFlip',numchk(obj,double(logical(val))));
        end
        function val = get.VerticalFlip(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'VerticalFlip');
        end

        function set.Brightness(obj,val)
%             if isempty(obj.BrightnessRange), error('This capture device does not support brightness adjustment.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Brightness',numchk2(obj,val,obj.BrightnessRange));
        end
        function val = get.Brightness(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Brightness'),obj.BrightnessRange);
        end
        function set.Contrast(obj,val)
%             if isempty(obj.ContrastRange), error('This capture device does not support contrast adjustment.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Contrast',numchk2(obj,val,obj.ContrastRange));
        end
        function val = get.Contrast(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Contrast'),obj.ContrastRange);
        end
        function set.Hue(obj,val)
%             if isempty(obj.HueRange), error('This capture device does not support hue adjustment.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Hue',numchk2(obj,val,obj.HueRange));
        end
        function val = get.Hue(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Hue'),obj.HueRange);
        end
        function set.Saturation(obj,val)
%             if isempty(obj.SaturationRange), error('This capture device does not support saturation adjustment.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Saturation',numchk2(obj,val,obj.SaturationRange));
        end
        function val = get.Saturation(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Saturation'),obj.SaturationRange);
        end
        function set.Sharpness(obj,val)
%             if isempty(obj.SharpnessRange), error('This capture device does not support sharpness adjustment.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Sharpness',numchk2(obj,val,obj.SharpnessRange));
        end
        function val = get.Sharpness(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Sharpness'),obj.SharpnessRange);
        end
        function set.Gamma(obj,val)
%             if isempty(obj.GammaRange), error('This capture device does not support gamma adjustment.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Gamma',numchk2(obj,val,obj.GammaRange));
        end
        function val = get.Gamma(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Gamma'),obj.GammaRange);
        end
        function set.ColorEnable(obj,val)
%             if isempty(obj.ColorEnableRange), error('This capture device does not support changing between color and black/white.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'ColorEnable',numchk2(obj,val,obj.ColorEnableRange));
        end
        function val = get.ColorEnable(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'ColorEnable'),obj.ColorEnableRange);
        end
        function set.WhiteBalance(obj,val)
%             if isempty(obj.WhiteBalanceRange), error('This capture device does not support white balance adjustment.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'WhiteBalance',numchk2(obj,val,obj.WhiteBalanceRange));
        end
        function val = get.WhiteBalance(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'WhiteBalance'),obj.WhiteBalanceRange);
        end
        function set.BacklightCompensation(obj,val)
%             if isempty(obj.BacklightCompensationRange), error('This capture device does not support turning on/off backlight compensation.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'BacklightCompensation',numchk2(obj,val,obj.BacklightCompensationRange));
        end
        function val = get.BacklightCompensation(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'BacklightCompensation'),obj.BacklightCompensationRange);
        end
        function set.Gain(obj,val)
%             if isempty(obj.GainRange), error('This capture device does not support gain adjustment.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Gain',numchk2(obj,val,obj.GainRange));
        end
        function val = get.Gain(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Gain'),obj.GainRange);
        end
        function set.Pan(obj,val)
%             if isempty(obj.PanRange), error('This capture device does not support pan adjustment.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Pan',numchk2(obj,val,obj.PanRange));
        end
        function val = get.Pan(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Pan'),obj.PanRange);
        end
        function set.Tilt(obj,val)
%             if isempty(obj.TiltRange), error('This capture device does not support tilt adjustment.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Tilt',numchk2(obj,val,obj.TiltRange));
        end
        function val = get.Tilt(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Tilt'),obj.TiltRange);
        end
        function set.Roll(obj,val)
%             if isempty(obj.RollRange), error('This capture device does not support roll adjustment.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Roll',numchk2(obj,val,obj.RollRange));
        end
        function val = get.Roll(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Roll'),obj.RollRange);
        end
        function set.Zoom(obj,val)
%             if isempty(obj.ZoomRange), error('This capture device does not support zoom adjustment.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Zoom',numchk2(obj,val,obj.ZoomRange));
        end
        function val = get.Zoom(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Zoom'),obj.ZoomRange);
        end
        function set.Exposure(obj,val)
%             if isempty(obj.ExposureRange), error('This capture device does not support exposure adjustment.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Exposure',numchk2(obj,val,obj.ExposureRange));
        end
        function val = get.Exposure(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Exposure'),obj.ExposureRange);
        end
        function set.Iris(obj,val)
%             if isempty(obj.IrisRange), error('This capture device does not support iris adjustment.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Iris',numchk2(obj,val,obj.IrisRange));
        end
        function val = get.Iris(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Iris'),obj.IrisRange);
        end
        function set.Focus(obj,val)
%             if isempty(obj.FocusRange), error('This capture device does not support focus adjustment.'); end
            mdqmex(55,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Focus',numchk2(obj,val,obj.FocusRange));
        end
        function val = get.Focus(obj)
            val = formatoutput(obj,mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Focus'),obj.FocusRange);
        end
        
        function val = get.Width(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Width');
        end
        function val = get.Height(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'Height');
        end
        function val = get.FrameRateRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'FrameRateRange');
        end
        function val = get.BrightnessRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'BrightnessRange');
        end
        function val = get.ContrastRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'ContrastRange');
        end
        function val = get.HueRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'HueRange');
        end
        function val = get.SaturationRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'SaturationRange');
        end
        function val = get.SharpnessRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'SharpnessRange');
        end
        function val = get.GammaRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'GammaRange');
        end
        function val = get.ColorEnableRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'ColorEnableRange');
        end
        function val = get.WhiteBalanceRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'WhiteBalanceRange');
        end
        function val = get.BacklightCompensationRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'BacklightCompensationRange');
        end
        function val = get.GainRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'GainRange');
        end
        function val = get.PanRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'PanRange');
        end
        function val = get.TiltRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'TiltRange');
        end
        function val = get.RollRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'RollRange');
        end
        function val = get.ZoomRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'ZoomRange');
        end
        function val = get.ExposureRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'ExposureRange');
        end
        function val = get.IrisRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'IrisRange');
        end
        function val = get.FocusRange(obj)
            val = mdqmex(56,obj.AdaptorName,obj.DeviceID,obj.SubsystemType,obj.TaskID,'FocusRange');
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
