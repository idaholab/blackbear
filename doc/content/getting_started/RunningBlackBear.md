# Obtaining and Running BlackBear

## Checking Out the Code

BlackBear uses the git revision control system, and its repository is
hosted on GitHub at [https://github.com/idaholab/blackbear](https://github.com/idaholab/blackbear).
The following steps are required for the initial checkout of
the code:

First, create a GitHub account and add an SSH key to your GitHub profile. Note that
this is not completely necessary, as you can check out the code without a GitHub
account using the https protocol.  However, using ssh with a GitHub account is recommended,
as it makes it easier to contribute to the code later on. The instructions provided here just
document the steps to take, and more detailed information
on using ssh keys with GitHub is available [here](https://help.github.com/articles/connecting-to-github-with-ssh/).

First generate a public/private keypair and show the generated public key by
typing the following in a terminal:

```bash
ssh-keygen -t rsa -C "your_email"
cat ~/.ssh/id_rsa.pub
```

Then copy and paste the key to the 'SSH and GPG keys' section under the 'Settings' menu
in your GitHub user profile. Next clone the BlackBear repository. This assumes
that you want the code to be in a directory named `projects` in your home directory, which
you have already created:

```bash
cd ~/projects/
git clone git@github.com:idaholab/blackbear.git
```

BlackBear requires a checkout of the MOOSE repository to link against. By default, the BlackBear
build process looks for that in a directory called `moose` located in the same directory
that contains the `blackbear` directory. Clone the MOOSE repository using the following command:

```bash
cd ~/projects/
git clone git@github.com:idaholab/moose.git
```

After doing this, the `~/projects` directory should contain `blackbear` and `moose` subdirectories:
```bash
$ ls ~/projects/
blackbear moose
```

It is necessary to build libMesh within the MOOSE repository before building any application:

```bash
cd ~/projects/moose/scripts
./update_and_rebuild_libmesh.sh
```

On a multiprocessor machine, this process can optionally be done in parallel by setting
the `JOBS` environment variable equal to the number of processors to be used. For example, to 
build using 8 processors, the libMesh build script can be run as follows:
```bash
JOBS=8 ./update_and_rebuild_libmesh.sh
```

Once libMesh has compiled successfully, you may now compile BlackBear:

```bash
cd ~/projects/blackbear/
make (add -jn to run on multiple "n" processors)
```

Once BlackBear has compiled successfully, it is recommended to run the tests
to make sure the version of the code you have is running correctly.

```bash
cd ~/projects/blackbear/
./run_test (add -jn to run "n" jobs at one time)
```

## Updating BlackBear

If it has been some time since you have checked out the code an update
will be required to gain access to the new features in BlackBear.
First update BlackBear:

```bash
cd ~/projects/blackbear/
git pull --rebase
```

Then update MOOSE:

```bash
cd ~/projects/moose/
git pull --rebase
```

Next rebuild libMesh:

```bash
cd ~/projects/moose/scripts/
./update_and_rebuild_libmesh.sh
```

And finally recompile BlackBear:

```bash
cd ~/projects/blackbear/
make (add -jn to run on multiple "n" processors)
```

## Executing BlackBear

When first starting out with BlackBear, it is recommended to start from an
example problem similar to the problem that you are trying to solve.
Multiple examples can be found in blackbear/test/tests/.
It may be worth running the example problems to see how the code works
and modifying input parameters to see how the run time, results and
convergence behavior change.

To demonstrate running BlackBear, consider running one of the regression tests:

```bash
cd blackbear/test/tests/concrete_ASR_swelling
# To run with one processor
~/projects/blackbear/blackbear-opt -i asr_confined.i
# To run in parallel (2 processors)
mpiexec -n 2 ~/projects/blackbear/blackbear-opt -i asr_confined.i
```

Note that the procedure for running this model in parallel is shown only
for illustrative purposes. This particular model is quite small, and would
not benefit from being run in parallel, although it can be run that way.

## Input to BlackBear

BlackBear simulation models are defined by the user through a text file
that defines the parameters of the run.  This text file specifies the
set of code objects that are composed together to simulate a physical
problem, and provides parameters that control how those objects behave
and interact with each other.  This text file can be prepared using any
text editor.

In addition to the text file describing the model parameters, BlackBear also
requires a definition of the finite element mesh on which the physics
equations are solved. The mesh can be generated internally by BlackBear using
parameters defined in BlackBear's input file for very simple geometries, or can
be read from a file as defined in the BlackBear input file. BlackBear supports the
full set of mesh file formats supported by MOOSE, although the most common
mesh format is the ExodusII format.

## Post Processing

BlackBear typically writes solution data to an ExodusII file. Data may also
be written in other formats, a simple comma separated file giving global
data being the most common.

Several options exist for viewing ExodusII results files. These include
commercial as well as open-source tools. One good choice is ParaView,
which is open-source.

ParaView is available on a variety of platforms. It is capable of
displaying node and element data in several ways. It will also produce
line plots of global data or data from a particular node or element.
Detailed information on ParaView is available on its project
[website](https://www.paraview.org).

## Graphical User Interface

It is worth noting that a graphical user interface (GUI) exists for all
MOOSE-based applications. This GUI is named Peacock. Information about
Peacock and how to set it up for use may be found on
[the MOOSE wiki page](http://mooseframework.org/wiki/Peacock).

Peacock may be used to generate a text input file. It is also capable of
submitting the analysis. It also provides basic post processing
capabilities.
