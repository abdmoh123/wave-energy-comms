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
            output_arg = fspl(obj.distance, physconst('LightSpeed')/frequency) - gain_tx - gain_rx;
        end

        function output_arg = attenuate(obj, input_signal, frequency)
            %% Attenuates the signal using the distance between the transceivers

            transmit_power = 10 * log10(rms(input_signal) .^ 2);
            fspl = calc_fspl(obj, frequency, 0, 0);
            receive_power = transmit_power / fspl;

            output_arg = input_signal * (receive_power / transmit_power);
        end

        function output_arg = attenuate_with_noise(obj, input_signal, frequency, signal_noise_ratio)
            %% Attenuates and applies noise to the LoRa signal (noise does not get attenuated)

            % generates noise using only a signal to noise ratio
            noisy_signal = add_white_noise(obj, input_signal, signal_noise_ratio);
            % separates noise from the input signal
            noise = noisy_signal - input_signal;

            output_arg = attenuate(obj, input_signal, frequency) + noise;
        end

        function output_arg = gen_white_noise(~, noise_power, load_impedance, num_rows, num_cols)
            %% Generates white gaussian noise (wrapper function)
            
            % noise is generated using a seed of 1 (for reproducible results)
            output_arg = wgn(num_rows, num_cols, power=noise_power, imp=load_impedance, seed=1);
        end

        function output_arg = add_white_noise(~, input_signal, signal_noise_ratio)
            %% Applies white gaussian noise

            output_arg = awgn(input_signal, signal_noise_ratio, 'measured');
        end
    end
end
