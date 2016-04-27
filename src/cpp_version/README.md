# How to compile command-line version of ecosystem?

Just run:
`g++ main.cpp ecosystem.cpp --std=c++11 -o ecosystem`

It does not have dependencies, so it should compile very easily.

# How to run the GUI?

The GUI has been developed using JUCE (www.juce.com), so it has to be installed in your system. A Projucer project is included in `src/cpp_version/gui_juce`. Two builds are included in this project:

* Linux makefile
* MacOSX XCode project

By default, JUCE is searched in `../../../../../JUCE` relative path. For example, JUCE may be installed in `~/JUCE` and the repository in `~/git/ecosystem`. If you want to modify this path, you can do it by opening `gui_jucer.jucer` in Projucer.
