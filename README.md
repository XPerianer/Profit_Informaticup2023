[![Format](https://github.com/XPerianer/Profit_Informaticup2023/actions/workflows/clang-format.yml/badge.svg)](https://github.com/XPerianer/Profit_Informaticup2023/actions/workflows/clang-format.yml)
[![Clang-Tidy](https://github.com/XPerianer/Profit_Informaticup2023/actions/workflows/clang-tidy.yml/badge.svg)](https://github.com/XPerianer/Profit_Informaticup2023/actions/workflows/clang-tidy.yml)
[![Tests](https://github.com/XPerianer/Profit_Informaticup2023/actions/workflows/test.yml/badge.svg)](https://github.com/XPerianer/Profit_Informaticup2023/actions/workflows/test.yml)

# Solution of Team uwunauten for the Informaticup 2023
This is the repository for the solution of the Team uwunauten for the Informaticup 2023 hosted by the [German Society of Computer Science](https://gi.de). You can find more information about the competition [here](https://informaticup.github.io/).

# Background for Task "Profit!"
This years task was called "Profit!". The task was to place different components on a 2D-Grid to connect deposits with factories to enable the production of products. Solutions were scored by the products they were able to produce in a number of fixed timesteps of the simulation.
More information can be found in the [offical task description](https://github.com/informatiCup/informatiCup2023/blob/main/informatiCup%202023%20-%20Profit!.pdf).

# Solution
Our main idea was to use split the task in different parts, and then combine these solutions into the global solution.
At the beginning, we analyse the map for different parts that are not connected and can therefore be solved by themselves in a Divide and Conquer fashion.
For each of the parts, we model the selection of products as a Knapsack problem and applied a greedy solving strategy.
The connection between deposits and factories is done by a modified Dijkstra-Algorithm to prevent self-intersections. 
To evaluate the score of given connections, we modeled the available resources as linear functions, which allows us to estimate the score within constant time, without the need for step-wise simulation.
To allow the fast combination and testing of the different partial solutions we implemented our solution in modern C++20 with execution speed as a priority. More information is found in our [documentation (german)](https://github.com/XPerianer/Profit_Informaticup2023/blob/main/Ausarbeitung.pdf).

# Usage

## Docker
The easiest way to run our solution is using Docker with the provided Dockerfile.
You can build the docker container using 
```
docker build --tag DOCKER_TAG .
```
and run it using 
```
docker run -i DOCKER_TAG < tasks/001.task.json
```
This prints the solution on the command line.

## Build yourself
Our project uses CMake as a build system.
We support to build options: `RELEASE` and `DEBUG`. You can configure them e.g., with [ccmake](https://cmake.org/cmake/help/latest/manual/ccmake.1.html).

To generate Makefiles:
``` 
mkdir build && cmake build 
```

To build, run make from the build folder
``` 
make 
```
We use STDIO for input and output. An example call would be:
```
./main < ../tasks/001.task.json > output.json
```

## Development
Tests can be run with ctest:
``` ctest ```

