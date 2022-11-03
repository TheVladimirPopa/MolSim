Molecular Simulation Assignment 1 Group D
===
### Authors: Leo Pahl, Vladimir Popa, Jakob Semmler

This is the first assignment as part of the Scientific Computing (PSE) Molecular Dynamics (IN0012, IN4229) practical course. The template this code is based on
can be found [here](https://github.com/TUM-I5/MolSim).

## Requirements
- [cmake](https://cmake.org/) (3.18.4)
- [gcc](https://gcc.gnu.org/) (11.1.0)
- [Doxygen](https://doxygen.nl/) (1.9) (_Not mandatory, only for documentation_)

## Build
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
# -D BUILD_DOC=ON is used to obtain a makefile for the documentation
# -D BUILD_DOC=OFF if Doxygen is not installed
 $ cmake .. -D BUILD_DOC=ON -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++
```
4. Create the `molsim` target with the generated Makefile:
```bash 
$ make
```

# Usage
In order to perform simulation, run the `molsim` target with command line arguments 
`input_file`, `end_time` and `delta_t`, _e.g._ :
```bash
# simulation has an end time of 1000 and the difference between time segments (delta_t) is 0.014, with 
# the input taken from the file ../input/eingabe-sonne.txt
./MolSim ../input/eingabe-sonne.txt 1000 0.014
```
Running the target with faulty parameters or with fewer parameters will return an appropriate output.

# Visualisation (_optional_)
In order to visualize the results of the simulation, open [Paraview](https://www.paraview.org/).
- Import all `.vtu` output files all at once.
- Click `Apply` after importing them.
- Create a Glyph filter.
- Select the Glyph type Sphere.
- Choose a sensible scaling and disable masking.
- Click `Apply`.
- Center the camera on the data.
- (_optional_) Choose cooler colours nad adjust the colour scale.
- (_optional_) Rescale colour range over all time steps.
- Run the simulation.

# Full Documentation
In order to view the full the documentation for the project:
- Run the following from the `MolSim` directory:
```bash
$ cd cmake-build-debug
$ make doc_doxygen
```
- Now open the file `./cmake-build-debug/doxys_documentation/html/index.html` in a browser of choice