#pragma once

#include "gin_parameter.h"

class Processor;

//==============================================================================
class Program
{
public:
    void loadProcessor (Processor* p);
    void saveProcessor (Processor* p);

    void loadFromFile (File f);
    void saveToDir (File f);
    void deleteFromDir (File f);

    String name;
    String valueTree;
    Array<Parameter::ParamState> states;
};
