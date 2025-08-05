# INSTALLING DIMROOM

## Installation (Using CMAKE)

Installing and running this package requires:

* a C++ compiler;
* the [Git](https://git-scm.com/downloads) version control system; and
* the [CMake](https://cmake.org/download/) build tool.

Dimroom was originally built and tested on a Mac Studio (MacOS 15.5), using the Clang 17.0.0
C++ compiler and VSCode. It does compile without error in that environment.

It has also been tested on 64-bit Windows, using version 19.44.35214 of the Microsoft C++
compiler for VSCode.

### Obtaining the Source Code

The source code is available on GitHub.

#### Mac OS

Clone `git@github.com:jbtubman/dimroom.git` into a directory.

    $ cd ~/src
    $ git clone git@github.com:jbtubman/dimroom.git
    $ cd dimroom

#### Windows

The instructions for Windows must be done in a command window.

Clone `git@github.com:jbtubman/dimroom.git` into a folder.

    C:\Users\asmithee\src> git clone git@github.com:jbtubman/dimroom.git
    C:\Users\asmithee\src> cd dimroom

### Building and Running On Mac OS

In the `dimroom` directory, create a `build` directory and go into it:

    $ mkdir build
    $ cd build

Create the `Makefile` by running CMake thusly:

    $ cmake ..

This will create the file `build\Makefile`. Then type

    $ make

This will build the executable file `dimroom` in the `build` directory
and the executable file `test_dimroom` in the `build/test` directory.

To run the tests, in the `dimroom/build` directory, enter the command:

    $ test/test_dimroom

To run `dimroom`, in the `dimroom/build` directory, enter the command:

    $ ./dimroom ../test/data/sample.csv

### Building and Running On Windows

In the `dimroom` folder, create a `build` folder and go into it:

    C:\Users\asmithee\src\dimroom> mkdir build
    C:\Users\asmithee\src\dimroom> cd build

Create the VS Code solution by running CMake thusly:

    C:\Users\asmithee\src\dimroom\build> cmake ..

This will detect the compiler and create the `dimroom.sln` file.

To build the executables, enter the following command:

    C:\Users\asmithee\src\dimroom\build> cmake --build . --config Debug

This will build the execuable file `dimroom.exe` in the `build\Debug` folder
and the executable file `test_dimroom.exe` in the `build\test\Debug` folder.
(You can also use `Release` instead of `Debug` if desired.)

To run the tests, in the `dimroom\build` folder, enter the command:

    C:\Users\asmithee\src\dimroom\build> .\test\Debug\test_dimroom.exe

To run `dimroom`, enter the command:

    C:\Users\asmithee\src\dimroom\build> .\Debug\dimroom.exe ..\test\data\sample.csv
