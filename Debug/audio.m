recObj = audiorecorder(8000, 8, 1, 1);

%% Recording
recDuration = 5;
disp("Begin speaking.")
% recordblocking(recObj,recDuration);
recObj.record;
pause(5)
stop(recObj);
disp("End of recording.")

%% Play back
play(recObj);

%% Data
y = getaudiodata(recObj, 'double');
plot(y)

%% Info
info = audiodevinfo;