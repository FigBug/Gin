#pragma once

#include "parameter.h"

class GinProcessor;

//==============================================================================
class GinProgram
{
public:
    void loadProcessor (GinProcessor* p);
    void saveProcessor (GinProcessor* p);
    
    void loadFromFile (File f);
    void saveToDir (File f);
    void deleteFromDir (File f);
    
    String name;
    String valueTree;
    Array<Parameter::ParamState> states;
};
