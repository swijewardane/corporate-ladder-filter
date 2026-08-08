function [g, hm, h] = volterraModel(y, X, L, N, fs, len_IR)

Y = fft(y)/fs;   % FFT of the output signal

% Deconvolution
H = Y./X; H(1) = 0;  % avoid Inf at DC


h = ifft(H,'symmetric');     % impulse response


%% -- separation of higher harmonic impulse responses

pre_IR = len_IR / 2;

dt = L.*log(1:N).*fs;

hm = synchronized_swept_sine_IR_separation(h,dt,len_IR,pre_IR);

% -- end of the separation of higher harmonic impulse responses
%% Higher Harmonic Frequency Responses (HHFRs)
Hm = fft(hm);


%% Estimation of filters Gm of Generalized Hammerstein model (Diagonal Volterra Kernels)
% Coefficients A of the transform between the Hm and Gm
A = powersin(N);
% tranform the HHFR Hm to filters Gm
Gm = Hm/A;


%% signal reconstruction

g =real(ifft(Gm, 'symmetric'));