//
// Created by abdha on 12/03/2023.
//

#include <random>
#include <algorithm>

using namespace std;

class Noisifier {
    /* Class for applying noise to data induced by wireless transmission */

private:
    double mean = 0.0;
    double std = 1.0;
public:
    Noisifier(double mean, double std) {
        /* Constructor for instantiating a Noisifer object */

        set_mean(mean);
        set_std(std);
    }

    vector<double> gen_gaussian(const double noise_amp, const int vector_size) {
        /* Generates random values based on a gaussian distribution and an inputted amplitude */

        vector<double> random_vector;

        // sets the mean and std of the gaussian distribution
        default_random_engine generator;
        normal_distribution<double> dist(mean, std);

        // generates a vector with random based on the distribution above
        for (int i = 0; i < vector_size; ++i) {
            random_vector.push_back(noise_amp * dist(generator));
        }

        return random_vector;
    }

    vector<double> add_gaussian(const vector<double> &input_data, const double noise_amp) {
        /* Applies gaussian noise to inputted data */

        vector<double> noisy_data;

        // generates a random gaussian noise vector
        vector<double> random_noise = gen_gaussian(noise_amp, (int) input_data.size());
        // applies the random noise to the inputted data
        transform(input_data.begin(), input_data.end(), random_noise.begin(), noisy_data.begin(),plus<>());

        return noisy_data;
    }

    vector<double> add_periodic_gaussian(
        const vector<double> &input_data,
        const double noise_amp,
        const double periodic_freq
    ) {
        /* Applies gaussian noise that varies with time periodically to inputted data */

        vector<double> noisy_data;
        vector<double> sine_noise;

        // generates a random gaussian noise vector
        vector<double> random_noise = gen_gaussian(noise_amp, (int) input_data.size());
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
    void set_mean(const double mean_in) {
        this->mean = mean_in;
    }
    void set_std(const double std_in) {
        this->std = std_in;
    }
    double get_mean() {
        return mean;
    }
    double get_std() {
        return std;
    }
};
