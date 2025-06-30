function plotfoodTTL()
% Viaualize food TTL programs

tf = evalin('base','exist(''nicfg'')');

if tf
    nicfg = evalin('base', 'nicfg');
else
    disp('No config file loaded');
    return;
end

%% Define window
% Pre and post windows
prew = (-nicfg.optodelayTTL.lead - 1) * 1000;
postw = 10 * 1000;

% Starting point
if nicfg.optodelayTTL.optothenTTL
    t0 = 0;
else
    t0 = -nicfg.optodelayTTL.lead;
end

% Cues
tcueon = (nicfg.optodelayTTL.cuedelay + t0) * 100;
tcueoff = (nicfg.optodelayTTL.cuedur + nicfg.optodelayTTL.cuedelay + t0) * 100;

% Actions
tactionon = (nicfg.optodelayTTL.actiondelay + t0) * 100;
tactionoff = (nicfg.optodelayTTL.actiondur + nicfg.optodelayTTL.actiondelay + t0) * 100;

% Food
tfoodon = (nicfg.optodelayTTL.delay + t0) * 100;
tfoodoff = (nicfg.optodelayTTL.deliverydur + nicfg.optodelayTTL.delay + t0) * 100;

% Trains
% Trial type 0
traincell = cell(4,1);
for itrial = 1 : 4
    train_l = nicfg.optodelayTTL.cycle(itrial) * (nicfg.optodelayTTL.trainlength(itrial)-1) + nicfg.optodelayTTL.pulsewidth(itrial);
    train_mat = (1 : nicfg.optodelayTTL.cycle(itrial) : nicfg.optodelayTTL.cycle(itrial) * nicfg.optodelayTTL.trainlength(itrial))';
    train_mat(:,2) = (nicfg.optodelayTTL.pulsewidth(itrial) : nicfg.optodelayTTL.cycle(itrial) : train_l)';
    trainvec = zeros(train_l, 1);
    for i = 1 : nicfg.optodelayTTL.trainlength(itrial)
        trainvec(train_mat(i,1):train_mat(i,2)) = 1;
    end
    trainvec = cat(1, 0, trainvec, 0);
    traincell{itrial,1} = (-10 : 10: train_l * 10)';
    traincell{itrial,2} = trainvec;
end

%% Probabilities
freqs = nicfg.optodelayTTL.trialfreq;
freqs(nicfg.optodelayTTL.ntrialtypes+1 : end) = 0;
freqsum = sum(freqs);

%% Plot
ys = [1.3 1.2 1.1];
figure('Position', [50 200 1800 420])
for iplot = 1 : 4
    subplot(1,4,iplot)
    hold on
    
    % Opto
    plot([0 0], [0 1.5], 'k--')
    
    % Cue
    if nicfg.optodelayTTL.cueenable
        % Cue color
        trialcue = nicfg.optodelayTTL.(sprintf('type%i', iplot));
        if strcmp(trialcue.cuetype, 'Buzzer')
            cuec = [0 0 0];
        elseif strcmp(trialcue.cuetype, 'DIO')
            cuec = [0.5 0.5 0.5];
        else
            cuec = trialcue.RGB;
            cuec = cuec / max(cuec);
        end
        plot([tcueon tcueoff(iplot)], [ys(1) ys(1)], 'Color', cuec, 'LineWidth', 3)
        text(tcueon, ys(1) + 0.05, 'Cue', 'Color', cuec);
    end

    % Action
    if nicfg.optodelayTTL.conditional(iplot)
        % Action window
        plot([tactionon tactionoff(iplot)], [ys(2) ys(2)], 'Color', [0.5 0.5 0.5], 'LineWidth', 3)
        text(tactionon, ys(2) + 0.05, 'Action', 'Color', [0.5 0.5 0.5]);

        % Earliest time
        tfirstfood = max(tfoodon, tactionon);

        % Conditional or pav
        conpav = 'conditional';
    else
        tfirstfood = tfoodon;
        conpav = 'pavlovian';
    end

    % Delivery window
    plot([tfoodon tfoodoff(iplot)], [ys(3) ys(3)], 'Color', [0 0 0], 'LineWidth', 3)
    text(tfoodon, ys(3) + 0.05, 'Delivery', 'Color', [0 0 0]);
    
    % Pulses
    plot(traincell{iplot,1} + tfirstfood,  traincell{iplot,2}, 'Color', [1 0.5 0.1])
    
    % Reward type
    switch trialcue.rewardtype
        case 'DIO'
            rewardtype = sprintf('DIO%i', trialcue.DIOport);
        case 'Native'
            rewardtype = 'Native';
    end
    
    text(traincell{iplot,1}(end) + tfirstfood+100, 1, rewardtype)


    xlim([prew postw])
    ylim([0 1.5])

    xlabel('Time from opto start (ms)')
    title(sprintf('Type %i: %s (freq = %i/%i)', iplot, conpav, freqs(iplot), freqsum))


end


end