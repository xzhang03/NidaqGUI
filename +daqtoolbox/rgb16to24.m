function out = rgb16to24(imdata,format)
% out = rgb16to24(camdata_struct);
% out = rgb16to24(frame,format);

if isstruct(imdata), in = imdata; else in.Format = format; in.Frame = imdata; end
if isempty(in.Frame), out = []; return, end
if isa(in.Frame,'uint8'), out = in.Frame; return, end

sz = size(in.Frame);
switch ndims(in.Frame)
    case 2, sz(3) = 1;
    case 3  % do nothing
    otherwise, error('The frame data must be 2 or 3 dimensional!!!');
end
out = zeros([sz(1:2) 3 sz(3)],'uint8');

switch in.Format
    case 'RGB565'
        for m=1:sz(3)
            out(:,:,1,m) = bitshift(bitand(in.Frame(:,:,m),63488),-8);
            out(:,:,2,m) = bitshift(bitand(in.Frame(:,:,m),2016),-3);
            out(:,:,3,m) = bitshift(bitand(in.Frame(:,:,m),31),3);
        end
    case 'YUY2'
        for m=1:sz(3)
            C = uint32(bitand(in.Frame(:,:,m),255) - 16)*298;
            D = uint32(bitand(in.Frame(:,:,m),65280)/256 - 128);
            E = D;
            D(:,2:2:end,:) = D(:,1:2:end,:);
            E(:,1:2:end,:) = E(:,2:2:end,:);
            out(:,:,1,m) = (C + 409*E + 128)/256;
            out(:,:,2,m) = (C - 100*D - 208*E + 128)/256;
            out(:,:,3,m) = (C + 516*D + 128)/256;
        end
    case 'RGB555'
        for m=1:sz(3)
            out(:,:,1,m) = bitshift(bitand(in.Frame(:,:,m),31744),-7);
            out(:,:,2,m) = bitshift(bitand(in.Frame(:,:,m),992),-2);
            out(:,:,3,m) = bitshift(bitand(in.Frame(:,:,m),31),3);
        end
end
