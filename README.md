# libNCPA
Useful utilities and libraries for acoustics work

# Important Note:
This was compiled and installed on an x86 Linux installation. Specifically, on Void Linux:
https://voidlinux.org/

# Compiling
```
make
```
# Installing
```
sudo make install
```

# Notes on src/*
This is the source code for the compiled binaries in bin

These are examples of how to utilize the headers in include

# Notes on bin/*
## noiseMaker
This generates Gaussian white noise and sends that data to stdout
## sineMaker
This generates data for a sine wave of a given frequency and sends that data to stdout
## noisePropagator
This takes time-series data from stdin and outputs what an acoustic array would measure to stdout
## r2cFFT and c2rFFT
These utilities take in time-series data from stdin and performs a fast Fourier transformation, outputing to stdout
## rollingDFT
This takes in time-series data from stdin and outputs the Fourier coefficient for the given bins to stdout
## simplePipeIn and simplePipeOut
These are operate on FIFO files to pipe data into or read data out from them.

# Notes on include/*
These are the headers that contain the useful classes for performing operations.
