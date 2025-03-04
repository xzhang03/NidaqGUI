function pingCOMs()
% Ping all the available COMs with self-id code [252 0]
% This works for nanosec (v3.53) and picodaq (v1.32)

COMs = arduinoList;
ncoms = length(COMs);
baudrate = 19200;

for i = 1 : ncoms
    com = COMs{i};
    serialin = serialinitial(com, baudrate);
    pause(0.1);
    arduinoWrite(serialin, [252 0]);
    pause(0.3);
    if arduinoGetBytes(serialin) > 4
        echo = arduinoRead(serialin, arduinoGetBytes(serialin), 'uint8');
        fprintf('%s: %s\n', com, char(echo'));
    else
        fprintf('%s: Nothing\n', com);
    end
    
    arduinoClose(serialin);
end

end