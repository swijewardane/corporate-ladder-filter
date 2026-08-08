clear;clc;

% Base Parameters 
fs = 44100;
f1 = 20;
f2 = 20000;
T = 10;
N = 4;
len_IR = 2^15;

% Construct swept sine
L = T/(log(f2/f1));
t = (0:1/fs:T-1/fs);
x = sin(2*pi*f1*L*(exp(t/L)))';
Npts = length(x);
x_stereo = [x, x];


% Open plugin, set parameters, process audio

plugin = loadAudioPlugin("MoogLadderPlugin.vst3");
plugin.Resonance = 1.0;
plugin.Gain = 6;
plugin.Cutoff = 7000;
plugin.setMaxSamplesPerFrame(Npts);
% dispParameter(plugin)

y = process(plugin, x_stereo);
y_mono = y(:, 1);

% Synchronized swept-sine spectra

[X,f] = synchronized_swept_sine_spectra(f1,L,fs,Npts);

% Volterra Model call

[g, hm, h] = volterraModel(y_mono, X, L, N, fs, len_IR);


%% Plot 1

figure;
subplot(2, 1, 1);
plot(t, x);
title('ESS Input'); xlabel('Time'); ylabel('Amplitude'); xlim([0 T]); ylim([-2 2]);

subplot(2, 1, 2);
plot(t, y_mono);
title('Plugin Output'); xlabel('Time'); ylabel('Amplitude'); xlim([0 T]);

%% Plot 2

figure;
plot((-length(h):length(h)-1)./fs,[h; h], 'linewidth',1);
xlim([-4 1]);
xlabel('Time [s]');
title('Deconvolved Impulse Response');
grid on; box on;

%% Plot 3

% Higher Harmonic Frequency Responses (HHFRs)
Hm = fft(hm);
axe_w = linspace(0,2*pi,len_IR+1).'; axe_w(end) = [];

% -- plot the HHFRs
figure_line_colors = [0.0, 0.0, 1.0; 0.0, 0.5, 0.0; 1.0, 0.0, 0.0; 0.5, 0.5, 0.5];
set(groot,'defaultAxesColorOrder',figure_line_colors);

figure;
semilogx(axe_w*fs/2/pi/1000,20*log10(abs(Hm)),'linewidth',1);
xlim([f1 N*f2]./1000); ylim([-60 12]);
set(gca,'FontSize',18,'FontName','Helvetica');
title('HHFRs','FontSize',16,'FontName','Courier New');
xlabel('Frequency [kHz]','FontSize',16,'FontName','Helvetica');
ylabel('Amplitude [dB]','FontSize',16,'FontName','Helvetica');
legend({'H_1(f)','H_2(f)','H_3(f)', 'H_4(f)'});
set(gca,'XTick',[1 3 5 7 10 20 40 60]);
grid on; box on;

% -- end of plot the HHFRs

%% 2nd-Order Volterra Model Simulation

sim_order_1 = conv(x, g(:, 1), 'same');
sim_order_2 = conv(x.^2, g(:, 2), 'same');

sim = sim_order_1 + sim_order_2;

figure;
subplot(3, 1, 1);
plot(t, y_mono,'b');
title('Plugin Output'); xlabel('Time'); ylabel('Amplitude');

subplot(3, 1, 2);
plot(t, sim, 'r');
title('2nd-Order Volterra Simulation Output'); xlabel('Time'); ylabel('Amplitude');

subplot(3, 1, 3);
plot(t, y_mono - sim, 'g');
title('Difference Between Plugin and Simulation'); xlabel('Time'); ylabel('Amplitude');

% Spectral Comparison

% figure;
% Y_actual = 20*log10(abs(fft(y_mono)) + 1e-12);
% Y_approx = 20*log10(abs(fft(sim)) + 1e-12);
% f_ax = linspace(0, fs, length(y_mono));
% semilogx(f_ax, Y_actual, 'b', f_ax, Y_approx, 'r--', 'linewidth', 1.5);
% xlim([20 20000]);
% xlabel('Frequency (Hz)'); ylabel('Magnitude (dB)');
% legend({'Actual Output', 'Volterra Approximation'});
% title('Spectral Comparison: Actual vs 2nd Order Volterra');
% grid on;



