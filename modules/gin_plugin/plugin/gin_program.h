#pragma once

#include "gin_parameter.h"

class Processor;

//==============================================================================
/** Save your plugin state
*/
class Program
{
public:
    Program() = default;
    
    void loadProcessor (Processor& p);
    void saveProcessor (Processor& p);

    void loadFromFile (juce::File f);
    void saveToDir (juce::File f);
    void deleteFromDir (juce::File f);

    juce::String name;
    juce::String author;
    juce::StringArray tags;
    juce::String valueTree;
    juce::Array<Parameter::ParamState> states;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Program)
};
