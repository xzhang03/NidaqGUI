function coms = arduinoList()
    old = isempty(which('serialport'));
    if old
        coms = seriallist;
    else
        coms = serialportlist;
    end
end