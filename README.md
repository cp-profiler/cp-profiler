# CP-Profiler

CP-Profiler provides search tree visualisations for executions of constraint programming solvers.

## Table of Contents

- [Building](#building)
- [Usage](#usage)

### Building

Dependencies:

- Qt5.x

```
    git submodule update --init
    mkdir build && cd build
    qmake .. && make
```

### Usage

1. Starting CP-Profiler

    `cp-profiler.app/Contents/MacOS/cp-profiler` (Mac)

This starts a local TCP server listening on one of the available ports (6565 by default).

2. Executing a supported solver

The solver must implement the profiling protocol (TODO). Integration libraries are available if you wish to extend your solver to work with CP-Profiler.