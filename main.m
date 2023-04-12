clc;
clear variables;
clear global;

%% Signal properties
spreading_factor = 12; % Spreading Factor
bandwidth = 125e3; % Bandwidth
sample_frequency = 10e6; % Sampling frequency
carrier_frequency_offset = 915e6 - 921.5e6; % Frequency offset
transmit_power = 14; % Transmission power (in dBm)
signal_noise_ratio = -5; % Signal Noise Ratio

% Message/payload being transmitted
message = "emf:10.0;acc:3;.;";
disp(['Message sent = ' char(message)])

%% Transmit Signal
modulated_signal = LoRa_Tx( ...
    message, ...
    bandwidth, ...
    spreading_factor, ...
    transmit_power, ...
    sample_frequency, ...
    carrier_frequency_offset ...
);

% calculates and prints the transmitted power (should equal the predefined value)
Pt = 10 * log10(rms(modulated_signal) .^ 2);
disp(['Transmit Power (Pt) = ' num2str(Pt) ' dBm'])

%% Noise and attenuation
interf_sim = InterferenceSimulator(3e3); % 3km distance

% modulated signal with added noise
noisy_signal = interf_sim.add_white_noise(modulated_signal, signal_noise_ratio);
% modulated signal with attenuation as well as added noise
atten_noisy_signal = interf_sim.attenuate_with_noise( ...
    modulated_signal, sample_frequency, signal_noise_ratio ...
);

% calcuates and prints the Free Space Path Loss
fspl = interf_sim.calc_fspl(sample_frequency, 0, 0);
disp(['Free Space Path Loss = ' num2str(fspl) ' dB'])

% calculates and prints the transmitted power (should equal the predefined value)
Pr = 10 * log10(rms(atten_noisy_signal) .^ 2);
disp(['Receive Power (Pr) = ' num2str(Pr) ' dBm'])

%% Received Signal
message_out = LoRa_Rx( ...
    atten_noisy_signal, ...
    bandwidth, ...
    spreading_factor, ...
    2, ... % non-coherent FSK detection enabled
    sample_frequency, ...
    carrier_frequency_offset ...
);

%% Message Out
disp(['Message Received = ' char(message_out)])

%% LoRa signal plots
% plots frequency spectrum of the modulated signal without noise
figure(1)
spectrogram(modulated_signal, 500, 0, 500, sample_frequency, 'yaxis', 'centered')
title("Frequency spectrum clean modulated signal");
% plots the 99% occupied bandwidth of the modulated signal without noise
figure(2)
obw(modulated_signal, sample_frequency);

%% Noisy signal plots
% plots frequency spectrum of the noisy modulated signal
figure(3)
spectrogram(noisy_signal, 500, 0, 500, sample_frequency, 'yaxis', 'centered')
title("Frequency spectrum noisy modulated signal")
% plots the 99% occupied bandwidth of the noisy modulated signal
figure(4)
obw(noisy_signal, sample_frequency);

%% Attenuated noisy signal plots
% plots frequency spectrum of the attenuated modulated signal (with added noise)
figure(5)
spectrogram(atten_noisy_signal, 500, 0, 500, sample_frequency, 'yaxis', 'centered')
title("Frequency spectrum of attenuated noisy modulated signal")
% plots the 99% occupied bandwidth of the attenuated noisy modulated signal
figure(6)
obw(atten_noisy_signal, sample_frequency);
