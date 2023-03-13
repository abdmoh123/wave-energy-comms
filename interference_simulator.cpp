//
// Created by abdha on 12/03/2023.
//

#include <random>

using namespace std;

class Noisifier {
private:
    double mean = 0.0;
    double std = 1.0;
public:
    double gen_gaussian(const double noise_amp) {
        /* Generates random values based on a gaussian distribution and an inputted amplitude */

        default_random_engine generator;
        normal_distribution<double> dist(mean, std);
        return noise_amp * dist(generator);
    }

    double add_gaussian(const double input_data, const double noise_amp) {
        /* Applies gaussian noise to inputted data */

        return input_data + gen_gaussian(noise_amp);
    }

    double add_periodic_gaussian(
        const double input_data,
        const double time,
        const double noise_amp,
        const double periodic_freq
    ) {
        /* Applies gaussian noise that varies with time periodically to inputted data */

        double sine_noise = sin(periodic_freq * time) * gen_gaussian(noise_amp);
        return input_data + sine_noise;
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
