# INSTALLING DIMROOM

## Installation (Now Uses CMAKE)

Dimroom was built and tested on a Mac Studio (MacOS 15.5), using the Clang 17.0.0 C++ compiler
and VSCode. It does compile without error in that environment. The unit tests (such as they
are) all pass.

The package has been altered to use the CMake build generator. I do not have a Windows
C++ compiler, so hopefully using CMake will enable the code to build in that environment.

The source code is available on GitHub. Clone `git@github.com:jbtubman/geologic.git`
into a directory.

    $ git clone git@github.com:jbtubman/geologic.git


In the `geologic` directory, create a `build` directory and go into it:

    $ mkdir build
    $ cd build

Create the `Makefile` by running cmake thusly:

    $ cmake ..

This will create the file `build\Makefile`. Then type

    $ make

This will build the execuable file `dimroom` in the `build` directory
and the executable file `test_dimroom` in the `build/test` directory.

To run the tests, in the `geologic/build` directory, enter the command

    $ test/test_dimroom
