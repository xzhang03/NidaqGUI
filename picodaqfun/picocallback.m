function picocallback(src, ~, fid)

% Read and write
data = read(src, src.BytesAvailableFcnCount, 'int32');
fwrite(fid, data, 'int32');

end