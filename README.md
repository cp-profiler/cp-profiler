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


### The Protocol (high level)

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

### The Protocol (low level)

Each message starts with a four-byte integer encoding the size of the remainder of the message in bytes. This is followed by a single byte encofing the type of the message. The corresponding values are: **`Node`**: `0`, **`Done`**: `1`, **`Start`**: `2`, **`Restart`**: `3`.

#### `Node` message

In case the message is of the type **`Node`**, the following fields are added in order: `id`, `pid`, `alt`, `children` and `status`.

Node identifiers `id` and `pid` are represented using three four-byte integers: first identifies the identifier of the node within a thread, the second -- the identifier of the restart (in a restart-based search), and the third -- the identifier of the thread.
The `alt` and `children` fields are represented by a single four byte integer each.
The `status` field is represented by a single byte; its possible values are: *SOLVED*: 0, *FAILED*: 1, *BRANCH*: 2, *SKIPPED*: 3.
All multi-byte integer values are encoded using the *two's compliment* notation in the *big-endian order*.

Additionally, each node message can contain the following optional fields:
- `label`: branching decision (or any arbitrary string to be drawn together with the node);
- `nogood`: string representation of a newly generated nogood in a learning solver;
- `info`: arbitrary information about the node (*TODO*).

Field identifiers and their sizes in bytes:

| field name | field id | size (bytes) |
|:----------:|:--------:|:------------:|
|    `id`    |    n/a   |      12      |
|    `pid`   |    n/a   |      12      |
|    `alt`   |    n/a   |       4      |
| `children` |    n/a   |       4      |
|  `status`  |    n/a   |       1      |
|   `label`  |     0    |      any     |
|  `nogood`  |     1    |      any     |
|   `info`   |     2    |      any     |

**Example**. The following is a possible correspondence between a solver and the profiler that generates the simple tree above.The order in which different fields arrive is shown from top to bottom (rows are numbered for convenience).

*Message 1:*

| Row | Bytes                                                                              | Interpretation                |
|-----|------------------------------------------------------------------------------------|-------------------------------|
| 1   | `00 00 00 21`                                                                      | message size (33)             |
| 2   | `02`                                                                               | message type (*START*)        |
| 3   | `02`                                                                               | field (*info*)                |
| 4   | `00 00 00 1B`                                                                      | string size (27)              |
| 5   | `7b 22 6e 61 6d 65 22 3a 20 22 6d 69 6e 69 6d 61 6c 20 65 78 61 6d 70 6c 65 22 7d` | '{"name": "minimal example"}' |

*Message 2:*

| Row | Bytes         | Interpretation          |
|-----|---------------|-------------------------|
| 6   | `00 00 00 2B` | message size (43)       |
| 7   | `00`          | message type (**NODE**) |
| 8   | `00 00 00 00` | node id (0)             |
| 9   | `FF FF FF FF` | node restart id (-1)    |
| 10  | `FF FF FF FF` | node thread id (-1)     |
| 11  | `FF FF FF FF` | parent id (-1)          |
| 12  | `FF FF FF FF` | parent restart id (-1)  |
| 13  | `FF FF FF FF` | parent thread id (-1)   |
| 14  | `FF FF FF FF` | alternative (-1)        |
| 15  | `00 00 00 02` | children (2)            |
| 16  | `02`          | status (*BRANCH*)       |
| 17  | `00`          | field (label)           |
| 18  | `00 00 00 04` | string size (4)         |
| 19  | `52 6f 6f 74` | 'Root'                  |

*Message 3:*

| Row | Bytes         | Interpretation          |
|-----|---------------|-------------------------|
| 20  | `00 00 00 01` | message size (1)        |
| 21  | `01`          | message type (**DONE**) |