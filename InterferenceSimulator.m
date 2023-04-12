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

        % function output_arg = calc_receiving_power_density( ...
        %     obj, ...
        %     transmit_power, ...
        %     antenna_gain, ...
        %     target_radar_cross_section ...
        % 
        % )
        %     %% Calculates the Power Density at the receiving end
        % 
        %     output_arg = transmit_power / (4 * pi * obj.distance);
        % end

        function output_arg = attenuate(obj, input_signal)
            %% Attenuates the signal using the distance between the transceivers

            % signal strength is proportional to the inverse square of distance
            coefficient = 1 / (obj.distance^2);
            output_arg = coefficient * input_signal;
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
