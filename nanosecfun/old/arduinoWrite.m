function arduinoWrite(open_serial, data)
    fwrite(open_serial, uint8(data));
end