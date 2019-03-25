# ecosystem v0.4\_dev

**Individual-based ecosystem simulation.**

*authors: Juan Luis Salas & Emilio Molina*

![](https://raw.githubusercontent.com/EliosMolina/ecosystem/master/doc/images/ecosystem_0.4_dev.gif)

- Ecosystem core, in folder `src/cpp` (the only dependency is `boost`):
    + `ecosystem.h` and `.cpp`
    + `ExperimentInterface.h` and `.cpp`
    + `main.cpp`
    + `json.hpp` (Third party: https://github.com/nlohmann/json)
- Django web-app to control the core: experiments running, visualization, etc.

# How to compile a command-line version of ecosystem?

The only dependency used is `boost` (http://www.boost.org/). In Mac OSx it can be installed with brew: `brew install boost`, which install the library in `/usr/local/Cellar/boost/1.63.0` (version can vary). Therefore, to compile the program run:

`g++ *.cpp -O3 --std=c++11 -I/usr/local/Cellar/boost/1.63.0/include -L/usr/local/Cellar/boost/1.63.0/lib -lboost_system -lboost_filesystem -lboost_iostreams -o ecosystem`

In Linux, boost might be installed already in your system, so try:

`g++ *.cpp --std=c++11 -lboost_system -lboost_filesystem -lboost_iostreams -o ecosystem`

Then you can run `./ecosystem`.
