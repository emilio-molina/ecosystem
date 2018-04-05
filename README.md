# ecosystem v0.4\_dev

**Individual-based ecosystem simulation.**

*authors: Juan Luis Salas & Emilio Molina*

![](https://raw.githubusercontent.com/EliosMolina/ecosystem/documentation_fix46/doc/images/ecosystem_0.4_dev.gif)

Since version v0.3, the simulator is implemented in C++, so it is much faster than v0.2, which was implemented in Python.

It has two independent set of files:
- Ecosystem core, in folder `src/cpp` (the only dependency is `boost`):
    + `ecosystem.h` and `.cpp`
    + `ExperimentInterface.h` and `.cpp`
    + `main.cpp`
    + `json.hpp` (Third party: https://github.com/nlohmann/json)
- OpenGL based GUI implemented with JUCE (www.juce.com) in folder `src/cpp/gui_juce`:
    + `Main.cpp`
    + `MainComponent.h` and `.cpp`
    + `MapComponent.h` and `.cpp`
    + `ExperimentComponent.h` and `.cpp`

In addition, the initial prototype in python is included in folder `src/python`.


# How to run the OpenGL-based GUI?

The GUI has been developed using JUCE (www.juce.com), so it has to be installed in your system. A Projucer project is included in `src/cpp/gui_juce/gui_juce.jucer`. Two builds are included in this project:

* Linux makefile
* MacOSX XCode project

By default, JUCE is searched in `../../../../../JUCE` relative path. For example, JUCE may be installed in `~/JUCE` and the repository in `~/git/ecosystem`. If you want to modify this path, you can do it by opening `gui_jucer.jucer` in Projucer.

# How to compile a command-line version of ecosystem?

The core of the system does not depend on JUCE. Instead, the only dependency used is `boost` (http://www.boost.org/). In Mac OSx it can be installed with brew: `brew install boost`, which install the library in `/usr/local/Cellar/boost/1.63.0` (version can vary). Therefore, to compile the program run:

`g++ *.cpp -O3 --std=c++11 -I/usr/local/Cellar/boost/1.63.0/include -L/usr/local/Cellar/boost/1.63.0/lib -lboost_system -lboost_filesystem -lboost_iostreams -o ecosystem`

In Linux, boost might be installed already in your system, so try:

`g++ *.cpp --std=c++11 -lboost_system -lboost_filesystem -lboost_iostreams -o ecosystem`

Then you can run `./ecosystem`.
