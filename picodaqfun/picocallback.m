function picocallback(src, ~, fid)

% Read and write
if src.BytesAvailable > src.UserData.bytecheck
    % tic
    data = read(src, src.BytesAvailableFcnCount, 'int32');
    % toc;
    fwrite(fid, data, 'int32');
end

end