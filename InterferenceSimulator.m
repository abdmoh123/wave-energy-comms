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

            power_diff = receive_power - transmit_power; % should always be < 0
            output_arg = input_signal * db2mag(power_diff); % converts to linear scale
        end

        function output_arg = attenuate_with_noise( ...
            obj, input_signal, frequency, gain_tx, gain_rx, signal_noise_ratio ...
        )
            %% Attenuates then applies noise to the LoRa signal (noise does not get attenuated)

            % generates noise using only a signal to noise ratio based on
            % the original modulated signal (not attenuated)
            noisy_signal = awgn(input_signal, signal_noise_ratio, 'measured');
            noise = noisy_signal - input_signal; % separates noise from the input signal

            output_arg = attenuate(obj, input_signal, frequency, gain_tx, gain_rx) + noise;
        end
    end
end
