function bytes = arduinoGetBytes(open_serial)
    bytes = open_serial.NumBytesAvailable;
end