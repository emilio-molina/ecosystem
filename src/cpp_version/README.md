# How to compile command-line version of ecosystem?

The only dependency used is `boost` (http://www.boost.org/). In Mac OSx it can be installed with brew: `brew install boost`, which install the library in `/usr/local/Cellar/boost/1.57.0`. Therefore, to compile the program run:

`g++ main.cpp ecosystem.cpp --std=c++11 -I/usr/local/Cellar/boost/1.57.0/include -L/usr/local/Cellar/boost/1.57.0/lib -lboost_system -lboost_filesystem -o ecosystem`

# How to run the GUI?

The GUI has been developed using JUCE (www.juce.com), so it has to be installed in your system. A Projucer project is included in `src/cpp_version/gui_juce`. Two builds are included in this project:

* Linux makefile
* MacOSX XCode project

By default, JUCE is searched in `../../../../../JUCE` relative path. For example, JUCE may be installed in `~/JUCE` and the repository in `~/git/ecosystem`. If you want to modify this path, you can do it by opening `gui_jucer.jucer` in Projucer.
