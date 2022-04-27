function position = omniencoderparse(arduinodata)

% offset
offset = 128;

% Initialize
l = length(arduinodata);
position = zeros(l,1);
ind = 0;

for i = 1 : l
    ind = ind + 1;
    if (arduinodata(i) < 256) && (arduinodata(i) > 1)
        position(ind) = arduinodata(i) - offset;
    elseif arduinodata(i) >= 256
        v = double(typecast(int32(arduinodata(i)),'uint8'));
        nb = v(4) + 1;
        position(ind : ind+nb-1) = v(1:nb) - offset;
        ind = ind + nb - 1;
    end
end

position = cumsum(position);

end