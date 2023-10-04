/*==============================================================================

Copyright 2018 by Roland Rabien
For more information visit www.rabiensoftware.com

==============================================================================*/

#pragma once

class EquationParser
{
public:
    EquationParser();
    EquationParser (const juce::String& equation);
    ~EquationParser();

    void defineNameChars (const juce::String& chars);

    void setEquation (const juce::String& equation);

    void clearVariables ();
    void addVariable (const juce::String& name, double* value);
    void setVarFactory (std::function<double* (const char*)> fun);

    void clearConstants();
    void addConstant (const juce::String& name, double value);

    void addFunction (const juce::String& name, std::function<double (int id, const juce::String&)> fun);

    void addFunction (const juce::String& name, std::function<double (int id)> fun);
    void addFunction (const juce::String& name, std::function<double (int id, double)> fun);
    void addFunction (const juce::String& name, std::function<double (int id, double, double)> fun);
    void addFunction (const juce::String& name, std::function<double (int id, double, double, double)> fun);
    void addFunction (const juce::String& name, std::function<double (int id, double, double, double, double)> fun);

    double evaluate();

    bool hasError();
    juce::String getError();

private:
    class Callback
    {
    public:
        virtual ~Callback() = default;
    };
    class CallbackS;
    class Callback0;
    class Callback1;
    class Callback2;
    class Callback3;
    class Callback4;
    class CallbackVF;

    juce::OwnedArray<Callback> callbacks;

    class EquationParserImpl;
    std::unique_ptr<EquationParserImpl> impl;
    std::unique_ptr<CallbackVF> varFactory;
    juce::String errorMessage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EquationParser)
};
