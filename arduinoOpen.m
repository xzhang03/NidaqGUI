function open_serial = arduinoOpen(com, baumrate)
%ARDUINOPEN Opens a connection to an Arduino over the COM connection
%   specified by the int com and returns the open connection. Close with
%   fclose(open_serial)
    
    if nargin < 2
        baumrate = 9600;
    end

    open_serial = serial(sprintf('COM%i', com), 'Terminator', '', 'BaudRate', baumrate);
    fopen(open_serial);

end

