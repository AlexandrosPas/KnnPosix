clear all
close all


N = 1048576;
D = 3;
Q = 1024;
k = 128;
NUM_THREADS=6;

%%%%%%%%%%%%%%%%%%%%%%%%%%

if(D~=2 && D~=3)
    error('Can plot only 2D data');
    exit(-1);
end


system(sprintf('make clean; make'));
system(sprintf('./knnTest %d %d %d %d %d', N, D, Q, k));

[kdist kidx data queries] = importData_knn(N, D, Q, k);

if(D==2)
    figure
    plot(data(1,:), data(2,:), 'o');
    hold on
    plot(queries(1,:), queries(2, :), 'or')
    for i=(1:Q)
        plot(data(1, kidx(:,i)), data(2, kidx(:,i)), 'og');
    end
    hold off
end
if(D==3)
    figure
    plot3(data(1,:), data(2,:), data(3,:), 'o');
    hold on
    plot3(queries(1,:), queries(2, :), queries(3,:),'or')
    for i=(1:Q)
        plot3(data(1, kidx(:,i)), data(2, kidx(:,i)),data(3, kidx(:,i)), 'og');
    end
    hold off
end