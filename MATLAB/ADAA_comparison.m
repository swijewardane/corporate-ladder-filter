clear; clc;

% Test sine
fs = 44100;
f_test = 7000;
t_test = 0:1/fs:10;
x = sin(2*pi*f_test*t_test)';
x_stereo = [x, x];

% Load plugin with and without ADAA

plugin = loadAudioPlugin('MoogLadderPlugin.vst3');
plugin.Gain = 6;
plugin.Resonance = 1;
plugin.Cutoff = 20000;
plugin.setMaxSamplesPerFrame(length(x));

plugin_no_adaa = loadAudioPlugin('MoogLadderPlugin.vst3');
plugin_no_adaa.Gain = 6;
plugin_no_adaa.Resonance = 1;
plugin_no_adaa.Cutoff = 20000;
plugin_no_adaa.ADAABypass = "On";
plugin_no_adaa.setMaxSamplesPerFrame(length(x));
% Process and Plot
y = process(plugin, x_stereo);
y = y(:, 1);
y_no_adaa = process(plugin_no_adaa, x_stereo);
y_no_adaa = y_no_adaa(:, 1);

Nfft = length(x_stereo);
f_ax = linspace(0, fs/2, Nfft/2 + 1);

Y = db(abs(fft(y, Nfft)/(sqrt(Nfft))));
Y_no_adaa = db(abs(fft(y_no_adaa, Nfft)/(sqrt(Nfft))));

figure;
semilogx(f_ax, Y(1:length(f_ax)), 'b'); hold on;
semilogx(f_ax, Y_no_adaa(1:length(f_ax)), 'r');
semilogx(f_ax, -Y(1:length(f_ax)) + Y_no_adaa(1:length(f_ax)), '-g');
xlim([1000 20000]); ylim([-120 60]);
xlabel('Frequency (Hz)'); ylabel('Magnitude (db)'); legend({'With ADAA', 'Without ADAA', 'Difference'})
title('Plugin Output with and without ADAA for 7 kHz Sinusoid Input')