# philips_to_ismrmrd

A tool for converting Philips raw MR data into [ISMRMRD](https://ismrmrd.github.io) format.

## Dependencies

- [ISMRMRD](https://github.com/ismrmrd/ismrmrd)
- [CMake](http://www.cmake.org/) for building
- [Boost](http://www.boost.org/)
- [Libxslt](http://xmlsoft.org/libxslt/)
- [Libxml2](http://xmlsoft.org/)

## Build

### Linux and OS X

1. Install dependencies
2. Get the code: `git clone https://github.com/ismrmrd/philips_to_ismrmrd`
3. Generate build files, compile, and install:
    ```
    cd philips_to_ismrmrd
    mkdir build
    cd build
    cmake ..
    make install
    ```
