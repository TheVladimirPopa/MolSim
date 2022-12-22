# Credentials

Name: di34mit

Password: Look on discord, not gonna put it here xD

# Connecting

1. Connect to a login node

   CooMUC2: lxlogin1, lxlogin2, lxlogin3, **lxlogin4**

   CooMUC3: lxlogin8 (DON'T compile here since computation nodes have other architecture)

    ```bash
    ssh -CY di34mit@lxlogin4.lrz.de
    ```

2. Navigate to our scratch space
    ```bash
    cd $SCRATCH/di34mit
    ```

3. If needed load modules like that:
    ```
    module load cmake/3.14.5
    module load gcc/11
    module load pkgconf/1.8.0
    module load xerces-c/3.2.1
    module load doxygen/1.9.2
    module load intel-oneapi-vtune/2021.7.1
    ```

# Transferring stuff

// Example: Upload of `./MolSim` into Scratch space (MolSim will **not** be executable. You have to compile this on the
server)

```
scp ./MolSim di34mit@lxlogin4.lrz.de:/gpfs/scratch/t1221/di34mit/di34mit/
```

// Example: Download of vtune.zip

```
scp di34mit@lxlogin4.lrz.de:/gpfs/scratch/t1221/di34mit/di34mit/2022-12-15_tests/vtune.zip ./
```

# Jobs

Example for starting a job

1. Compile everything as usual

2. Configure `sbatch.sh`
    ``` bash
    #!/bin/bash
    # SET THIS FIRST 
    #SBATCH --mail-user=j.semmler@tum.de
    #SBATCH --time=00:05:00
    #
    # Job Name (MUST BE 10 or less characters)
    #SBATCH -J MSimGroupD
    #Output and error
    #SBATCH -o /gpfs/scratch/t1221/di34mit/di34mit/stdout.txt
    #SBATCH -e /gpfs/scratch/t1221/di34mit/di34mit/stderr.txt
    #Initial working directory (also --chdir):
    #SBATCH -D /gpfs/scratch/t1221/di34mit/di34mit/2022-12-15_tests/MolSim/build
    #Notification and type
    #SBATCH --mail-type=ALL
    #Setup of execution environment
    #SBATCH --export=NONE
    #SBATCH --get-user-env

    # Once the first line without # is encountered, sbatch stops reading parameters
    module load xerces-c/3.2.1

    ./MolSim -f ../input/worksheet4-periodic-boundary.txt -o task4_periodic_merge -e  0.0006 -d 0.0005 -w 200 -r -t sphere
    ```
3. Run
    ```bash
    . sbatch ./sbatch.sh
    ``` 

# Intel compiler

Since the intel compiler is really "funny", we have to do some things differently here

Use these modules:

```
module load cmake/3.21.4
module load pkgconf/1.8.0
module load xerces-c/3.2.1
module load doxygen/1.9.2
module load intel-mkl/2020
module load intel-oneapi-compilers/2022.2.0
```

Build it like that:

```
cmake -DCMAKE_C_COMPILER=icx -DCMAKE_CXX_COMPILER=icpx .. -DCMAKE_RULE_MESSAGES:BOOL=OFF -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
```

```
make MolSim --no-print-directory
```
