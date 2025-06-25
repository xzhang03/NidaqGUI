function nanosec_hardware_testpanel(com)
% Serial
if nargin < 1
    coms = arduinoList;
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
baudrate = 19200;
indx = 1;
while true
    indx = funlist(indx);
    
    if isempty(indx)
        return;
    end
    
    switch indx
        case 1
            % Firmware version
            firmware_ver();
        case 2
            % I2c scan
            disp('I2c scan:');
            i2c_scan();
        case 3
            % PWM RGB module
            testPCA9685();
        case 4
            % DIO expander module
            testMCP23008();
        case 5
            % Dump Nanosec state
            nanosec_state();
        case 6
            % Dump opto RNG
            disp('Opto RNG of last experiment:');
            dump_rng(0);
        case 7
            % Dump ITI RNG
            disp('ITI RNG of last experiment:');
            dump_rng(1);
        case 8
            % Dump trial type RNG
            disp('Trial type RNG of last experiment:');
            dump_rng(2);
        case 9
            % Dump all serial buffer
            dump_buffer();
        case 10
            % Encoder test
            enctest_30();
        case 11
            % Food TTL test
            foodttltest();
        case 12
            % Cam TTL test
            camttltest();
    end
end

%% Anonymous functions
    % UI
    function indx = funlist(ini)
        fn = {'Firmware Version', 'I2c scan', 'Test PWM RGB', 'Test DIO expander',...
            'Dump Nanosec state', 'Dump opto RNG',...
            'Dump ITI RNG', 'Dump Trial type RNG', 'Dump all serial', '30s_encoder_test',...
            'Food TTL test', 'Cam TTL test'};
        [indx, ~] = listdlg('PromptString', sprintf('Select test %s (Nanosec)', com),...
            'SelectionMode','single', 'InitialValue', ini, 'ListString',fn);
    end
    
    % Firmware version
    function firmware_ver()
        serialin = serialinitial(com, baudrate);
        pause(0.1);
        arduinoWrite(serialin, [254 0]);
        pause(0.3);
        if arduinoGetBytes(serialin) > 20
            ver = arduinoRead(serialin, arduinoGetBytes(serialin), 'uint8');
        end
        fprintf('Nanosec firmware version: %s\n', char(ver'));
        arduinoClose(serialin);
    end
    
    % I2c scan
    function i2c_scan()
        serialin = serialinitial(com, baudrate);
        pause(0.1);
        arduinoWrite(serialin, [60 0]);
        pause(0.3);
        vec = [];
        ind = 0;
        while arduinoGetBytes(serialin) > 0
            ind = ind + 1;
            vec(ind) = arduinoRead(serialin, 1, 'uint8');
        end
        fprintf('%s', char(vec));
        arduinoClose(serialin);
    end
    
    % Dump state
    function nanosec_state()
        serialin = serialinitial(com, baudrate);
        pause(0.1);
        arduinoWrite(serialin, [9 0]);
        pause(0.3);
        vec = [];
        ind = 0;
        while arduinoGetBytes(serialin) > 0
            ind = ind + 1;
            vec(ind) = arduinoRead(serialin, 1, 'uint8');
        end
        fprintf('%s', char(vec));
        arduinoClose(serialin);
    end

    % Dump serial buffer
    function dump_buffer()
        serialin = serialinitial(com, baudrate);
        pause(0.1);
        vec = [];
        ind = 0;
        while arduinoGetBytes(serialin) > 0
            ind = ind + 1;
            vec(ind) = arduinoRead(serialin, 1, 'uint8');
        end
        fprintf('%s', char(vec));
        arduinoClose(serialin);
    end

    % Dump RNG
    function dump_rng(type)
        serialin = serialinitial(com, baudrate);
        pause(0.1);
        arduinoWrite(serialin, [64 type]);
        pause(0.3);
        if arduinoGetBytes(serialin) > 20
            RNG = arduinoRead(serialin, arduinoGetBytes(serialin), 'uint8');
        end
        disp(RNG);
        arduinoClose(serialin);
    end

    % Test MCP23008
    function testMCP23008()
        serialin = serialinitial(com, baudrate);
        pause(0.1);
        arduinoWrite(serialin, [68 0]);
        pause(0.1);
        arduinoClose(serialin);
    end

    % Test MCP23008
    function testPCA9685()
        serialin = serialinitial(com, baudrate);
        pause(0.1);
        arduinoWrite(serialin, [61 0]);
        pause(0.1);
        arduinoClose(serialin);
    end
    
    % Ping encoder
    function val = ping_enc(serialtemp)
        arduinoWrite(serialtemp, [5 0])
        val = arduinoRead(serialtemp, 1, 'int32');
    end

    % Encoder test 30s
    function enctest_30()
        % Initialize
        test_time = 30;
        test_inc = 1;
        c = 0;
        
        % Serial
        disp('30s encoder test:')
        serialin = serialinitial(com, baudrate);
        pause(0.1);
        tic;

        % First read
        val = ping_enc(serialin);
        fprintf('%i: %i\n', c, val);
        
        % Continue read
        while c < (test_time / test_inc)
            if toc > test_inc
                c = c + 1;
                val = ping_enc(serialin);
                fprintf('%i: %i\n', c, val);
                tic;
            end
        end
        
        arduinoClose(serialin);
    end

    % Food TTL test
    function foodttltest()
        nclicks = num2str(10);
        clicksui = inputdlg('How many pulses (150 ms on/150 ms off)?', 'Pulses', [1 40], {nclicks});
        nclicks = str2double(clicksui{1});
        fprintf('Sending %i clicks\n', nclicks);
        serialin = serialinitial(com, baudrate);
        pause(0.1);
        arduinoWrite(serialin, [74 nclicks]);
        pause(0.1);
        arduinoClose(serialin);
    end

    % Cam TTL test
    function camttltest()
        % Start
        serialin = serialinitial(com, baudrate);
        pause(0.1);
        arduinoWrite(serialin, [75 0]);
        pause(0.1);
        arduinoClose(serialin);
        
        % Tell it to stop
        questdlg('When to stop cam pulses', ...
	        'Stop Cam Pulsing', ...
	        'Stop','Stop');

        % Stop
        serialin = serialinitial(com, baudrate);
        pause(0.1);
        arduinoWrite(serialin, [75 0]);
        pause(0.1);
        arduinoClose(serialin);
    end
end