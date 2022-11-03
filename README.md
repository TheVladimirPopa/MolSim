Molecular Simulation Assignment 1
===
This is the first assignment as part of the Scientific Computing (PSE) Molecular Dynamics (IN0012, IN4229) practical course. The template this code is based on
can be found [here](https://github.com/TUM-I5/MolSim).
# Installation
```bash
$ git clone https://github.com/TheVladimirPopa/MolSim.git
$ cd MolSim
```
### Build

#### Using the Makefile:

1. Create a `build` folder and rund cmake with make.
```bash
$ mkdir build
$ make
```
2. Access the newly created `./build` folder.
```bash
$ cd ./build
```
3. The `MolSim` target will be created with the generated Makefile.
```bash
$ make
```

#### Using CMake:
1. Create `build` folder.
```bash
$ mkdir build
```
2. Access the newly created `./build` folder
```bash
$ cd ./build 
```
3. Run `cmake` using the following arguments:
```bash
 $ cmake .. -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++
```
4. Create the `molsim` target with the generated Makefile:
```bash 
$ make
```

## Prerequisites
- [cmake](https://cmake.org/) (3.18.4)
- [gcc](https://gcc.gnu.org/) (11.1.0)
- _(optional)_ [Clion](https://www.jetbrains.com/clion/) (IDE for running C++ code)
- _(optional)_ [Doxygen](https://doxygen.nl/) (1.9)
- _(optional)_ [Paraview](https://www.paraview.org/) (5.11.0)

# Usage
In order to perform simulation, run the `molsim` target with command line arguments 
`input_file`, `end_time` and `delta_t`, _e.g._ :
```bash
# simulation has an end time of 1000 and the difference between time segments (delta_t) is 0.014, with 
# the input taken from the file ../input/eingabe-sonne.txt
./MolSim.cpp ../input/eingabe-sonne.txt 1000 0.014
```
Running the target with faulty parameters or with fewer parameters will return an appropriate output.

# Visualisation
In order to visualize the results of the simulation, open Paraview.
- Import all `.vtu` output files all at once.
- Click `Apply` after importing them.
- Create a Glyph filter.
- Select the Glyph type Sphere.
- Choose a sensible scaling and disable masking.
- Click `Apply`.
- Center the camera on the data.
- _(optional)_ Choose cooler colours nad adjust the colour scale.
- _(optional)_ Rescale colour range over all time steps.
- Run the simulation.

# Full Documentation
In order to view the full the documentation for the project:
- Run the following from the `MolSim` directory:
```bash
$ cd cmake-build-debug
$ make doc_doxygen
```
- Now open the file `./cmake-build-debug/doxys_documentation/html/index.html` in a browser of choice