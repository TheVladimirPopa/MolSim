Molecular Simulation Group D
===

### Authors: Leo Pahl, Vladimir Popa, Jakob Semmler

This is the first assignment as part of the Scientific Computing (PSE) Molecular Dynamics (IN0012, IN4229) practical
course. The template this code is based on
can be found [here](https://github.com/TUM-I5/MolSim).

## Requirements

- [cmake](https://cmake.org/) (3.18.4)
- [gcc](https://gcc.gnu.org/) (11.1.0)
- [Doxygen](https://doxygen.nl/) (1.9) (_Not mandatory, only for documentation_)

## Build

1. Create `build` folder.

    ``` bash
    $ mkdir build
    ```

2. Access the newly created `./build` folder

    ``` bash
    $ cd ./build 
    ```

3. Run `cmake` using the following arguments:

   `-D BUILD_DOC=ON` is used to obtain a makefile for the documentation

   `-D BUILD_DOC=OFF` if Doxygen is not installed

    ``` bash
     $ cmake .. -D BUILD_DOC=ON -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++
    ```

4. Create the `MolSim` target with the generated Makefile:

    ```bash 
    $ make
    ```

## Usage

```
Usage
        ./MolSim -i <filepath> [-f <input-file>] [-t (single|cuboid|sphere)] [-o <output-file>] [-e <endtime>]
                                [-d <deltaT>] [-w <iteration-count>] [-n] [-p] [-r] [-s] [-c] [-v] [-v] [-q]

OPTIONS:
        -i <filepath>, --xml=<filepath>
                Use the given <filepath> of an XML file as an input for the simulation.
        
        -s, --export-checkpoint
                When set, the state of the particles after the simulation are saved to
                the file ../input/checkpoint.txt
                
        -c, --input-checkpoint
                When set, a previous checkpoint is taken as an input, in addition to the 
                XML input

        -o <filepath>, --output-name=<filepath>
                Use the given <filepath> as the path for 
                the outputfiles(iteration number and file-ending are added automatically)
                If not specified "MD_vtk" is used
                
        -n, --no-output
                If active no files will be written, even overwrites -o.

        -t (single|cuboid|sphere), --type=(single|cuboid|sphere)
                Specifies the way to set up particles (default is single).
                Use single if you want particles on their own and use 
                cuboid if you want the particles to spawn in cuboids.

        -f <filepath>, --input-file=<filepath>
                Use the file at the <filepath> as an input.

        -e <endtime>, --end-time=<endtime>
                The time until the simulation is run (default is 1000).

        -d <deltaT>, --delta-t=<deltaT>
                The timestep by which the time gets increased 
                in every iteration (default is 0.014).

        -w <iteration-count>, --write-frequency=<iteration-count>
                Every <iteration-count>nth iteration the particles 
                get written to a file (default is 10).
        
        -p, --performance
                Takes a performace measurement of the simulation, 
                implicitly sets the -n flag and deactivates logging entirely.
                
        -r, --hit-rate
               Measures the hit-rate of the pairwise force calculation.
               It's defined as the number of pairwise force calculations which 
               were within the cutoff radius (a hit), divided by the total 
               number of pairwise force calculations.

        
        -v, --verbose
                If specified the log-level is lowered from INFO to DEBUG.
                If specified twice the log-level is even lowered to TRACE.
                
        -q, --quiet
                Set loglevel to ERROR. Overrites -v.

        -h, --help
                Prints this help screen.
```

Running the target with faulty parameters will return an appropriate message.

## Input file format

We support two different file-formats (along with the XML file input). For both of them holds:

- Lines of comment start with '#' and are only allowed at the beginning of the file
- Empty lines are not allowed.
- The first line not being a comment has to be one integer, indicating the number of molecule data sets.

### Single particle file(`--type=single`)

A line of molecule data consists of:

* xyz-coordinates (3 double values)
* velocities (2 or 3 double values)
* mass (1 double value)

For example:`0.0 5.36 0.0 -0.425 0.0 0.0   9.55e-4`

### Cuboid file(`--type=cuboid`)

A line of molecule data consists of:

* xyz-coordinates of left front corner (3 double values)
* initial velocities (3 double values)
* dimension (3 positive integers)
* distance of particles (1 double value)
* mass of one particle (1 double value)
* type of particles (1 integer)

For example: `15.0 15.0 0.0   0.0 -10.0 0.0   8 8 1           1.1225          1.0         2`

### Sphere file(`--type=sphere`)

A line of molecule data consists of:

* xyz-coordinates of center (3 double values)
* initial velocities (3 double values)
* radius in number of molecules (1 positive integer)
* dimension (1 positive integer)
* distance of particles (1 double value)
* mass of one particle (1 double value)
* type of particles (1 integer)

For example: `4.0 15.0 0.0    0.0 -10.0 0.0    6         2            1.1225          1.0         2`

## XML File Format

We support XML file input. The file has to include:

* the simulation parameters (endTime, deltaT, writeOutFrequency, filename, cutOffRadius, gravity)
* _(optional)_ preinitialised particles
* a simulationType (containing either Cuboids or Spheres)
* a containerType, describing the strategy used (either LinkedCell for the linkedCells algorithm or VectorContainer for
  DirectSum)
* when using the linkedCellsContainer cellSize and boundaries have to be specified
* a thermostat with the appropriate parameters (initialTemp, targetTemp, maxChangeTemp, periodLength, dimension)
* an input file (by default checkpoint.txt, in order to perform checkpointing)

The following is an example of how an XML input file:
```xml

<Simulation_XML endTime="10.0" deltaT="0.019" writeOutFrequency="20" filename="MD.vtk" cutOffRadius="2.0"
                gravity="-12.44">

    <Particle id="0" epsilon="1.0" sigma="1.0"/>
    <Particle id="1" epsilon="1.0" sigma="0.9412"/>

    <SimTypes>
        <Sphere radius="3" dimension="2" distance="1.1225" mass="1.0" type="1">
            <position x="0.0" y="0.0" z="0.0"/>
            <velocity x="0.0" y="0.0" z="0.0"/>
        </Sphere>

        <Sphere radius="6" dimension="2" distance="1.1225" mass="1.0" type="0">
            <position x="4.0" y="15.0" z="0.0"/>
            <velocity x="0.0" y="-10.0" z="0.0"/>
        </Sphere>

        <Sphere radius="2" dimension="3" distance="1.1225" mass="1.0" type="1">
            <position x="4.0" y="-15.0" z="0.0"/>
            <velocity x="0.0" y="-10.0" z="0.0"/>
        </Sphere>

        <!--        <Cuboid distance="1.23" mass="0.5" type="1">-->
        <!--            <position x="4.0" y="4.0" z="0.0"/>-->
        <!--            <dimension x="3" y="3" z="3"/>-->
        <!--            <velocity x="0.0" y="0.0" z="10.0"/>-->
        <!--        </Cuboid>-->

    </SimTypes>

    <Container_T>
        <LinkedCell cellSize="10.0">
            <leftLowerBound x="-15.0" y="-20.0" z="-5"/>
            <rightUpperBound x="55.0" y="30.0" z="5"/>
            <left>REFLECT</left>
            <right>REFLECT</right>
            <top>PERIODIC</top>
            <bottom>REFLECT</bottom>
            <front>REFLECT</front>
            <back>REFLECT</back>
        </LinkedCell>
    </Container_T>

    <Thermostat initialTemp="40" targetTemp="40" maxTempChange="400" periodLength="1000" dimension="2"/>

    <InputFile path="../input/checkpoint.txt"/>
</Simulation_XML>
```

## Visualisation (_optional_)

In order to visualize the results of the simulation, open [Paraview](https://www.paraview.org/).

- Import all `.vtu` output files all at once.
- Click `Apply` after importing them.
- Create a Glyph filter.
- Select the Glyph type Sphere.
- Choose a sensible scaling and disable masking.
- Click `Apply`.
- Center the camera on the data.
- (_optional_) Choose cooler colours and adjust the colour scale.
- (_optional_) Rescale colour range over all time steps.
- Run the simulation.

## Full Documentation

In order to view the full the documentation for the project:

- Run the following from the `MolSim` directory:
    ```bash
    $ cd cmake-build-debug
    $ make doc_doxygen
    ```
- Now open the file `./cmake-build-debug/doxys_documentation/html/index.html` in a browser of choice