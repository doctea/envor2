# envor2

Morphing, multi-ADSR Envelope generator sketch for Arduino Nano etc.

## Acknowledgements

with many thanks to Discord "synthesizers, yo" user Borogove for the help debugging the circuit and for the basis of the envelope generator (and hence thanks to user meem for the source samples that the envelope timings are based on)

## Features

- 4 different envelopes output from 1 gate input using 2xMCP4922
- knob per envelope, blending each envelope between 8 preset envelope shapes
- envelope 1 triggers when envelope 0 reaches Release stage
- envelope 1 and 3 biased + inverted to use as inverted envelopes
- slewing of envelope 1 back to level 0 when envelope 0 finishes

## Future/todo

- fix so that envelope speeds are predictable
- different envelope shapes
- CV morph between shapes
- LED indicators
