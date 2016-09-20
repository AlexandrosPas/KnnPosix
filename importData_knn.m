function [knndist knnidx data queries] = importData_knn(N, D, Q, k)


fid = fopen('training_set.bin');

data = fread(fid, [D N], 'double');

fclose(fid);


fid = fopen('query_set.bin');

queries = fread(fid, [D Q], 'double');

fclose(fid);


fid = fopen('KNNdist.bin');

knndist = fread(fid, [k Q], 'double');
[knndist iid] = sort(knndist);


fclose(fid);


fid = fopen('KNNidx.bin');

knnidx = fread(fid, [k Q], 'int32');
for i=1:Q
    knnidx(:,i) = knnidx(iid(:,i), i) + 1;
end

fclose(fid);



end