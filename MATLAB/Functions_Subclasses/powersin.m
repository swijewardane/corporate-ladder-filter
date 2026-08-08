function A = powersin(N)
% Coefficients A of the transform between the Higher Harmonic Frequency
% Responses and the filters of the Generalized Hammerstein model
%
% [Novak et al. (2010)] 
% A. Novak, L. Simon, F. Kadlec & P. Lotton (2010), "Nonlinear system 
%      identification using exponential swept-sine signal", Instrumentation
%      and Measurement, IEEE Transactions on. Vol. 59(8), pp. 2220-2229.
%
% Antonin Novak (antonin.novak@univ-lemans.fr), 11/02/2011
% Laboratoire d'Acoustique de l'Université du Mans (LAUM, UMR CNRS 6613), 
% 72085 Le Mans, France
%
% https://ant-novak.com
%
%%

A(N,N) = 0;
for n = 1:N
    for m = 1:N
        if ( (n>=m) && (mod(n+m,2)==0) )
            % Eq. (48) of [Novak et al. (2010)] 
            A(n,m) = (((-1)^(2*n+(1-m)/2))/(2^(n-1)))*nchoosek(n,(n-m)/2);
        end
    end
end

