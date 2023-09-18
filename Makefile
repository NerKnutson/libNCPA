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

executables: sineMaker noiseMaker noisePropagator simplePipeIn simplePipeOut rollingDFT decimator r2cFFT c2rFFT xcorrelator signalAdd

sineMaker: src/sineMaker.cpp
	$(CXX) $(CFLAGS) $(WARNING) $^ -o bin/$@ $(LDLIBS) 

noiseMaker: src/noiseMaker.cpp
	$(CXX) $(CFLAGS) $(WARNING) $^ -o bin/$@ $(LDLIBS) 

noisePropagator: src/noisePropagator.cpp
	$(CXX) $(CFLAGS) $(WARNING) $^ -o bin/$@ $(LDLIBS)

simplePipeIn: src/simplePipeIn.cpp
	$(CXX) $(CFLAGS) $(WARNING) $^ -o bin/$@ $(LDLIBS)

simplePipeOut: src/simplePipeOut.cpp
	$(CXX) $(CFLAGS) $(WARNING) $^ -o bin/$@ $(LDLIBS)

rollingDFT: src/rollingDFT.cpp
	$(CXX) $(CFLAGS) $(WARNING) $^ -o bin/$@ $(LDLIBS)
	
decimator: src/decimator.cpp
	$(CXX) $(CFLAGS) $(WARNING) $^ -o bin/$@ $(LDLIBS)
	
r2cFFT: src/r2cFFT.cpp
	$(CXX) $(CFLAGS) $(WARNING) $^ -o bin/$@ $(LDLIBS)
	
c2rFFT: src/c2rFFT.cpp
	$(CXX) $(CFLAGS) $(WARNING) $^ -o bin/$@ $(LDLIBS)
	
xcorrelator: src/xcorrelator.cpp
	$(CXX) $(CFLAGS) $(WARNING) $^ -o bin/$@ $(LDLIBS)

signalAdd: src/signalAdd.cpp
	$(CXX) $(CFLAGS) $(WARNING) $^ -o bin/$@ $(LDLIBS)

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
	cp bin/decimator $(INSTALL_PREFIX)/bin/
	cp bin/r2cFFT $(INSTALL_PREFIX)/bin/
	cp bin/c2rFFT $(INSTALL_PREFIX)/bin/
	cp bin/xcorrelator $(INSTALL_PREFIX)/bin/
	cp bin/signalAdd $(INSTALL_PREFIX)/bin/

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
	rm -f $(INSTALL_PREFIX)/bin/decimator
	rm -f $(INSTALL_PREFIX)/bin/r2cFFT
	rm -f $(INSTALL_PREFIX)/bin/c2rFFT
	rm -f $(INSTALL_PREFIX)/bin/xcorrelator
	rm -f $(INSTALL_PREFIX)/bin/signalAdd

clean:
	rm -f bin/*
