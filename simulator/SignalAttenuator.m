classdef SignalAttenuator
    % Interference simulator class for applying noise to LoRa signal

    properties
        distance;
    end

    methods
        function obj = SignalAttenuator(distance_in)
            %% Constructs an instance of this class

            obj.distance = distance_in;
        end

        function output_arg = calc_fspl(obj, frequency, gain_tx, gain_rx)
            %% Calculates the Free Space Path Loss for a 2 transceiver system
            
            % 20 * log10((4pi*df) / c) - g_tx - g_rx;
            out = fspl( ...
                obj.distance, physconst('LightSpeed') / frequency ...
            ) - gain_tx - gain_rx;

            % takes into account the gains of the transceiver antennas
            output_arg = out;
        end

        function output_arg = attenuate(obj, input_signal, frequency, gain_tx, gain_rx)
            %% Attenuates the signal using the distance between the transceivers

            fspl = calc_fspl(obj, frequency, gain_tx, gain_rx);
            % calculates the transmit and receive power values using the fspl
            transmit_power = rms(input_signal) .^ 2;
            % RSSI = received power, which should always be < 0
            rssi = transmit_power - fspl; % in dBm form, FSPL = P_t - P_r

            % linear scale = 10 ^ (power_diff / 20)
            output_arg = db2mag(rssi) * input_signal / transmit_power;
        end

        function output_arg = attenuate_with_noise( ...
            obj, input_signal, frequency, noise_power, gain_tx, gain_rx ...
        )
            %% Attenuates then applies noise to the LoRa signal (noise does not get attenuated)

            % generates noise
            noise = wgn( ...
                size(input_signal, 1), size(input_signal, 2), noise_power ...
            );
            % measures noise power
            noise_power_measured = 10 * log10(rms(noise)^2); % always < 0
            disp(['Noise Power (Pn) = ' num2str(noise_power_measured) ' dBm'])

            % attenuates the signal
            atten_signal = attenuate(obj, input_signal, frequency, gain_tx, gain_rx);
            % signal to noise ratio is calculated using the receive power
            rssi = 10 * log10(rms(atten_signal)^2);
            disp(['RSSI = ' num2str(rssi)])
            snr = rssi - noise_power;
            disp(['SNR = ' num2str(snr)])

            output_arg = atten_signal + noise;
        end
    end
end
