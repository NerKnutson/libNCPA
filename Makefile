INSTALL_PREFIX?=/usr/local

CFLAGS=-DMAX_CHANNELS=128 -O3
LDLIBS=-liir -lfftw3
#INCLUDE=-I $(LIBUSB_PREFIX)/include/libusb-1.0
#UNAME := $(shell uname)
#ifeq ($(UNAME), Linux)
#LDLIBS += -ludev
#endif

# ADD MORE WARNINGS!
#WARNING := -Wall -Wextra

# .PHONY means these rules get executed even if
# files of those names exist.
.PHONY: all clean

all: executables

executables: sineMaker noiseMaker noisePropagator simplePipeIn simplePipeOut rollingDFT decimator r2cFFT c2rFFT

sineMaker: src/sineMaker.cpp
	$(CXX) $(CFLAGS) $(LDLIBS) $(WARNING) $^ -o bin/$@

noiseMaker: src/noiseMaker.cpp
	$(CXX) $(CFLAGS) $(LDLIBS) $(WARNING) $^ -o bin/$@

noisePropagator: src/noisePropagator.cpp
	$(CXX) $(CFLAGS) $(LDLIBS) $(WARNING) $^ -o bin/$@

simplePipeIn: src/simplePipeIn.cpp
	$(CXX) $(CFLAGS) $(LDLIBS) $(WARNING) $^ -o bin/$@

simplePipeOut: src/simplePipeOut.cpp
	$(CXX) $(CFLAGS) $(LDLIBS) $(WARNING) $^ -o bin/$@

rollingDFT: src/rollingDFT.cpp
	$(CXX) $(CFLAGS) $(LDLIBS) $(WARNING) $^ -o bin/$@
	
decimator: src/decimator.cpp
	$(CXX) $(CFLAGS) $(LDLIBS) $(WARNING) $^ -o bin/$@
	
r2cFFT: src/r2cFFT.cpp
	$(CXX) $(CFLAGS) $(LDLIBS) $(WARNING) $^ -o bin/$@
	
c2rFFT: src/c2rFFT.cpp
	$(CXX) $(CFLAGS) $(LDLIBS) $(WARNING) $^ -o bin/$@

install:
	cp include/importArrayGeometry.h $(INSTALL_PREFIX)/include/
	cp include/rollingDFT.h $(INSTALL_PREFIX)/include/
	cp include/simplePipe.h $(INSTALL_PREFIX)/include/
	cp bin/sineMaker $(INSTALL_PREFIX)/bin/
	cp bin/noiseMaker $(INSTALL_PREFIX)/bin/
	cp bin/noisePropagator $(INSTALL_PREFIX)/bin/
	cp bin/simplePipeIn $(INSTALL_PREFIX)/bin/
	cp bin/simplePipeOut $(INSTALL_PREFIX)/bin/
	cp bin/rollingDFT $(INSTALL_PREFIX)/bin/

uninstall:
	rm -f $(INSTALL_PREFIX)/include/importArrayGeometry.h
	rm -f $(INSTALL_PREFIX)/include/rollingDFT.h
	rm -f $(INSTALL_PREFIX)/include/simplePipe.h
	rm -f $(INSTALL_PREFIX)/bin/sineMaker
	rm -f $(INSTALL_PREFIX)/bin/noiseMaker
	rm -f $(INSTALL_PREFIX)/bin/noisePropagator
	rm -f $(INSTALL_PREFIX)/bin/simplePipeIn
	rm -f $(INSTALL_PREFIX)/bin/simplePipeOut
	rm -f $(INSTALL_PREFIX)/bin/rollingDFT

clean:
	rm -f bin/*
