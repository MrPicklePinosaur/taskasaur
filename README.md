# Taskasaur

A very simple kanban style todolist with vim keybindings.

This project is practically a clone of [taskell](https://github.com/smallhadroncollider/taskell), but written in c, since the haskell dependencies that were pulled in were a bit too heavy for my liking.

Taskasaur is also meant to be extended with patches, allowing the main project to remain slim. Simply edit the config.h file to your liking and recompile.

## Installation

To build from source, simply clone the repo and run
```
make && sudo make clean install
```

A gentoo ebuild with savedconfig use may or may not be coming in the future.
