function open_serial = serialinitial(com, baud, terminator)

if nargin < 3
    open_serial = serial(com, 'BaudRate', baud);
else
    open_serial = serial(com, 'BaudRate', baud, 'Terminator', terminator);
end

fopen(open_serial);

end