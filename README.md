
# LoRa transmission and reception simulator

> By Abdul Hawisa and Lewis Marshall

Simulates how data can be transmitted and received wirelessly through LoRa.
This includes adding the effects of interference through transmission.

Here is a simple diagram illustrating how the system would work:

![interference simulator process plan.png](interference%20simulator%20process%20plan.png)

LoRa uses CSS (Chip Spread Spectrum) to modulate data, which involves generating upchirps and symbols to represent the data.
This allows signals to be transmitted at a long range with minimal loss in quality, however, the drawback is the lower transmission data rate.

This project uses code from [this repository](https://github.com/bhomssi/LoRaMatlab) to emulate LoRa transmission and reception. Noise and attenuation code was designed and written by us.

## To run the simulation:

- Install MATLAB + required addons
  - DSP System Toolbox
  - Communications Toolbox
  - Signal Processing Toolbox
- Tweak signal properties and message in main.m as you like
- Run main.m and record the produced results

## References

### LoRa emulator:

- B. Al Homssi, K. Dakic, S. Maselli, H. Wolf, S. Kandeepan, and A. Al-Hourani, "IoT Network Design using Open-Source LoRa Coverage Emulator," in IEEE Access. 2021.

- Link on IEEE: https://ieeexplore.ieee.org/document/9395074

- Link on researchgate: https://www.researchgate.net/publication/350581481_IoT_Network_Design_using_Open-Source_LoRa_Coverage_Emulator

# Arduino LoRa transmitter and receiver

Sketch files holding code that transmits and receives LoRa packets with data read from a sensor and an external voltage signal. The code also includes a sleeping feature to reduce power consumption.

The devices used include 2 Arduino MKR WAN 1310s, an MPU6050 sensor, and an MKR ETH Shield for logging data (other shields that connect an SD card can also be used).
