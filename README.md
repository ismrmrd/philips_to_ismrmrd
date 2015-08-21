[![DOI](https://zenodo.org/badge/15627/ismrmrd/philips_to_ismrmrd.svg)](https://zenodo.org/badge/latestdoi/15627/ismrmrd/philips_to_ismrmrd)

# philips_to_ismrmrd

A tool for converting Philips raw MR data into [ISMRMRD](https://ismrmrd.github.io) format.

## Dependencies

- [ISMRMRD](https://github.com/ismrmrd/ismrmrd)
- [CMake](http://www.cmake.org/) for building
- [Boost](http://www.boost.org/)
- [Libxslt](http://xmlsoft.org/libxslt/)
- [Libxml2](http://xmlsoft.org/)

## Build

### Mac OS X

1. Install dependencies
	- Example commands to acquire dependencies and build `ismrmrd` using `brew` tested on Mac OS X 10.10.5 Yosemite, XCode version 2339
		* If you already have `brew`, you should make sure it is up-to-date
		
		```
		brew update
		```
	
		* Otherwise, install `brew`
		
		```
		ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
		```
		
		* Then install `ismrmrd` dependencies
		
		```
		brew install wget cmake doxygen boost fftw
		```
		
		* If you have the `anaconda` distribution of `python`, you should already have `hdf5`, but you should make sure everything is up-to-date
		
		```
		conda update
		```
		
		* Otherise, let `brew` install `hdf5`
		
		```
		brew tap homebrew/science
		brew install homebrew/science/hdf5
		```
		
		* Download, compile and install `ismrmrd`
		
		```
		git clone https://github.com/ismrmrd/ismrmrd
		cd ismrmrd
		mkdir build
		cd build
		cmake ..
		make install
		``` 

2. Get the code: 

   ```
   git clone https://github.com/ismrmrd/philips_to_ismrmrd
   ```
   
3. Generate build files, compile, and install:

    ```
    cd philips_to_ismrmrd
    mkdir build
    cd build
    cmake ..
    make install
    ```

	+ If `cmake` complains about missing `libxml2` and/or `libxslt`, you can use `brew` to fix it
		
		```
		brew install libxml2
		brew link --force libxml2
		```
		
	+ and/or
		
		```
		brew install libxslt
		brew link --force libxslt
		``` 
	+ then try again
	
		```
		cmake ..
		make install
		```
	
### Linux

1. Install dependencies
	- Example commands to acquire dependencies and build `ismrmrd` using `apt-get` tested on Ubuntu 14.04.3 LTS
	
		```
		sudo apt-get update
		sudo apt-get install build-essential git cmake cmake-qt-gui doxygen libboost-all-dev fftw-dev libhdf5-serial-dev hdf5-tools
		git clone https://github.com/ismrmrd/ismrmrd
		cd ismrmrd
		mkdir build
		cd build
		cmake ..
		make
		sudo make install
		sudo ldconfig
		``` 
	- Example commands to acquire `philips_to_ismrmrd` dependencies `libxml` and `libxslt` using `apt-get` tested on Ubuntu 14.04.3 LTS
	
		```
		sudo apt-get install libxml2-dev libxslt1-dev
		```

2. Get the code: 

   ```
   git clone https://github.com/ismrmrd/philips_to_ismrmrd
   ```
   
3. Generate build files, compile, and install:

    ```
    cd philips_to_ismrmrd
    mkdir build
    cd build
    cmake ..
    sudo make install
    ```

### Windows 7 (tested with Visual Studio Pro 2013)
1. Compile and install 32-bit version (not 64-bit version) of `ismrmrd` following steps inspired by instructions found [here](http://sourceforge.net/p/gadgetron/home/Windows%20Installation/)
	- Add `C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin` to system `path` environment variable
	- Copy 32-bit [FFTW3 binaries](http://www.fftw.org/install/windows.html) to `C:\Libraries\FFTW3`
	- Create FFTW3 .lib files on the command line with:
	
		```
		c:\Libraries\FFTW3>lib /machine:x86 /def:libfftw3f-3.def
		c:\Libraries\FFTW3>lib /machine:x86 /def:libfftw3-3.def
		c:\Libraries\FFTW3>lib /machine:x86 /def:libfftw3l-3.def
		```
	
	- Add `C:\Libraries\FFTW3` to the system path environment variable
    - Install `boost_1_55_0-msvc-12.0-32.exe` available [here](http://sourceforge.net/projects/boost/files/boost-binaries/1.55.0/) to location `C:\Libraries\boost_1_55_0`
    - Install `hdf5-1.8.15-win32-vs2013-shared.zip` available [here](https://www.hdfgroup.org/ftp/HDF5/releases/hdf5-1.8.15/bin/windows/) to location `C:\local\HDF_Group\HDF5\1.8.15\`
    - Install CMake available [here](http://www.cmake.org/files/v3.3/cmake-3.3.0-win32-x86.exe) and add to system `path` environment variable
    - Get the `ismrmrd` code using `git clone https://github.com/ismrmrd/ismrmrd` or download zip and save to a parent folder located at `C:\Libraries\ismrmrd`
    - Create folder `C:\Libraries\ismrmrd\build`
    - Set the following system environment variables (forward slashes, no quotes)
    
    	```
    	FFTW3_ROOT_DIR = C:/Libraries/FFTW3
    	BOOST_ROOT = C:/Libraries/boost_1_55_0
    	BOOST_LIBRARYDIR = C:/Libraries/boost_1_55_0/lib32-msvc-12.0
    	HDF5_DIR = C:/local/HDF_Group/HDF5/1.8.15/cmake
    	```
    
    - Run CMake GUI with following entries
    
    	```
    	Where is the source code: C:/Libraries/ismrmrd
		Where to build the binaries: C:/Libraries/ismrmrd/build
		```
		
	- Click CMake Configure button and select options
	
		```
		Generator: Visual Studio 12 2013
		Use default native compilers
    	```
    	
	- Open `C:\Libraries\ismrmrd\build\ISMRMRD.sln` created by CMake
	- Change from `Debug` to `Release`
	- In Solution Explorer window, right-click `ismrmrd` and select `properties -> C/C++ -> General -> Additional Include Directories` and insert `C:\local\HDF_Group\HDF5\1.8.15\include`
	- Using a text editor outside Visual Studio, replace all occurrences of `bin\hdf5.dll` with `lib\hdf5.lib` in `./ismrmrd.vcxproj`, `./examples/c/ismrmrd_c_example.vcxproj`, `./utilities/ismrmrd_generate_cartesian_shepp_logan.vcxproj`, `./utilities/ismrmrd_info.vcxproj`, `./utilities/ismrmrd_read_timing_test.vcxproj`, `./utilities/ismrmrd_recon_cartesian_2d.vcxproj`
	- Return to Visual Studio, select "Reload All" to load altered .vcxproj files
	- Select `BUILD -> Build Solution`
	- Install NSIS Windows installer creator available [here](http://nsis.sourceforge.net/Download)
	- Create a `README.html` file for packaging
	
		```
		copy C:\Libraries\ismrmrd\README.md C:\Libraries\ismrmrd\README.html
		```
	- Edit `C:\Libraries\ismrmrd\build\CPackConfig.cmake` and replace all occurrences of `"C:/Program Files (x86)/ISMRMRD"` with `"/ISMRMRD"`
	
	- Create installer package from the command line
	
	```
	cd C:\Libraries\ismrmrd\build
	cpack
	```

	- Run installer created at location `C:/Libraries/ismrmrd/build/ismrmrd-1.3.2.exe` and select add ISMRMRD to path for all users and install to parent folder `C:\local\ismrmrd`

	- Add `C:\local\ismrmrd\ISMRMRD\lib` to the system path environment variable

2. Download `philips_to_ismrmrd` dependencies `libxml` and `libxslt`
	- Download `iconv-1.9.2.win32.zip`, `libxml2-2.6.30.win32.zip`, `libxslt-1.1.26.win32.zip` and `zlib-1.2.5.win32.zip` available [here](http://xmlsoft.org/sources/win32/) and install to `C:\Libraries\iconv-1.9.2.win32`, `C:\Libraries\libxml2-2.6.30.win32`, `C:\Libraries\libxslt-1.1.26.win32` and `C:\Libraries\zlib-1.2.5` respectively
	- Add `C:\Libraries\iconv-1.9.2.win32\bin;C:\Libraries\libxml2-2.6.30.win32\bin;C:\Libraries\libxslt-1.1.26.win32\bin;C:\Libraries\zlib-1.2.5\bin` to the system path environment variable
	
3. Get the `philips_to_ismrmrd` code using `git clone https://github.com/ismrmrd/philips_to_ismrmrd` or download zip with parent folder located at `C:\Libraries\philips_to_ismrmrd`

4. Prepare to compile from the command-line

	```
	cd "c:\Program Files (x86)\Microsoft Visual Studio 12.0\VC"
	vcvarsall x86
	```
	
5. Compile `philips_to_ismrmrd.exe`

	```
	cd c:\Libraries\philips_to_ismrmrd
	cl main.cpp pugixml.cpp /EHa /Fephilips_to_ismrmrd.exe /GS /TP /W3 /Zc:wchar_t- /Gm- /O2 /Ob2 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_WIN32" /D "UNICODE" /D "_UNICODE" /D "_CRT_SECURE_NO_WARNINGS" /D "__func__=__FUNCTION__" /WX- /Zc:forScope /Gd /MD /I"C:/Libraries/iconv-1.9.2.win32/include/" /I"C:/Libraries/libxml2-2.6.30.win32/include/" /I"C:/Libraries/boost_1_55_0/" /I"C:/Libraries/libxml2-2.6.30.win32/include/" /I"C:/Libraries/libxslt-1.1.26.win32/include/" /I"C:/local/ismrmrd/ISMRMRD/include/" /link /libpath:"C:/Libraries/libxml2-2.6.30.win32/lib/" libxml2.lib /libpath:"C:/Libraries/libxslt-1.1.26.win32/lib/" libxslt.lib /libpath:"C:/local/ismrmrd/ISMRMRD/lib/" ismrmrd.lib /libpath:"C:/Libraries/boost_1_55_0/lib32-msvc-12.0/" /DYNAMICBASE "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "comdlg32.lib" "advapi32.lib"
	```
 
 6. Use `philips_to_ismrmrd.exe` on another Windows 7 computer by gathering the following files into a single folder to copy to the other computer. Alternatively, use the [philips_to_ismrmrd_exe](https://github.com/welcheb/philips_to_ismrmrd_exe) tool designed for use on a Philips MRI Scanner console running Windows 7 (Philips MRI software release 5 or newer)
 
 	```
 	C:\Libraries\philips_to_ismrmrd\philips_to_ismrmrd.exe
 	C:\Libraries\philips_to_ismrmrd\IsmrmrdPhilips.xsl
 	C:\local\HDF_Group\HDF5\1.8.15\bin\hdf5.dll
 	C:\Libraries\iconv-1.9.2.win32\bin\iconv.dll
 	C:\local\ismrmrd\ISMRMRD\lib\ismrmrd.dll
 	C:\Libraries\libxml2-2.6.30.win32\bin\libxml2.dll
 	C:\Libraries\libxslt-1.1.26.win32\bin\libxslt.dll
 	C:\local\ismrmrd\ISMRMRD\bin\mscvcp120.dll
 	C:\local\ismrmrd\ISMRMRD\bin\msvcr120.dll
 	C:\local\HDF_Group\HDF5\1.8.15\bin\szip.dll
 	C:\local\HDF_Group\HDF5\1.8.15\bin\zlib.dll
 	C:\Libraries\zlib-1.2.5\bin\zlib1.dll
 	```
 
## Running the converter

### Simple conversion:

```
philips_to_ismrmrd -f <PHILIPS FILE (without extenssion) > -x IsmrmrdPhilips.xsl -o philips.h5
```

### Conversion with schema validation:

```
philips_to_ismrmrd -f <PHILIPS FILE (without extenssion) > -x IsmrmrdPhilips.xsl -s <PATH TO ISMRMRD SCHEMA> -o philips.h5
```
