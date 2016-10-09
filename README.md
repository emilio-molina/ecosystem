# ecosystem v0.3

**Individual-based ecosystem simulation.**

*authors: Juan Luis Salas & Emilio Molina*

Note: By the moment it is very simple. Only three kind of organisms are considered: predator, prey and grass. 

![](https://raw.githubusercontent.com/EliosMolina/ecosystem/master/ecosystem_terrain.gif)

v0.3 is implemented in C++, so it is much faster than v0.2, which was implemented in Python.

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

The GUI has been developed using JUCE (www.juce.com), so it has to be installed in your system. A Projucer project is included in `src/cpp_version/gui_juce`. Two builds are included in this project:

* Linux makefile
* MacOSX XCode project

By default, JUCE is searched in `../../../../../JUCE` relative path. For example, JUCE may be installed in `~/JUCE` and the repository in `~/git/ecosystem`. If you want to modify this path, you can do it by opening `gui_jucer.jucer` in Projucer.

# How to compile a command-line version of ecosystem?

The core of the system does not depend on JUCE. Instead, the only dependency used is `boost` (http://www.boost.org/). In Mac OSx it can be installed with brew: `brew install boost`, which install the library in `/usr/local/Cellar/boost/1.57.0`. Therefore, to compile the program run:

`g++ main.cpp ecosystem.cpp --std=c++11 -I/usr/local/Cellar/boost/1.57.0/include -L/usr/local/Cellar/boost/1.57.0/lib -lboost_system -lboost_filesystem -lboost_iostreams -o ecosystem`

In Linux, boost might be installed already in your system, so try:

`g++ main.cpp ecosystem.cpp --std=c++11 -lboost_system -lboost_filesystem -lboost_iostreams -o ecosystem`

Then you can run `./ecosystem`. By the moment, our `main.cpp` is too simple, so you can not access to the data of this ecosystem, but it is a good starting point to keep developing a proper `main.cpp` for a CLI ecosystem. In the future, this main will be extended with more functionalities to be fully usable.
