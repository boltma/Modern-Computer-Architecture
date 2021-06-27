Getting Started
===
The gem5-NVP is a simulating framework for non-volatile processors (NVP) and non-volatile systems based on the gem5 simulator. It provides multiple interfaces for adding description of SimObjects' energy behavior as well as an energy management module which allows users to simulate state changes (power-on, power-off, etc.) easily.

## <span id="contents">Contents</span>
* [Contents](#contents)
* [Pre-Requirement](#prereq)
* [Download](#download)
* [Build](#build)
* [Usages](#usages)

## <span id="prereq">Pre-Requirement</span>
Gem5-NVP depends on nothing besides the dependencies of gem5 simulator. Tools needed before building gem5-NVP can be found at http://www.gem5.org/Dependencies.

## <span id="download">Download</span>
Gem5-NVP uses github to control its version. The latest stable version can be found at https://github.com/zlfben/gem5:master, and there are previous versions among tags of the repository (nvp-v*.*). You can get the latest stable version like this:
```Bash
git clone https://github.com/zlfben/gem5.git
cd gem5
git checkout master
```

## <span id="build">Build</span>
The build process of gem5-NVP is almost the same as gem5 simulator. Scons is used to build target system to be simulated. Theoretically, different ISAs are supported by gem5-NVP. However, gem5-NVP is tested under ARM, so the following build option is recommended:
```Bash
mv README.md README
scons build/ARM/gem.debug
```
One tricky thing is that gem5 checks its original raw README during its build, so we can simply rename README.md into README. Markdown file is used only for better github page.

## <span id="usages">Usages</span>
Gem5-nvp provides an alternated script based on "./configs/example/se.py" to simulate energy-related systems, using System Call Emulation Mode in gem5. The path to the script is "./configs/example/se_engy.py". Gem5-nvp has several interfaces for users to control the energy behavior of the whole system. Most of the interfaces present as arguments of the "se_engy.py" script, while others (mostly debug output control) present as arguments of the target system such as "build/ARM/gem5.debug". Note: Target system in gem5 already receives arguments to control debug output, and gem5-NVP only adds some debug flags for energy behavior.
#### Arguments of Simulating Script
|Argument |Description |
|:-----------------------------|:---------------------------------------------------------------------------------------|
|--energy-prof={FILE} |Locate the path to energy profile. |
|--energy-time-unit={TIME_UNIT}|Set time unit of energy profile in ticks. (Default 1000) |
|--energy-modules={MODULES} |Set which module we care about in case of energy. Separated by space. Example: "cpu mem"|
#### Arguments of Target System Related to gem5-NVP
|Argument |Description |
|:--------------------|:---------------------------------------------------------------------------------|
|--debug-flags={FLAGS}|Decide what kinds of debug information to print. "EnergyMgmt" for energy behavior.|
|--debug-file={FILE} |Set the target file for debug output. |
#### Example
The following examples will simulate systems running "./test/test-progs/hello/bin/arm/linux/hello".
To run exactly like gem5 (No energy behavior is simulated):
```Bash
build/ARM/gem5.debug configs/example/se_engy.py -c tests/test-progs/hello/bin/arm/linux/hello
```
To simulate atomic cpu's energy behavior, using energy profile "./profile/energy_prof", with energy_time_unit = 100, printing all the debug information of energy behavior into "./m5out/a.out":
```Bash
build/ARM/gem5.debug --debug-flags=EnergyMgmt --debug-file=a.out configs/example/se_engy.py -c tests/test-progs/hello/bin/arm/linux/hello --energy-profile=./profile/energy_prof --energy-time-unit=100 --energy-modules='cpu'
```

Gem5-NVP also supports more advanced functions which require developers to modify source code, like simulating systems with 2-threshold energy state machine. Tutorials and examples of such usage can be found at https://github.com/zlfben/gem5/wiki.

gem5 README
===
This is the gem5 simulator.

The main website can be found at http://www.gem5.org

A good starting point is http://www.gem5.org/Introduction, and for
more information about building the simulator and getting started
please see http://www.gem5.org/Documentation and
http://www.gem5.org/Tutorials.

To build gem5, you will need the following software: g++ or clang,
Python (gem5 links in the Python interpreter), SCons, SWIG, zlib, m4,
and lastly protobuf if you want trace capture and playback
support. Please see http://www.gem5.org/Dependencies for more details
concerning the minimum versions of the aforementioned tools.

Once you have all dependencies resolved, type 'scons
build/<ARCH>/gem5.opt' where ARCH is one of ALPHA, ARM, NULL, MIPS,
POWER, SPARC, or X86. This will build an optimized version of the gem5
binary (gem5.opt) for the the specified architecture. See
http://www.gem5.org/Build_System for more details and options.

With the simulator built, have a look at
http://www.gem5.org/Running_gem5 for more information on how to use
gem5.

The basic source release includes these subdirectories:
  - configs: example simulation configuration scripts
  - ext: less-common external packages needed to build gem5
  - src: source code of the gem5 simulator
  - system: source for some optional system software for simulated systems
  - tests: regression tests
  - util: useful utility programs and files

To run full-system simulations, you will need compiled system firmware
(console and PALcode for Alpha), kernel binaries and one or more disk
images. Please see the gem5 download page for these items at
http://www.gem5.org/Download

If you have questions, please send mail to gem5-users@gem5.org

Enjoy using gem5 and please share your modifications and extensions.
