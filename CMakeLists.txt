cmake_minimum_required(VERSION 3.15)
set_property(GLOBAL PROPERTY USE_FOLDERS YES)
set(JUCE_ENABLE_MODULE_SOURCE_GROUPS ON CACHE BOOL "" FORCE)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

project(Gin VERSION 1.0.0)

set (CMAKE_OSX_DEPLOYMENT_TARGET 10.9)

add_subdirectory(juce)

add_subdirectory(modules)
add_subdirectory(examples/Demo)
add_subdirectory(examples/Effect)
add_subdirectory(examples/Synth)
