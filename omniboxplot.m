function omniboxplot(nicfg)
%% Parameters
% Panel size
psize = 200;
color1 = [0, 0.4470, 0.7410];
color2 = [0.8500, 0.3250, 0.0980];
color3 = [0.5 0.5 0.5];
%% Plot the box setting
% N panels
npanels = 2 + nicfg.scheduler.enable + nicfg.optodelayTTL.enable;

% Figure
hfig = figure;
hfig.Position(1) = psize/2;
hfig.Position(3) = npanels * psize + psize;
hfig.Position(4) = psize;

% Photometry mode
ipanel = 1;
subplot(1,npanels,ipanel);
npts = 100;
cycles = (1 : npts / 20)*20;
cycles(end) = [];

% TCP
if nicfg.tcp.enable
    cycles2 = cycles + 10;
    traces = zeros(npts,2);
    traces(:,1) = 5;
    
    for i = 1 : length(cycles)
        traces(cycles(i):cycles(i)+6,1) = 8;
        traces(cycles2(i):cycles2(i)+6,2) = 3;
    end
    plot(1:npts, traces)
    title('Two-color photometry (ms)')
    
    hold on
    plot(cycles(1) * [1 1], [0 10],'--','Color', color3)
    plot((cycles(2) - 1) * [1 1], [0 10],'--','Color', color3)
    hold off
    
    text(cycles(1), 9, '6', 'Color', color1)
    text(cycles(1) + 7, 6, '14', 'Color', color1)
    text(cycles(1) + 10, 4, '6', 'Color', color2)
    text(cycles(1) + 17, 1, '14', 'Color', color2)
    text(cycles(2), 9, 'Cycle = 20', 'Color', color3)

% Optophotometry
elseif nicfg.optophotometry.enable
    cycles2 = cycles + 6;
    traces = zeros(npts,2);
    traces(:,1) = 5;
    
    for i = 1 : length(cycles)
        traces(cycles(i):cycles(i)+6,1) = 8;
        if i == 1
            traces(cycles2(i):cycles2(i)+10,2) = 3;
        end
    end
    plot(1:npts, traces)
    title('Optophotometry (ms)')
    
    hold on
    plot(cycles(1) * [1 1], [0 10],'--','Color', color3)
    plot((cycles(2) - 1) * [1 1], [0 10],'--','Color', color3)
    hold off
    
    text(cycles(1), 9, '6', 'Color', color1)
    text(cycles(1) + 7, 6, '14', 'Color', color1)
    text(cycles(1) + 6, 4, '10', 'Color', color2)
    text(cycles(1) + 17, 1, '4', 'Color', color2)
    text(cycles(2) + 5, 2, sprintf('%2.1f Hz', 50/nicfg.optophotometry.freqmod), 'Color', color2)
    text(cycles(2) + 5, 1, sprintf('%2.1f s / %2.0f s', nicfg.optophotometry.freqmod/...
        50*nicfg.optophotometry.trainlength, nicfg.optophotometry.cycle), 'Color', color2)
    text(cycles(2), 9, 'Cycle = 20', 'Color', color3)

% SCoptophotometry
elseif nicfg.scoptophotometry.enable
    cycles2 = cycles;
    traces = zeros(npts,2);
    traces(:,1) = 5;
    
    for i = 1 : length(cycles)
        traces(cycles(i):cycles(i)+6,1) = 8;
        if i == 1
            traces(cycles2(i):cycles2(i)+nicfg.scoptophotometry.pulsewidth,2) = 3;
        end
    end
    plot(1:npts, traces, 'Color', color1)
    title('SCoptophotometry (ms)')
    
    hold on
    plot(cycles(1) * [1 1], [0 10],'--','Color', color3)
    plot((cycles(2) - 1) * [1 1], [0 10],'--','Color', color3)
    hold off
    
    text(cycles(1), 9, '6', 'Color', color1)
    text(cycles(1) + 7, 6, '14', 'Color', color1)
    text(cycles(1), 4, num2str(nicfg.scoptophotometry.pulsewidth), 'Color', color1)
    text(cycles(2) + 5, 2, sprintf('%2.1f Hz', 50/nicfg.scoptophotometry.freqmod), 'Color', color1)
    text(cycles(2) + 5, 1, sprintf('%2.1f s / %2.0f s', nicfg.scoptophotometry.freqmod/...
        50*nicfg.scoptophotometry.trainlength, nicfg.scoptophotometry.cycle), 'Color', color1)
    text(cycles(2), 9, 'Cycle = 20', 'Color', color3)
end

xticklabels([])
yticklabels([])
ylim([0 10])
end