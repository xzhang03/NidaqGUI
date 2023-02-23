fpath =...
    'C:\Users\steph\OneDrive\Documents\MATLAB\GitHub\Lab\NidaqGUI\Debug\picodaq\test4.mat';

%% Start
openDAQ = startpicoDAQ(fpath, {'daqcom', 'COM28', 'maxsamples', [], ...
    'databuffer', 7500});
disp('Start');
pause(1800)
stoppicoDAQ(openDAQ,[],[],[]);
disp('End');
load(fpath)