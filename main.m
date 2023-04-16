clc;
clear variables;
clear global;

%% Signal properties
spreading_factor = 10; % Spreading Factor
bandwidth = 125e3; % Bandwidth
carrier_frequency = 868e6; % LoRa frequency band (868MHz = license exempt)
transmit_power = 14; % Transmission power (in dBm)
signal_noise_ratio = -5; % Signal Noise Ratio

% Message/payload being transmitted
message = "Hello World!";
disp(['Message sent = ' char(message)])

%% Sampling
sample_frequency = 15e6; % Sampling frequency (must satsify nyquist: >2x frequency_offset)
spectrum_centre_frequency = 874.5e6;
% offset is used to circumvent running out of memory (too many samples)
frequency_offset = spectrum_centre_frequency - carrier_frequency;

%% Transmit Signal
modulated_signal = LoRa_Tx( ...
    message, ...
    bandwidth, ...
    spreading_factor, ...
    transmit_power, ...
    sample_frequency, ...
    frequency_offset ...
);

% calculates and prints the transmitted power (should equal the predefined value)
Pt = 10 * log10(rms(modulated_signal) .^ 2);
disp(['Transmit Power (Pt) = ' num2str(Pt) ' dBm'])

%% Noise and attenuation
interf_sim = InterferenceSimulator(3e3); % 3km distance

% modulated signal with attenuation as well as added noise
atten_noisy_signal = interf_sim.attenuate_with_noise( ...
    modulated_signal, carrier_frequency, 0, 0, signal_noise_ratio ...
);

% calcuates and prints the Free Space Path Loss
fspl = interf_sim.calc_fspl(carrier_frequency, 0, 0);
disp(['Free Space Path Loss = ' num2str(fspl) ' dB'])

% calculates and prints the transmitted power
Pr = 10 * log10(rms(atten_noisy_signal) .^ 2);
disp(['Receive Power (Pr) = ' num2str(Pr) ' dBm'])

%% Received Signal
message_out = LoRa_Rx( ...
    atten_noisy_signal, ...
    bandwidth, ...
    spreading_factor, ...
    2, ... % non-coherent FSK detection enabled
    sample_frequency, ...
    frequency_offset ...
);

%% Message Out
disp(['Message Received = ' char(message_out)])

%% LoRa signal plots
figure(1)
clean_tiles = tiledlayout(2, 1);

% plots frequency spectrum of the modulated signal without noise
nexttile
spectrogram(modulated_signal, 500, 0, 500, sample_frequency, 'yaxis', 'centered')
title("Frequency spectrum clean modulated signal");

% plots the 99% occupied bandwidth of the modulated signal without noise
nexttile
obw(modulated_signal, sample_frequency);

exportgraphics(clean_tiles, 'clean_signal.png', 'Resolution', 300)

%% Attenuated noisy signal plots
figure(2)
noisy_tiles = tiledlayout(2, 1);

% plots frequency spectrum of the attenuated modulated signal (with added noise)
nexttile
spectrogram(atten_noisy_signal, 500, 0, 500, sample_frequency, 'yaxis', 'centered')
title("Frequency spectrum of attenuated noisy modulated signal")

% plots the 99% occupied bandwidth of the attenuated noisy modulated signal
nexttile
obw(atten_noisy_signal, sample_frequency);

exportgraphics(noisy_tiles, 'noisy_signal.png', 'Resolution', 300)
