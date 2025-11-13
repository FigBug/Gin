/*==============================================================================

Copyright 2018 by Roland Rabien
For more information visit www.rabiensoftware.com

==============================================================================*/

#pragma once

/**
    Mathematical equation parser and evaluator with variable and function support.

    EquationParser provides a flexible system for parsing and evaluating mathematical
    expressions at runtime. It supports variables, constants, custom functions, and
    standard mathematical operations. Built on top of muParser library.

    Key Features:
    - Parse and evaluate mathematical expressions
    - Dynamic variable binding (pointers to live values)
    - Named constants
    - Custom functions with 0-4 parameters or string parameters
    - Variable factory for on-demand variable creation
    - Comprehensive error handling
    - Query which variables are used in an equation

    Supported Operations:
    - Basic arithmetic: +, -, *, /, ^
    - Parentheses for grouping
    - Standard math functions (via custom function registration)
    - User-defined functions and variables

    Usage:
    @code
    EquationParser parser;

    // Add variables
    double x = 5.0;
    parser.addVariable("x", &x);

    // Add constants
    parser.addConstant("pi", 3.14159265359);

    // Set equation
    parser.setEquation("2 * x + pi");

    // Evaluate
    double result = parser.evaluate(); // Returns 13.14159...

    // Change variable and re-evaluate
    x = 10.0;
    result = parser.evaluate(); // Returns 23.14159...

    // Check for errors
    if (parser.hasError())
        DBG("Error: " + parser.getError());
    @endcode

    Custom Functions:
    @code
    parser.addFunction("myFunc", [](int id, double a, double b) {
        return a * a + b;
    });
    parser.setEquation("myFunc(3, 4)");
    double result = parser.evaluate(); // Returns 13.0
    @endcode

    @see muParser documentation for expression syntax details
*/
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
    juce::StringArray getUsedVariables ();

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
