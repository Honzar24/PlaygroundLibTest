cmake_minimum_required(VERSION 3.17)

# set the project name
project(PlaygroundLibTest C CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(Executable ${PROJECT_NAME})

set(Sources "Playground.cpp")
set(CMAKE_FLAGS "-WALL")


add_subdirectory(Playground)


add_executable(${Executable} ${Sources})

target_link_libraries(${Executable} Playground)

#Testing
file(COPY res DESTINATION ${CMAKE_BINARY_DIR})
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/out)
