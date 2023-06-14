cmake_minimum_required(VERSION 3.15)

project(Gin VERSION 1.0.0)

set (CMAKE_OSX_DEPLOYMENT_TARGET 10.9)

add_subdirectory(juce)

add_subdirectory(modules)
add_subdirectory(examples/Demo)
add_subdirectory(examples/Synth)
