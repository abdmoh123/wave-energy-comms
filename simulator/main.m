clc;
clear variables;
clear global;

%% Signal properties
spreading_factor = 7; % Spreading Factor (range = 7-12)
bandwidth = 125e3; % Bandwidth [Hz]
carrier_frequency = 868e6; % LoRa frequency band (868MHz = license exempt)
transmit_power = 14; % Transmission power [dBW]
noise_power = 31; % Noise power [dBW] (5.2 approximately = -5 SNR) - limit for 7SP = 31, limit for 12SP = 45
distance = 3e3; % Distance [m] between the 2 transceivers

% Message/payload being transmitted
message = "Hello World!";
disp(['Message sent = ' char(message)])

%% Sampling
sample_frequency = 15e6; % Sampling frequency (must satsify nyquist: >2x modified_carrier_frequency)
% offset is used to circumvent running out of memory (too many samples)
carrier_frequency_offset = 874.5e6;
% this value will be shown in the displayed graphs
modified_carrier_frequency = carrier_frequency_offset - carrier_frequency; % 6.5MHz

%% Transmit Signal
modulated_signal = LoRa_Tx( ...
    message, ...
    bandwidth, ...
    spreading_factor, ...
    transmit_power, ...
    sample_frequency, ...
    modified_carrier_frequency ...
);

% calculates and prints the transmitted power (should equal the predefined value)
Pt = 10 * log10(rms(modulated_signal) .^ 2);
disp(['Transmit Power (Pt) = ' num2str(Pt) ' dBm'])

%% Noise and attenuation
interf_sim = InterferenceSimulator(distance); % 3km distance

% modulated signal with attenuation as well as added noise
atten_noisy_signal = interf_sim.attenuate_with_noise( ...
    modulated_signal, carrier_frequency, noise_power, 0, 0 ...
);

% calcuates and prints the Free Space Path Loss
fspl = interf_sim.calc_fspl(carrier_frequency, 0, 0);
disp(['Free Space Path Loss = ' num2str(fspl)])

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
    modified_carrier_frequency ...
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
