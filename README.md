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



#### Similar Subtree Analysis

This analysis allows users to find similarities in the search tree.

**Elimination of subsumed patterns**

A pattern `P` is said to be subsumed by one or more patterns if subtrees of those patterns
contain all of the subtrees of `P` as descendants.

**Applying filters.**

Should filtered out patterns be allowed to subsume?


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



#### Similar Subtree Analysis

This analysis allows users to find similarities in the search tree.

**Elimination of subsumed patterns**

A pattern `P` is said to be subsumed by one or more patterns if subtrees of those patterns
contain all of the subtrees of `P` as descendants.

**Applying filters.**

Should filtered out patterns be allowed to subsume?


### The Protocol

The following describes the protocol that a solver must implement to communicate with the profiler.

The protocol distinguishes between the following types of messages: **`Start`**, **`Restart`**, **`Done`**, and **`Node`**.



The **`Start`** message is sent at the beginning of the execution (**TODO**: multithreaded search). The message has two optional (?) parameters:

- `Name`: execution's descriptor to be presented to the user (e.g. the model's name)
- `Execution ID`: globally unique identifier used to distinguish between different executions.

**TODO**: any other parameters?

The **`Restart`** message is sent whenever a solver performs a restart in case of a restart-based search.

The **`Done`** message is sent to the profiler at the end of the execution to indicate that no further nodes should be expected.

The **`Node`** message is sent whenever a new node is explored by the solver and contains information necessary for reconstructing the search tree. The required parameters are:

- `Node ID`: unique node identifier in the execution.
- `Parent ID`: the identifier (`Node ID`) of the parent node. A root node can have an identifier of `-1`. (**TODO**: needs checking)
- `Alternative`: the node's position relative to its siblings; for the left-most child it is `0`, for the second left-most it is `1` etc.
- `Number of Children`: the number of children nodes. If not known, can be set to `0` and the node will be extended with extra children later on if necessary. It is, however, advisable to specify the number of children the profiler should expect (for example, the yet to arrive nodes can be visualised to give a better idea about the search).
- `Status`: allows to distinguish between different types of nodes. Supported values are:
     - *BRANCH*: internal node in the tree;
     - *SOLUTION*: leaf node representing a solution;
     - *FAILURE*: leaf node representing a failure;
     - *SKIPPED*: leaf node representing unexplored search space due to backjumping.

**Example**. The following sequence of nodes (excluding the `Start` and `Done` messages) produces the simple tree below:

|  `Label` | `Node ID` | `Parent ID` | `Alternative` | `Number of Children` | `Status` |
|:--------:|:---------:|:-----------:|:-------------:|:--------------------:|:--------:|
|   Root   |     0     |      -1     |       -1      |           2          |  BRANCH  |
|  Failure |     1     |      0      |       0       |           0          |  FAILED  |
| Solution |     2     |      0      |       1       |           0          |  SOLVED  |

![A simple search tree](todo)