function data = arduinoRead(open_serial, readlength, readtype)
    data = read(open_serial, readlength, readtype);
end