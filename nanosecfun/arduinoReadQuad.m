function val = arduinoReadQuad(open_serial)
%ARDUINOREADQUAD Read from an Arduino over an open connection. The Arduino
%   must be configured to give only the position of the rotary encoder in
%   integers.

    if ~isempty(open_serial) && open_serial.BytesAvailable >= 4
%         fwrite(open_serial, uint8(5));
%         val = fread(open_serial, 1, 'int32');
        val = arduinoRead(open_serial, 1, 'int32');
    else
        val = [];
    end
end

