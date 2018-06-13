# cdx
The Channel Data Exchange (CDX) File Format

At all stages of radio channel sounding, estimation, modeling, and simulation, large data sets have to be processed.

The Channel Data Exchange (CDX) file format is proposed for channel data recording, storage, and processing in a convenient and portable way.

It is based on the HDF5 format and allows for the storage of large data sets.

Bindings exist to create and read CDX files for C++, Python, Matlab, and Octave.

In the CDX file, floating point data can be stored platform independently. CDX is able to store multiple transmitter-receiver links.

CDX allows for the storage of two different types of data:

- continuous-delay data: data consisting of path components with a continuous delay as produced by channel models
- discrete-delay data: data sampled with a certain sampling frequency, e.g. by a channel sounder

The full documentation can be found here: http://fms13.github.io/cdx/
