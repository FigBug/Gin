#pragma once

#include "gin_parameter.h"

class Processor;

//==============================================================================
/** Saves your plugin state to XML
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
