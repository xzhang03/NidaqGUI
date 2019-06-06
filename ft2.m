function [P1,freq] = ft2(data,samplingrate)
%ft2 applies fast fourier transformation to data and makes a figure
%   [power,freq] = ft2(data,samplingrate)

if nargin < 2
    samplingrate = 1;
end

y = fft(data);

n = length(data);

P2 = abs(y/n);
P1 = P2(1:n/2+1);
P1(2:end-1) = 2*P1(2:end-1);

freq = samplingrate*(0:(n/2))/n;

% y0 = fftshift(y);         % shift y values
% freq = (-n/2:n/2-1)*(samplingrate/n); % 0-centered frequency range
% power = abs(y0).^2/n;    % 0-centered power

figure
plot(freq,P1)
xlabel('Frequency')
ylabel('Power')

end

