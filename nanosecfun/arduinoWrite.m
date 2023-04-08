function arduinoWrite(open_serial, data)
    write(open_serial, data, 'uint8');
end