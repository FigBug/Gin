cmake_minimum_required(VERSION 3.15)
set_property(GLOBAL PROPERTY USE_FOLDERS YES)
set(JUCE_ENABLE_MODULE_SOURCE_GROUPS ON CACHE BOOL "" FORCE)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

project(Gin VERSION 1.0.0)

if (APPLE)
	execute_process (COMMAND xcodebuild -version OUTPUT_VARIABLE xcodebuild_version_output ERROR_QUIET)
	if (xcodebuild_version_output MATCHES "Xcode ([0-9]+)" AND CMAKE_MATCH_1 GREATER_EQUAL 27)
		set (CMAKE_OSX_DEPLOYMENT_TARGET 12.0)
	else()
		set (CMAKE_OSX_DEPLOYMENT_TARGET 10.13)
	endif()
endif()

add_subdirectory(juce)

add_subdirectory(modules)
add_subdirectory(examples/Demo)
add_subdirectory(examples/Effect)
add_subdirectory(examples/Synth)
add_subdirectory(examples/UnitTests)
