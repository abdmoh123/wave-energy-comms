classdef InterferenceSimulator
    % Interference simulator class for applying noise to LoRa signal

    properties
        distance;
    end

    methods
        function obj = InterferenceSimulator(distance_in)
            %% Constructs an instance of this class

            obj.distance = distance_in;
        end

        function output_arg = calc_fspl(obj, frequency, gain_tx, gain_rx)
            %% Calculates the Free Space Path Loss for a 2 transceiver system

            % takes into account the gains of the transceiver antennas
            output_arg = fspl( ...
                obj.distance, physconst('LightSpeed') / frequency ...
            ) - gain_tx - gain_rx;
        end

        function output_arg = attenuate(obj, input_signal, frequency, gain_tx, gain_rx)
            %% Attenuates the signal using the distance between the transceivers

            fspl = calc_fspl(obj, frequency, gain_tx, gain_rx);
            % calculates the transmit and receive power values using the fspl
            transmit_power = 10 * log10(rms(input_signal) .^ 2);
            receive_power = transmit_power / fspl;

            disp(['Receive power (clean) = ' num2str(receive_power) ' dBm'])

            power_diff = receive_power - transmit_power; % should always be < 0
            output_arg = input_signal * db2mag(power_diff); % converts to linear scale
        end

        function output_arg = attenuate_with_noise( ...
            obj, input_signal, frequency, noise_power, gain_tx, gain_rx ...
        )
            %% Attenuates then applies noise to the LoRa signal (noise does not get attenuated)

            % generates noise
            noise = wgn( ...
                size(input_signal, 1), size(input_signal, 2), noise_power, 1, 0, 'dBW', 'complex' ...
            );
            % measures noise power
            noise_power_measured = 10 * log10(rms(noise) .^ 2);
            disp(['Noise Power (Pn) = ' num2str(noise_power_measured) ' dBm'])

            % attenuates the signal
            atten_signal = attenuate(obj, input_signal, frequency, gain_tx, gain_rx);
            % signal to noise ratio is calculated using the receive power
            receive_power = 10 * log10(rms(atten_signal) .^ 2);
            snr = 10 * log10(db2mag(receive_power)^2/db2mag(noise_power)^2);
            disp(['SNR = ' num2str(snr)])

            output_arg = atten_signal + noise;
        end
    end
end
