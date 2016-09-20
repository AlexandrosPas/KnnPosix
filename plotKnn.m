clear all
close all


N = 2000;
D = 2;
Q = 1;
k = 87;


%%%%%%%%%%%%%%%%%%%%%%%%%%

if(D~=2)
   error('Can plot only 2D data');
end


system(sprintf('make clean; make'));

system(sprintf('./knnTest %d %d %d %d', N, D, Q, k));



[kdist kidx data queries] = importData_knn(N, D, Q, k);


figure
plot(data(1,:), data(2,:), 'o');
hold on
plot(queries(1,1), queries(2, 1), 'or')
hold on
plot(data(1, kidx(:,1)), data(2, kidx(:,1)), 'og');
hold off

