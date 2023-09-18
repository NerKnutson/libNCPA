# libNCPA
Useful utilities and libraries for acoustics work

# Important Note:
This was compiled and installed on an x86 Linux installation. Specifically, on Void Linux:
https://voidlinux.org/
You should be able to play around with the examples in bin and the headers in include without performing a full install.

# Dependencies
This repository must be installed to use the decimator.

https://github.com/berndporr/iir1

If using Void Linux, simply
```
sudo xbps-install -Su libiir1-devel
```

Otherwise, follow the instructions to install from source.
Be sure to append the following to your bashrc or other startup script if installing from source:
```
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```
Or run the decimator class as
```
LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH decimator <...>
```

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
