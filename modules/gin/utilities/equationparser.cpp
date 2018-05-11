/*==============================================================================

Copyright 2018 by Roland Rabien
For more information visit www.rabiensoftware.com

==============================================================================*/

#include "../3rdparty/muParser/muParser.h"
namespace gin {

class EquationParser::EquationParserImpl
{
public:
    mu::Parser parser;
};

EquationParser::EquationParser()
{
    impl = new EquationParserImpl();
}

EquationParser::EquationParser (juce::String equation)
{
    try
    {
        impl = new EquationParserImpl();
        impl->parser.SetExpr (equation.toRawUTF8());
    }
    catch (...)
    {
    }
}

EquationParser::~EquationParser()
{
    
}

void EquationParser::setEquation (juce::String equation)
{
    try
    {
        impl->parser.SetExpr (equation.toRawUTF8());
    }
    catch (...)
    {
    }
}

void EquationParser::addVariable (juce::String name, double* value)
{
    try
    {
        impl->parser.DefineVar (name.toRawUTF8(), value);
    }
    catch (...)
    {
    }
}

double EquationParser::evaluate()
{
    try
    {
        return impl->parser.Eval();
    }
    catch (...)
    {
    }
    return 0;
}
    
}
