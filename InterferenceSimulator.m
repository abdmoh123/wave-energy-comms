classdef InterferenceSimulator
    % Interference simulator class for applying noise to LoRa signal

    properties
        distance
    end

    methods
        function obj = InterferenceSimulator(distance_in)
            % Constructs an instance of this class
            obj.distance = distance_in;
        end

        function output_arg = gen_white_noise(noise_power, load_impedance, num_rows, num_cols)
            % Generates white gaussian noise (wrapper function)
            
            % noise is generated using a seed of 1 (for reproducible results)
            output_arg = wgn(num_rows, num_cols, power=noise_power, imp=load_impedance, seed=1);
        end

        function output_arg = add_white_noise(input_signal, noise_power, load_impedance)
            % Applies white gaussian noise

            [num_rows, num_cols] = size(input_signal);

            % generates a random gaussian noise vector
            noise = gen_white_noise(noise_power, load_impedance, num_rows, num_cols);
            % applies the random noise to the input signal
            output_arg = input_signal + noise;
        end

        function output_arg = add_periodic_white_noise( ...
            input_signal, noise_power, load_impedance, periodic_frequency ...
        )
            % Applies white gaussian noise that varies with time periodically

            [num_rows, num_cols] = size(input_signal);

            % generates a random gaussian noise vector
            noise = gen_white_noise(noise_power, load_impedance, num_rows, num_cols);

            % modulates noise to be sine-like (noise periodically increases and decreases)
            x = 1:num_rows;
            sine_noise = zeros(num_rows, num_cols); % preallocation (faster)
            for i=1:num_rows
                sine_noise(:, i) = sin(periodic_frequency * x) .* noise(:, i);
            end

            % applies the periodic noise to the input signal
            output_arg = input_signal + sine_noise;
        end

        function output_arg = add_linear_white_noise( ...
            input_signal, noise_power, load_impedance, noise_gradient ...
        )
            % Applies white gaussian noise that increases with time based on the noise gradient

            [num_rows, num_cols] = size(input_signal);

            % generates a random gaussian noise vector
            noise = gen_white_noise(noise_power, load_impedance, num_rows, num_cols);

            % makes noise increase linearly with time
            x = 1:num_rows;
            linear_noise = zeros(num_rows, num_cols); % preallocation (faster)
            for i=1:num_rows
                linear_noise(:, i) = noise_gradient * (x .* noise(:, i));
            end

            % applies the increasing noise to the input signal
            output_arg = input_signal + linear_noise;
        end
    end
end
