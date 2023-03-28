function nanosec_hardware_testpanel(com)
% Serial
if nargin < 1
    coms = seriallist;
    if isempty(coms)
        disp('No open COM detected');
        return;
    elseif length(coms) == 1
        com = char(coms);
    else
        com = '';
    end
else
    if ~ischar(com)
        com = sprintf('COM%i',com);
    end
end

%% COM
if isempty(com)
    s = sprintf('%s ', coms);
    uiquestion = sprintf('Detected COM: %s', s); 
    com = char(coms(1));
else
    uiquestion = 'COM';
end
com_ui = inputdlg({uiquestion}, 'Serial port', [1 40], {com});

if isempty(com_ui{1})
    disp('No open COM detected');
    return;
else
    com = com_ui{1};
end

%% Tests
nanosec_serial = serial(com, 'BaudRate', 19200);
indx = 1;
while true
    indx = funlist(indx);
    
    if isempty(indx)
        return;
    end
    
    switch indx
        case 1
            % Firmware version
            firmware_ver(nanosec_serial);
        case 2
            % I2c scan
            disp('I2c scan:');
            i2c_scan(nanosec_serial)
        case 3
            % PWM RGB module
            testPCA9685(nanosec_serial)
        case 4
            % DIO expander module
            testMCP23008(nanosec_serial)
        case 5
            % Dump Nanosec state
            nanosec_state(nanosec_serial)
        case 6
            % Dump opto RNG
            disp('Opto RNG of last experiment:');
            dump_rng(nanosec_serial, 0)
        case 7
            % Dump ITI RNG
            disp('ITI RNG of last experiment:');
            dump_rng(nanosec_serial, 1)
        case 8
            % Dump trial type RNG
            disp('Trial type RNG of last experiment:');
            dump_rng(nanosec_serial, 2)
        case 9
            % Dump all serial buffer
            dump_buffer(nanosec_serial)
    end
end

%% Anonymous functions
    % UI
    function indx = funlist(ini)
        fn = {'Firmware Version', 'I2c scan', 'Test PWM RGB', 'Test DIO expander',...
            'Dump Nanosec state', 'Dump opto RNG',...
            'Dump ITI RNG', 'Dump Trial type RNG', 'Dump all serial'};
        [indx, ~] = listdlg('PromptString', sprintf('Select test %s', com),...
            'SelectionMode','single', 'InitialValue', ini, 'ListString',fn);
    end
    
    % Firmware version
    function firmware_ver(serialin)
        fopen(serialin);
        pause(0.1);
        fwrite(serialin, uint8([254 0]));
        pause(0.3);
        if serialin.BytesAvailable > 20
            ver = fread(serialin, serialin.BytesAvailable, 'uint8');
        end
        fprintf('Nanosec firmware version: %s\n', char(ver'));
        fclose(serialin);
    end
    
    % I2c scan
    function i2c_scan(serialin)
        fopen(serialin);
        pause(0.1);
        fwrite(serialin, uint8([60 0]));
        pause(0.3);
        vec = [];
        ind = 0;
        while serialin.BytesAvailable > 0
            ind = ind + 1;
            vec(ind) = fread(serialin, 1, 'uint8');
        end
        fprintf('%s', char(vec));
        fclose(serialin);
    end
    
    % Dump state
    function nanosec_state(serialin)
        fopen(serialin);
        pause(0.1);
        fwrite(serialin, uint8([9 0]));
        pause(0.3);
        vec = [];
        ind = 0;
        while serialin.BytesAvailable > 0
            ind = ind + 1;
            vec(ind) = fread(serialin, 1, 'uint8');
        end
        fprintf('%s', char(vec));
        fclose(serialin);
    end

    % Dump serial buffer
    function dump_buffer(serialin)
        fopen(serialin);
        pause(0.1);
        vec = [];
        ind = 0;
        while serialin.BytesAvailable > 0
            ind = ind + 1;
            vec(ind) = fread(serialin, 1, 'uint8');
        end
        fprintf('%s', char(vec));
        fclose(serialin);
    end

    % Dump RNG
    function dump_rng(serialin, type)
        fopen(serialin);
        pause(0.1);
        fwrite(serialin, uint8([64 type]));
        pause(0.3);
        if serialin.BytesAvailable > 20
            RNG = fread(serialin, serialin.BytesAvailable, 'uint8');
        end
        disp(RNG);
        fclose(serialin);
    end

    % Test MCP23008
    function testMCP23008(serialin)
        fopen(serialin);
        pause(0.1);
        fwrite(serialin, uint8([68 0]));
        pause(0.1);
        fclose(serialin);
    end

    % Test MCP23008
    function testPCA9685(serialin)
        fopen(serialin);
        pause(0.1);
        fwrite(serialin, uint8([61 0]));
        pause(0.1);
        fclose(serialin);
    end
end