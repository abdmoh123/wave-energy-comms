
# LoRa transmission and reception simulator

> By Abdul Hawisa

Simulates how data can be transmitted and received wirelessly through LoRa.
This includes adding the effects of interference through transmission.

Here is a simple diagram illustrating how the system would work:

![interference simulator process plan.png](interference%20simulator%20process%20plan.png)

LoRa uses CSS (Chip Spread Spectrum) to modulate data, which involves generating upchirps and symbols to represent the data.
This allows signals to be transmitted at a long range with minimal loss in quality, however, the drawback is the lower transmission data rate.

## To run the simulation:

- Install MATLAB + required addons
  - DSP System Toolbox
  - Communications Toolbox
  - Signal Processing Toolbox
- Tweak signal properties and message in main.m as you like
- Run main.m and record the produced results
