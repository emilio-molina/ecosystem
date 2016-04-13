# ecosystem
Virtual ecosystem of bugs eating each other. It consists of two files:

- ./src/ecosystem.py: Run an ecosystem and stores results in a history folder
- ./src/gui.py: Read a history folder a draw the ecosystem

It also includes a test experiment to test the GUI in:

- ./histories/test 

# How to run a new experiment?
Just run:
```
$ python src/ecosystem.py histories/new_experiment
```

# How to visualize an experiment?
```
$ python src/gui.py histories/test  # test experiment already included
```
