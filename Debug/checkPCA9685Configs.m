% checkPCA9685Configs.m
% Audit ns_config_*.m files for settings tied to the PCA9685 sunset.
% Flags any file whose values don't match the expected post-sunset defaults:
%   scheduler.indicator.enable    -> 0, false, or 2 (anything but PCA9685's 1/true)
%   optodelayTTL.type1.cuetype    -> 'LED'
%   optodelayTTL.type1.rewardtype -> 'Native'
%   optodelayTTL.type2-4.cuetype  -> 'PWMINT'

folder = uigetdir(pwd, 'Select folder to search for ns_config files');
if isequal(folder, 0)
    disp('No folder selected.');
    return;
end

files = dir(fullfile(folder, '**', 'ns_config*.m'));
if isempty(files)
    disp('No ns_config*.m files found.');
    return;
end

fprintf('Found %i ns_config file(s) under %s\n\n', numel(files), folder);

global nicfg

for i = 1 : numel(files)
    fp = fullfile(files(i).folder, files(i).name);
    nicfg = struct(); % Reset before each config so stale fields don't carry over

    try
        run(fp);
    catch me
        fprintf('%s\n  ERROR running file: %s\n\n', fp, me.message);
        continue;
    end

    indicatorEnable = getnested(nicfg, {'scheduler', 'indicator', 'enable'});
    cuetype1        = getnested(nicfg, {'optodelayTTL', 'type1', 'cuetype'});
    rewardtype1     = getnested(nicfg, {'optodelayTTL', 'type1', 'rewardtype'});

    fprintf('%s\n', fp);
    fprintf('  scheduler.indicator.enable    = %-10s%s\n', tostr(indicatorEnable), flagtagset(indicatorEnable, {0, 3}));
    fprintf('  optodelayTTL.type1.cuetype    = %-10s%s\n', tostr(cuetype1), flagtag(cuetype1, 'LED'));
    fprintf('  optodelayTTL.type1.rewardtype = %-10s%s\n', tostr(rewardtype1), flagtag(rewardtype1, 'Native'));

    for t = 2 : 4
        cuetypeN = getnested(nicfg, {'optodelayTTL', sprintf('type%i', t), 'cuetype'});
        fprintf('  optodelayTTL.type%i.cuetype    = %-10s%s\n', t, tostr(cuetypeN), flagtag(cuetypeN, 'PWMINT'));
    end
    fprintf('\n');
end

function val = getnested(s, fieldchain)
    val = [];
    for k = 1 : numel(fieldchain)
        if isstruct(s) && isfield(s, fieldchain{k})
            s = s.(fieldchain{k});
        else
            val = '<missing>';
            return;
        end
    end
    val = s;
end

function str = tostr(val)
    if ischar(val)
        str = val;
    elseif isnumeric(val) || islogical(val)
        str = mat2str(val);
    else
        str = '<unrecognized>';
    end
end

function tag = flagtag(val, expected)
    if isequal(val, expected)
        tag = '';
    else
        tag = '  <-- MISMATCH';
    end
end

function tag = flagtagset(val, acceptable)
    % Like flagtag, but accepts any value matching one of several options.
    for k = 1 : numel(acceptable)
        if isequal(val, acceptable{k})
            tag = '';
            return;
        end
    end
    tag = '  <-- MISMATCH';
end
