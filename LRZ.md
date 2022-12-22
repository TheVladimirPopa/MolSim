# Credentials

Name: di34mit

Password: Look on discord, not gonna put it here xD

# Connecting

1. Connect to a login node

   CoolMUC-2: lxlogin1, lxlogin2, lxlogin3, **lxlogin4**

   CoolMUC-3: lxlogin8 (DON'T compile on this login node. Computation nodes have other architecture.)

    ```bash
    ssh -CY di34mit@lxlogin4.lrz.de
    ```

2. Navigate to our scratch space
    ```bash
    cd $SCRATCH/di34mit
    ```

3. Load modules
    ```
    module load cmake/3.14.5
    module load gcc/11
    module load pkgconf/1.8.0
    module load xerces-c/3.2.1
    module load doxygen/1.9.2
    module load intel-oneapi-vtune/2021.7.1
    ```

Note: icpx will only work with `cmake/3.21.4`.


# Exchanging files

Example: Upload of `./MolSim` into the scratch space (Note: compilation should not be down on the local machine.)

```
scp ./MolSim di34mit@lxlogin4.lrz.de:/gpfs/scratch/t1221/di34mit/di34mit/
```

Example: Download of vtune.zip

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

Load / unload the following modules in the given ordner.

```
module load cmake/3.21.4
module load pkgconf/1.8.0
module load xerces-c/3.2.1
module load doxygen/1.9.2
module unload intel
module load intel-oneapi-compilers/2022.2.0
```

Then build:

```
cmake -DCMAKE_C_COMPILER=icx -DCMAKE_CXX_COMPILER=icpx .. -DCMAKE_RULE_MESSAGES:BOOL=OFF -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
```

```
make MolSim --no-print-directory
```

When mixing compilers and respective modules things can get complicated. To ensure a clean environment.

# Contest 1 Measurements
1. Compile as described above with g++ or icpx
2. Create the following file and set stdout, stderr and the working directory accordingly.

File: `sbatch_contest1.sh`
```bash
#!/bin/bash
# Job Name (MUST BE 10 or less characters)
#SBATCH -J MSimGrDTst
#Output and error (also --output, --error):
#SBATCH -o /gpfs/scratch/t1221/di34mit/di34mit/2022-12-15_tests/stdout.txt
#SBATCH -e /gpfs/scratch/t1221/di34mit/di34mit/2022-12-15_tests/stderr.txt
#Initial working directory (also --chdir):
#SBATCH -D /gpfs/scratch/t1221/di34mit/di34mit/2022-12-15_tests/MolSim/build
#Notification and type
#SBATCH --mail-type=ALL
#SBATCH --mail-user=j.semmler@tum.de
# Wall clock limit:
#SBATCH --time=00:20:00
#Setup of execution environment
#SBATCH --export=NONE
#SBATCH --get-user-env

# Once the first line without # is encountered, sbatch stops reading parameters
module load xerces-c/3.2.1

./MolSim -i "../input/Assignment4/Task2/task2_bigExp.xml" -o operf -r -p

```

3. run sbatch `sbatch sbatch_contest1.sh`

The result of the performance measurment will appear in the `stdout.txt` after the job is completed.