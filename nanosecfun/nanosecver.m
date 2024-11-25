function nanosecver(com)
% Check nanosec version

    if nargin < 1
        coms = arduinoList;
        if isempty(coms)
            disp('No open COM detected');
            return;
        elseif length(coms) == 1
            com = char(coms);
            fprintf('Using com: %s\n', com);
        else
            fprintf('Detected coms: ')
            disp(coms)
            return;
        end
    end

    sport = serialinitial(com, 19200);
    arduinoWrite(sport, [254 0]);
    pause(0.3);
    if arduinoGetBytes(sport) > 20
        ver = arduinoRead(sport, arduinoGetBytes(sport), 'uint8');
    end
    fprintf('Nanosec firmware version: %s\n', char(ver'));
    arduinoClose(sport);
end