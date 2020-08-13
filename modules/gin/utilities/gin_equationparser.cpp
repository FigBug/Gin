/*==============================================================================

Copyright 2018 by Roland Rabien
For more information visit www.rabiensoftware.com

==============================================================================*/

#if __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wswitch-enum"
 #pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

#include "../3rdparty/muParser/muParser.h"

#if __clang__
 #pragma clang diagnostic pop
#endif

namespace gin {

//==============================================================================
class EquationParser::CallbackS : public EquationParser::Callback
{
public:
    CallbackS (std::function <double(int, const juce::String&)> f) : fun (f) {}
    std::function <double(int, const juce::String&)> fun;
};
class EquationParser::Callback0 : public EquationParser::Callback
{
public:
    Callback0 (std::function <double(int)> f) : fun (f) {}
    std::function <double(int)> fun;
};
class EquationParser::Callback1 : public EquationParser::Callback
{
public:
    Callback1 (std::function <double(int, double)> f) : fun (f) {}
    std::function <double(int, double)> fun;
};
class EquationParser::Callback2 : public EquationParser::Callback
{
public:
    Callback2 (std::function <double(int, double, double)> f) : fun (f) {}
    std::function <double(int, double, double)> fun;
};
class EquationParser::Callback3 : public EquationParser::Callback
{
public:
    Callback3 (std::function <double(int, double, double, double)> f) : fun (f) {}
    std::function <double(int, double, double, double)> fun;
};
class EquationParser::Callback4 : public EquationParser::Callback
{
public:
    Callback4 (std::function <double(int, double, double, double, double)> f) : fun (f) {}
    std::function <double(int, double, double, double, double)> fun;
};

//==============================================================================
double modFunc(mu::SParam, double a, double b)
{
    return std::fmod (a, b);
}

//==============================================================================
class EquationParser::EquationParserImpl
{
public:
    mu::Parser parser;
};

//==============================================================================
EquationParser::EquationParser()
{
    impl = std::make_unique<EquationParserImpl>();
}

EquationParser::EquationParser (juce::String equation)
{
    try
    {
        impl = std::make_unique<EquationParserImpl>();
        impl->parser.SetExpr (equation.toRawUTF8());

        impl->parser.DefineOprt ("%", modFunc, mu::prMUL_DIV, mu::oaLEFT, false);
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
    catch (mu::Parser::exception_type& e)
    {
        errorMessage = juce::String (e.GetMsg());
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

void EquationParser::addConstant (juce::String name, double value)
{
    try
    {
        impl->parser.DefineConst (name.toRawUTF8(), value);
    }
    catch (...)
    {
    }
}

void EquationParser::addFunction (juce::String name, std::function<double (int id, const juce::String&)> fun)
{
    try
    {
        auto cb = new CallbackS (fun);
        impl->parser.DefineFun (name.toRawUTF8(), [] (mu::SParam s, const char* p1)
                                {
                                    auto c = (CallbackS*)s.param;
                                    juce::String text = p1;
                                    return c->fun (s.id, text);
                                }, cb, false);
        callbacks.add (cb);
    }
    catch (...)
    {
    }
}

void EquationParser::addFunction (juce::String name, std::function<double (int id)> fun)
{
    try
    {
        auto cb = new Callback0 (fun);
        impl->parser.DefineFun (name.toRawUTF8(), [] (mu::SParam s)
                                {
                                    auto c = (Callback0*)s.param;
                                    return c->fun (s.id);
                                }, cb, false);
        callbacks.add (cb);
    }
    catch (...)
    {
    }
}

void EquationParser::addFunction (juce::String name, std::function<double (int id, double)> fun)
{
    try
    {
        auto cb = new Callback1 (fun);
        impl->parser.DefineFun (name.toRawUTF8(), [] (mu::SParam s, double p1)
                                {
                                    auto c = (Callback1*)s.param;
                                    return c->fun (s.id, p1);
                                }, cb, false);
        callbacks.add (cb);
    }
    catch (...)
    {
    }
}

void EquationParser::addFunction (juce::String name, std::function<double (int id, double, double)> fun)
{
    try
    {
        auto cb = new Callback2 (fun);
        impl->parser.DefineFun (name.toRawUTF8(), [] (mu::SParam s, double p1, double p2)
                                {
                                    auto c = (Callback2*)s.param;
                                    return c->fun (s.id, p1, p2);
                                }, cb, false);
        callbacks.add (cb);
    }
    catch (...)
    {
    }
}

void EquationParser::addFunction (juce::String name, std::function<double (int id, double, double, double)> fun)
{
    try
    {
        auto cb = new Callback3 (fun);
        impl->parser.DefineFun (name.toRawUTF8(), [] (mu::SParam s, double p1, double p2, double p3)
                                {
                                    auto c = (Callback3*)s.param;
                                    return c->fun (s.id, p1, p2, p3);
                                }, cb, false);
        callbacks.add (cb);
    }
    catch (...)
    {
    }
}

void EquationParser::addFunction (juce::String name, std::function<double (int id, double, double, double, double)> fun)
{
    try
    {
        auto cb = new Callback4 (fun);
        impl->parser.DefineFun (name.toRawUTF8(), [] (mu::SParam s, double p1, double p2, double p3, double p4)
                                {
                                    auto c = (Callback4*)s.param;
                                    return c->fun (s.id, p1, p2, p3, p4);
                                }, cb, false);
        callbacks.add (cb);
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
    catch (mu::Parser::exception_type& e)
    {
        errorMessage = juce::String (e.GetMsg());
    }
    return 0;
}

bool EquationParser::hasError()
{
    return errorMessage.isNotEmpty();
}

juce::String EquationParser::getError()
{
    return errorMessage;
}

}
