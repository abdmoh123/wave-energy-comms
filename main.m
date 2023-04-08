clear all;

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
Sxx = 10 * log10(rms(modulated_signal).^2);
disp(['Transmit transmit_power = ' num2str(Sxx) ' dBm'])

%% LoRa signal plots
figure(1)
spectrogram(modulated_signal, 500, 0, 500, sample_frequency, 'yaxis', 'centered')
figure(2)
obw(modulated_signal, sample_frequency);

%% Noise and attenuation
interf_sim = InterferenceSimulator(3000); % 3km distance
disp(size(modulated_signal));
disp(signal_noise_ratio);
noisy_signal = interf_sim.add_white_noise(modulated_signal, signal_noise_ratio);

%% Noisy signal plots
figure(3)
spectrogram(noisy_signal, 500, 0, 500, sample_frequency, 'yaxis', 'centered')
figure(4)
obw(noisy_signal, sample_frequency);

%% Received Signal
message_out = LoRa_Rx( ...
    modulated_signal, ...
    bandwidth, ...
    spreading_factor, ...
    2, ... % non-coherent FSK detection enabled
    sample_frequency, ...
    carrier_frequency_offset ...
);

%% Message Out
disp(['Message Received = ' char(message_out)])
