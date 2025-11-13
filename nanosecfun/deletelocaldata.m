clear
genpath = 'C:\Users\zhanglab\Documents\MATLAB';
serverpath = 'Z:\photometry';
fp = uigetdir(genpath);
flist = dir(fullfile(fp, '*nidaq.mat'));

%% Scan
deletevec = zeros(length(flist), 1);
szthresh = 1e7;
hwait = waitbar(0);
for i = 1 : length(flist)
    if mod(i, 20) == 0
        waitbar(i/length(flist), hwait, sprintf('Processing %i/%i', i, length(flist)));
    end

    dind = strfind(flist(i).name, '-');
    flist(i).mouse = flist(i).name(1:dind(1)-1);
    flist(i).date = flist(i).name(dind(1)+1:dind(2)-1);
    
    % Too small
    if flist(i).bytes <= szthresh
        deletevec(i) = 4;
        continue;
    end

    % SZ00
    if strcmpi(flist(i).mouse, 'SZ00') || strcmpi(flist(i).mouse, 'SZ000') || strcmpi(flist(i).mouse, 'SZ0')
        deletevec(i) = 5;
        continue;
    end

    % Output path
    outputdir = fullfile(serverpath, flist(i).mouse, sprintf('%s_%s', flist(i).mouse, flist(i).date));

    % Output file
    if exist(outputdir, 'dir')
        deletevec(i) = 1;
        if exist(fullfile(outputdir, flist(i).name), 'file')
            deletevec(i) = 2;
            f = dir(fullfile(outputdir, flist(i).name));
            if f.bytes == flist(i).bytes
                deletevec(i) = 3;
            end
        end
    end
   
end
close(hwait)


%% Delete
deletelist = flist(deletevec >= 3);
keeplist = flist(deletevec < 3);
hwait = waitbar(0);
for i = 1 : length(deletelist)
    if mod(i, 20) == 0
        waitbar(i/length(deletelist), hwait, sprintf('Processing %i/%i', i, length(flist)));
    end
    delete(fullfile(deletelist(i).folder,deletelist(i).name))
    delete(fullfile(deletelist(i).folder, sprintf('%s-running.mat', deletelist(i).name(1:end-10))));
end
close(hwait)