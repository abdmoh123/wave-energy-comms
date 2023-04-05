clear all;

spreading_factor = 12; % Spreading Factor
bandwidth = 1000; % Bandwidth
sample_frequency = 1000; % Sampling frequency
signal_noise_ratio = -5; % Signal Noise Ratio

% Message/payload being transmitted
message = "emf:10.0;acc:3;.;";

carrier_frequency_offset = 91500 - 92150;
% Transmission power (in dB)
transmit_power = 7;

%% Sampling
Fc = 92150;

%% Transmit Signal
modulated_signal = LoRa_Tx( ...
    message, ...
    bandwidth, ...
    spreading_factor, ...
    transmit_power, ...
    sample_frequency, ...
    carrier_frequency_offset ...
);

% calculates and prints the transmitted power in dBm
Sxx = 10 * log10(rms(modulated_signal).^2);
disp(['Transmit transmit_power = ' num2str(Sxx) ' dBm'])

%% LoRa signal plots
figure(1)
spectrogram(modulated_signal, 500, 0, 500, sample_frequency, 'yaxis', 'centered')
figure(2)
obw(modulated_signal, sample_frequency);

%% Noise and attenuation
interf_sim = InterferenceSimulator(3000); % 3km distance
noisy_signal = interf_sim.add_white_noise(signal_noise_ratio);

%% Received Signal
message_out = LoRa_Rx( ...
    noisy_signal, ...
    bandwidth, ...
    spreading_factor, ...
    2, ... % non-coherent FSK detection enabled
    sample_frequency, ...
    carrier_frequency_offset ...
);

%% Message Out
disp(['Message Received = ' char(message_out)])
