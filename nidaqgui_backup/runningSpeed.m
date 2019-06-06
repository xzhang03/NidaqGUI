function speed = runningSpeed(position, framerate, wheel_diameter_cm, wheel_tabs)
%RUNNINGSPEED Convert position of rotary encoder to running speed

    if nargin < 3, wheel_diameter_cm = 14; end
    if nargin < 4, wheel_tabs = 44; end
    
    wheel_circumference = wheel_diameter_cm*pi;
    step_size = wheel_circumference/(wheel_tabs*2);

    instantaneous_speed = zeros(length(position), 1);
    if ~isempty(instantaneous_speed)
        instantaneous_speed(2:end) = diff(position);
        instantaneous_speed(2) = 0;
        instantaneous_speed = instantaneous_speed*step_size*framerate;
    end
    
    speed = conv(instantaneous_speed, ones(ceil(framerate), 1)/ceil(framerate), 'same')';
end

