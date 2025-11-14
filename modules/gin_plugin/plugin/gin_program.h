#pragma once

#include "gin_parameter.h"

class Processor;

//==============================================================================
/**
    Plugin preset/program with file-based storage and parameter management.

    Program represents a single preset that stores all parameter values and
    optionally custom plugin state. Programs can be saved to/loaded from XML
    files and include metadata like name, author, and tags.

    Key Features:
    - Parameter state storage and recall
    - Custom plugin state via ValueTree
    - File-based preset management
    - Metadata: name, author, tags
    - Lazy loading (load metadata first, full state on demand)
    - XML-based storage format

    Programs are typically managed by the Processor class, which handles
    automatic loading from preset directories and file watching for changes.

    Usage:
    @code
    // Creating and saving a program
    Program program;
    program.name = "My Preset";
    program.author = "Username";
    program.tags.add("Bass");
    program.tags.add("Dark");
    program.saveProcessor(myProcessor);
    program.saveToDir(presetDirectory);

    // Loading a program
    Program loadedProgram;
    loadedProgram.loadFromFile(presetFile, true); // true = load fully
    loadedProgram.loadProcessor(myProcessor);
    @endcode

    @see Processor, Parameter
*/
class Program
{
public:
    Program() = default;
    virtual ~Program() = default;

    virtual void loadProcessor (Processor& p);
    virtual void saveProcessor (Processor& p);

    virtual juce::File getPresetFile (juce::File programDir);
    virtual void loadFromFile (juce::File f, bool loadFully);
    virtual void saveToDir (juce::File programDir);
    virtual void deleteFromDir (juce::File programDir);

    juce::File file;
    juce::String name;
    juce::String author;
    juce::StringArray tags;

    bool fullyLoaded = false;
    juce::ValueTree state;

    juce::Array<Parameter::ParamState> parameters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Program)
};
