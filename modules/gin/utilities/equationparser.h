/*==============================================================================

Copyright 2018 by Roland Rabien
For more information visit www.rabiensoftware.com

==============================================================================*/

#pragma once

class EquationParser
{
public:
    EquationParser();
    EquationParser (juce::String equation);
    ~EquationParser();
    
    void setEquation (juce::String equation);
    void addVariable (juce::String name, double* value);
    
    void addConstant (juce::String name, double value);
    
    void addFunction (juce::String name, double (*fun)());
    void addFunction (juce::String name, double (*fun)(double));
    void addFunction (juce::String name, double (*fun)(double, double));
    void addFunction (juce::String name, double (*fun)(double, double, double));

    double evaluate();
    
private:
    class EquationParserImpl;
    ScopedPointer<EquationParserImpl> impl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EquationParser)
};
