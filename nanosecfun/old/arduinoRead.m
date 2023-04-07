function data = arduinoRead(open_serial, readlength, readtype)
    data = fread(open_serial, readlength, readtype);
end