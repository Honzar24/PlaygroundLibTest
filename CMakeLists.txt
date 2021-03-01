cmake_minimum_required(VERSION 3.17)
project(PlaygroundLibTest)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#Compiler flags
add_subdirectory(Playground)
set(Sources "Playground.cpp")
#create executable
set(Executable ${PROJECT_NAME})
add_executable(${Executable} ${Sources})
target_link_libraries(${Executable} Playground)
#Testing
file(COPY res DESTINATION ${CMAKE_BINARY_DIR})
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/out)
