FROM ubuntu:22.04

RUN apt update && apt install cmake g++-12 -y
ENV CXX g++-12

COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./dependencies ./dependencies
COPY ./include ./include
COPY ./src ./src
COPY ./tests ./tests

RUN cmake .
RUN cmake --build . --target main --config Release

CMD [ "./main" ]
