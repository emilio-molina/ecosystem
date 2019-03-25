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

The only dependency used is `boost` (http://www.boost.org/). In Mac OSx it can be installed with brew: `brew install boost`.

You can generate a Xcode project with cmake:

```
$ mkdir build
$ cd build
$ cmake -G Xcode ..  # in linux $ cmake ..; make
```

It will create a Xcode project. Binaries will be placed in ./bin directory.
