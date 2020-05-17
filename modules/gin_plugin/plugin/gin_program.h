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

    void loadFromFile (File f);
    void saveToDir (File f);
    void deleteFromDir (File f);

    String name;
    String author;
    StringArray tags;
    String valueTree;
    Array<Parameter::ParamState> states;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Program)
};
