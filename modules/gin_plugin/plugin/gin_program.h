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

    void loadProcessor (Processor& p);
    void saveProcessor (Processor& p);

    juce::File getPresetFile (juce::File programDir);
    void loadFromFile (juce::File f, bool loadFully);
    void saveToDir (juce::File programDir);
    void deleteFromDir (juce::File programDir);

    juce::File file;
    juce::String name;
    juce::String author;
    juce::StringArray tags;

    bool fullyLoaded = false;
    juce::ValueTree state;

    juce::Array<Parameter::ParamState> states;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Program)
};
