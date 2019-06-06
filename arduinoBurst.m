function arduinoBurst(open_serial)
%ARDUINOBURST sends a burst command to arduino. Default is 30Hz for 2
%seconds.

    
    if ~isempty(open_serial)
        fwrite(open_serial, 1);
    end
end

