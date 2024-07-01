INSTALL_PREFIX?=/usr/local
INC_DIR=include
SRC_DIR=src
BIN_DIR=bin

SRC=$(wildcard $(SRC_DIR)/*.cpp)
BIN=$(SRC:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%)

CFLAGS=-DMAX_CHANNELS=128 --std=c++20 -I$(INC_DIR)

# ADD MORE WARNINGS!
#WARNING := -Wall -Wextra

# .PHONY means these rules get executed even if
# files of those names exist.
.PHONY: all clean

all: executables

executables: $(BIN)

# Default compilation with no dependent libraries
$(BIN_DIR)/% : $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) $(WARNING) $< -o $@

$(BIN_DIR)/decimator : $(SRC_DIR)/decimator.cpp
	$(CXX) $(CFLAGS) $(WARNING) $< -o $@ -liir

$(BIN_DIR)/c2rFFT : $(SRC_DIR)/c2rFFT.cpp
	$(CXX) $(CFLAGS) $(WARNING) $< -o $@ -lfftw3

$(BIN_DIR)/r2cFFT : $(SRC_DIR)/r2cFFT.cpp
	$(CXX) $(CFLAGS) $(WARNING) $< -o $@ -lfftw3

$(BIN_DIR)/xcorrelator : $(SRC_DIR)/xcorrelator.cpp
	$(CXX) $(CFLAGS) $(WARNING) $< -o $@ -lfftw3

install:
#	cp include/importArrayGeometry.h $(INSTALL_PREFIX)/include/
#	cp include/rollingDFT.h $(INSTALL_PREFIX)/include/
#	cp include/simplePipe.h $(INSTALL_PREFIX)/include/
#	cp bin/sineMaker $(INSTALL_PREFIX)/bin/
#	cp bin/noiseMaker $(INSTALL_PREFIX)/bin/
#	cp bin/noisePropagator $(INSTALL_PREFIX)/bin/
#	cp bin/rollingDFT $(INSTALL_PREFIX)/bin/
#	cp bin/decimator $(INSTALL_PREFIX)/bin/
#	cp bin/r2cFFT $(INSTALL_PREFIX)/bin/
#	cp bin/c2rFFT $(INSTALL_PREFIX)/bin/
#	cp bin/xcorrelator $(INSTALL_PREFIX)/bin/

#cp bin/simplePipeIn $(INSTALL_PREFIX)/bin/
#cp bin/simplePipeOut $(INSTALL_PREFIX)/bin/
#cp bin/signalAdd $(INSTALL_PREFIX)/bin/

uninstall:
#	rm -f $(INSTALL_PREFIX)/include/importArrayGeometry.h
#	rm -f $(INSTALL_PREFIX)/include/rollingDFT.h
#	rm -f $(INSTALL_PREFIX)/include/simplePipe.h
#	rm -f $(INSTALL_PREFIX)/bin/sineMaker
#	rm -f $(INSTALL_PREFIX)/bin/noiseMaker
#	rm -f $(INSTALL_PREFIX)/bin/noisePropagator
#	rm -f $(INSTALL_PREFIX)/bin/rollingDFT
#	rm -f $(INSTALL_PREFIX)/bin/decimator
#	rm -f $(INSTALL_PREFIX)/bin/r2cFFT
#	rm -f $(INSTALL_PREFIX)/bin/c2rFFT
#	rm -f $(INSTALL_PREFIX)/bin/xcorrelator

#rm -f $(INSTALL_PREFIX)/bin/simplePipeIn
#rm -f $(INSTALL_PREFIX)/bin/simplePipeOut
#rm -f $(INSTALL_PREFIX)/bin/signalAdd

clean:
	rm -f bin/*
