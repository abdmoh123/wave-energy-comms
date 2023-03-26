//
// Created by abdha on 12/03/2023.
//

#include <random>
#include <algorithm>

using namespace std;

class Noisifier {
    /* Class for applying noise to data induced by wireless transmission */

private:
    double distance_m;  // distance between transmitter and receiver in metres
public:
    Noisifier(double distance_in) {
        /* Constructor for instantiating a Noisifer object */

        this->distance_m = distance_in;
    }

    double calculate_distance_coefficient() {
        /* Calculates how much noise is generated based on distance between the 2 transceivers */

        return get_distance();  // temporarily just returns the distance
    }

    vector<double> gen_gaussian_noise(
        const double noise_amp, const int vector_size, const double mean, const double std
    ) {
        /* Generates random values based on a gaussian distribution and an inputted amplitude */

        vector<double> random_vector;

        // sets the mean and std of the gaussian distribution
        default_random_engine generator;
        normal_distribution<double> dist(mean, std);

        // generates a vector with random based on the distribution above
        for (int _ = 0; _ < vector_size; ++_) {
            // distance between the 2 transceivers affects the amplitude of the noise
            random_vector.push_back(noise_amp * calculate_distance_coefficient() * dist(generator));
        }

        return random_vector;
    }

    vector<double> add_gaussian_noise(
        const vector<double> &input_data,
        const double noise_amp,
        const double mean,
        const double std
    ) {
        /* Applies gaussian noise to inputted data */

        vector<double> noisy_data;

        // generates a random gaussian noise vector
        vector<double> random_noise = gen_gaussian_noise(noise_amp, (int) input_data.size(), mean, std);
        // applies the random noise to the inputted data
        transform(input_data.begin(), input_data.end(), random_noise.begin(), noisy_data.begin(),plus<>());

        return noisy_data;
    }

    vector<double> add_periodic_gaussian_noise(
        const vector<double> &input_data,
        const double noise_amp,
        const double mean,
        const double std,
        const double periodic_freq
    ) {
        /* Applies gaussian noise that varies with time periodically to inputted data */

        vector<double> noisy_data;
        vector<double> sine_noise;

        // generates a random gaussian noise vector
        vector<double> random_noise = gen_gaussian_noise(noise_amp, (int) input_data.size(), mean, std);
        // generates a sine vector
        for (int i = 0; i < input_data.size(); ++i) {
            sine_noise.push_back(sin(periodic_freq * i));
        }

        // modulates noise to be sine-like (noise periodically increases and decreases)
        transform(sine_noise.begin(), sine_noise.end(), random_noise.begin(), sine_noise.begin(), multiplies<>());
        // applies the periodic noise to the inputted data
        transform(input_data.begin(), input_data.end(), sine_noise.begin(), noisy_data.begin(),plus<>());

        return noisy_data;
    }

    /* Attribute setters and getters */
    void set_distance(const double distance_in) {
        this->distance_m = distance_in;
    }
    double get_distance() {
        return distance_m;
    }
};
