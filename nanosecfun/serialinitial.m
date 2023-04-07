function open_serial = serialinitial(com, baud, terminator)

if nargin < 3
    open_serial = serialport(com, baud);
else
    open_serial = serialport(com, baud);
    configureTerminator(open_serial, terminator);
end

end