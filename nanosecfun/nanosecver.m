function nanosecver(com)
% Check nanosec version
    sport = serialinitial(com, 19200);
    arduinoWrite(sport, [254 0]);
    pause(0.3);
    if arduinoGetBytes(sport) > 20
        ver = arduinoRead(sport, arduinoGetBytes(sport), 'uint8');
    end
    fprintf('Nanosec firmware version: %s\n', char(ver'));
    arduinoClose(sport);
end