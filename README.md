# libNCPA
Useful utilities and libraries for acoustics work

# Important Note:
This was compiled and installed on an x86 Linux installation. Specifically, on Void Linux:
https://voidlinux.org/

# Running Container
For users not using Void Linux, it is recommended that you use the ContainerFile to utilize this library.
The following instructions are for using Docker, but you can use your container manager of choice.
It is assumed that your working directory is this repository.

First, build the image on your system:
```
docker build -tag void-ncpa .
```
Note: '-tag' refers to the desired name of the container image.

Next, to run the image in a container:
```
docker run --interactive --tty void-ncpa
```
or the abbreviated command:
```
docker run -it void-ncpa
```
Note: '-interactive' refers to leaving stdin open which means you can stream data into the container through stdin. '-tty' refers to allocating a pseudo-TTY, which means that you have a shell environment to utilize the library's tools.

To return to an already running container,
```
docker exec --interactive --tty <CONTAINER_NAME> bash
```
or the abbreviated command:
```
docker exec -it <CONTAINER_NAME> bash
```
Note: you can replace 'bash' with your desired shell.
However, you will need to recompile the image after modifying the ContainerFile to install your desired shell.
Or, you can install your desired shell in the container:
```
xbps-install -Syu <DESIRED_SHELL>
```

To stream data into your container, you can utilize pipes:
```
cat <DATA> | docker run -i void-ncpa sh -c '<COMMANDS>'
xstream <OPTIONS> | docker run -i void-ncpa sh -c '<COMMANDS>'
```

# Dependencies
These are dependencies for some of the transformers in this library.


http://fftw.org/

https://github.com/berndporr/iir1

https://arma.sourceforge.net/docs.html


If using Void Linux, simply
```
sudo xbps-install -Su fftw-devel libiir1-devel armadillo-devel
```

Otherwise, follow the instructions to install from source.
Be sure to append the following to your bashrc or other startup script if installing from source:
```
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```
Or run the executables as
```
LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH <EXECUTABLE> <...>
```

# Compiling
```
make
```

# Installing
```
sudo make install
```

# Notes
The underlying design principles of the software library are that it runs in the command-line and utilizes pipes to handle inputs and outputs.
There are four types of objects: buffers, transformers, detectors, and generators.
The buffer implementation is utilized by the transformers, detectors, and generators.
Another key design principle in creating this library is that each class is implemented in a header file.
This means that there are two functional ways to use the library: integrating operations together into a single compiled C++ executable or implementing operations as separate C++ executables and piping them together in a shell script.
This flexibility is especially useful for testing the send/receive mechanisms with different detectors.

## Buffers
There is a parent BUFFER class in BUFFER.h.
This handles how data is stored and shared between objects.
The arrays are dynamically allocated and the users are expected to use the accessor functions to modify/use the array as a data storage object.
The accessor functions return a std::span (https://en.cppreference.com/w/cpp/container/span) which has the same nice functions as other standard library containers while maintaining a consistent memory layout.
The reason why std::span was chosen over std::vector is because of the vector implementation for Booleans are stored bit-wise to be more compact which breaks the inputBuffer and outputBuffer implementations.
BUFFER objects utilize reclaim(), steal(), and borrow() to determine how memory is shared between BUFFER objects.
The reclaim() function reclaims memory which means that the BUFFER owns the memory from that point forward.
The steal() function claims the memory of another BUFFER object.
The borrow() function utilizes the memory of the other BUFFER object.
These functions are especially important for weaving operations together into a single compiled C++ executable because they allow for mitigating I/O operations between the library's other objects.

The inputBuffer, outputBuffer, and geometryBuffer inherit the properties of BUFFER which means that they utilize the same functions as the parent (accessors, mutators, etc.).
The inputBuffer can read in data from stdin as either human-readable (ASCII) or raw binary.
This is done by function-overloading the read() function to take in a file pointer (FILE\*) or an input stream (std::istream).
The outputBuffer can write data to stdout as either human-readable (ASCII) or raw binary.
This is done by function-overloading the write() function to take in a file pointer (FILE\*) or an input stream (std::ostream).
The geometryBuffer shares the same properties as the inputBuffer but has an additional method for reading in data: it can input data directly from another std::span.
The geometryBuffer is not currently being utilized by the other classes yet, but it will be necessary for synthetic data generation.
As the library gets used, it can be determined if the utilities of the geometryBuffer may be integrated into the inputBuffer/outputBuffer implementations or if it will remain a separate implementation.

## Transformers
The parent class TRANSFORMER has the following structure: inputBuffer, outputBuffer, and transform() function.
Each inheritor from TRANSFORMER utilizes the I/O implemented by the inputBuffer/outputBuffer objects.
The transform() function operates on the inputBuffer and puts the transformed data into the outputBuffer.

### Examples
The decimator class decimates applies a low-pass filter and decimates incoming data.
The distanceToCoordinates class takes a given distance matrix (entries correspond to distances between points) and outputs their coordinates in a specified number of dimensions.
The fastFT applies the fast Fourier transform to incoming data.
The nullOperator does nothing to the incoming data.
The timeStamper applies a time stamp to incoming Boolean data (time stamped if true).

## Detectors
The parent class DETECTOR is essentially a specialized version of the TRANSFORMER class, but for documentation and ease-of-use it is implemented as a separate object.
Inheritors of DETECTOR take in data into an inputBuffer
The hypothesis\_test() function performs a hypothesis test to see if the incoming data is anomalous.
Then the objects write from an outputBuffer of Booleans.

### Examples
The simpleDetector checks if incoming data is above a specified threshold.
The averageDetector utilizes a short-term average over long-term average to determine if the incoming data was anomalous.
The normalDetector utilizes a p-test for a normal distribution to detect anomalies.

## Compiled Executables
In the src directory of the git repository, there are compilable implementations of the class objects.
These compile as executables for command-line usage.
Essentially, they operate as specified in the previous sections, but they take in data from stdin and output data to stdout.
The only one that differs is the detector executables; they have an additional timeStamper functionality in to output either the Boolean (trigger state) or the time stamp of when the state became triggered.
