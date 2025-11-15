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
class EquationParser::CallbackVF : public EquationParser::Callback
{
public:
    CallbackVF (std::function <double*(const char*)> f) : fun (f) {}
    std::function <double*(const char*)> fun;
};

//==============================================================================
static double modFunc(mu::SParam, double a, double b)
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
    impl->parser.DefineOprt ("%", modFunc, mu::prMUL_DIV, mu::oaLEFT, false);
}

EquationParser::EquationParser (const juce::String& equation)
{
    try
    {
        impl = std::make_unique<EquationParserImpl>();
        impl->parser.SetExpr (equation.toRawUTF8());

        impl->parser.DefineOprt ("%", modFunc, mu::prMUL_DIV, mu::oaLEFT, false);
    }
    catch (mu::Parser::exception_type& e)
    {
        errorMessage = juce::String (e.GetMsg());
    }
}

EquationParser::~EquationParser()
{
}

void EquationParser::defineNameChars (const juce::String& chars)
{
    impl->parser.DefineNameChars (chars.toRawUTF8());
}

void EquationParser::setEquation (const juce::String& equation)
{
    try
    {
        errorMessage = {};
        impl->parser.SetExpr (equation.toRawUTF8());
    }
    catch (mu::Parser::exception_type& e)
    {
        errorMessage = juce::String (e.GetMsg());
    }
}

void EquationParser::addVariable (const juce::String& name, double* value)
{
    try
    {
        impl->parser.DefineVar (name.toRawUTF8(), value);
    }
    catch (...)
    {
    }
}

void EquationParser::clearConstants()
{
    impl->parser.ClearConst();
}

void EquationParser::addConstant (const juce::String& name, double value)
{
    try
    {
        impl->parser.DefineConst (name.toRawUTF8(), value);
    }
    catch (...)
    {
    }
}

void EquationParser::clearVariables ()
{
    impl->parser.ClearVar();
}

void EquationParser::setVarFactory (std::function<double* (const char*)> fun)
{
    auto cb = new CallbackVF (fun);
    impl->parser.SetVarFactory ([] (mu::SParam, const char* name, void* data)
    {
        auto c = (CallbackVF*)data;
        auto r = c->fun (name);

        if (r == nullptr)
            throw mu::ParserError ("Variable buffer overflow.");

        return r;
    }, cb);
    varFactory.reset (cb);
}

juce::StringArray EquationParser::getUsedVariables ()
{
    juce::StringArray vars;

    try
    {
        errorMessage = {};

        for ( auto& itr : impl->parser.GetUsedVar () )
            vars.add ( itr.first );
    }
    catch (mu::Parser::exception_type& e)
    {
        errorMessage = juce::String (e.GetMsg());
    }

    return vars;
}

void EquationParser::addFunction (const juce::String& name, std::function<double (int id, const juce::String&)> fun)
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

void EquationParser::addFunction (const juce::String& name, std::function<double (int id)> fun)
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

void EquationParser::addFunction (const juce::String& name, std::function<double (int id, double)> fun)
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

void EquationParser::addFunction (const juce::String& name, std::function<double (int id, double, double)> fun)
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

void EquationParser::addFunction (const juce::String& name, std::function<double (int id, double, double, double)> fun)
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

void EquationParser::addFunction (const juce::String& name, std::function<double (int id, double, double, double, double)> fun)
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
        errorMessage = {};
        return impl->parser.Eval();
    }
    catch (mu::Parser::exception_type& e)
    {
        errorMessage = juce::String (e.GetMsg());
    }
    return 0;
}

bool EquationParser::hasError() const
{
    return errorMessage.isNotEmpty();
}

juce::String EquationParser::getError() const
{
    return errorMessage;
}

}

//==============================================================================
#if JUCE_UNIT_TESTS

class EquationParserTests : public juce::UnitTest
{
public:
    EquationParserTests() : juce::UnitTest ("Equation Parser", "gin") {}

    void runTest() override
    {
        testBasicArithmetic();
        testModuloOperator();
        testVariables();
        testConstants();
        testFunctions();
        testErrorHandling();
        testUsedVariables();
    }

private:
    void testBasicArithmetic()
    {
        beginTest ("Basic Arithmetic");

        EquationParser parser ("2 + 3");
        expectWithinAbsoluteError (parser.evaluate(), 5.0, 0.001, "Addition should work");

        parser.setEquation ("10 - 3");
        expectWithinAbsoluteError (parser.evaluate(), 7.0, 0.001, "Subtraction should work");

        parser.setEquation ("4 * 5");
        expectWithinAbsoluteError (parser.evaluate(), 20.0, 0.001, "Multiplication should work");

        parser.setEquation ("15 / 3");
        expectWithinAbsoluteError (parser.evaluate(), 5.0, 0.001, "Division should work");

        parser.setEquation ("2 + 3 * 4");
        expectWithinAbsoluteError (parser.evaluate(), 14.0, 0.001, "Order of operations should work");

        parser.setEquation ("(2 + 3) * 4");
        expectWithinAbsoluteError (parser.evaluate(), 20.0, 0.001, "Parentheses should work");
    }

    void testModuloOperator()
    {
        beginTest ("Modulo Operator");

        EquationParser parser ("10 % 3");
        expectWithinAbsoluteError (parser.evaluate(), 1.0, 0.001, "Modulo should work");

        parser.setEquation ("7.5 % 2.5");
        expectWithinAbsoluteError (parser.evaluate(), 0.0, 0.001, "Modulo with decimals should work");
    }

    void testVariables()
    {
        beginTest ("Variables");

        EquationParser parser;
        double x = 5.0;
        double y = 3.0;

        parser.addVariable ("x", &x);
        parser.addVariable ("y", &y);
        parser.setEquation ("x + y");

        expectWithinAbsoluteError (parser.evaluate(), 8.0, 0.001, "Variables should work");

        x = 10.0;
        expectWithinAbsoluteError (parser.evaluate(), 13.0, 0.001, "Changing variable values should work");

        parser.setEquation ("x * y + 2");
        expectWithinAbsoluteError (parser.evaluate(), 32.0, 0.001, "Complex expressions with variables should work");
    }

    void testConstants()
    {
        beginTest ("Constants");

        EquationParser parser;
        parser.addConstant ("pi", 3.14159265359);
        parser.setEquation ("2 * pi");

        expectWithinAbsoluteError (parser.evaluate(), 6.28318530718, 0.001, "Constants should work");

        parser.clearConstants();
        parser.setEquation ("2 * pi");
        expect (parser.hasError(), "Should error after clearing constants");
    }

    void testFunctions()
    {
        beginTest ("Functions");

        EquationParser parser;

        // Test function with 0 parameters
        parser.addFunction ("five", [] (int) { return 5.0; });
        parser.setEquation ("five()");
        expectWithinAbsoluteError (parser.evaluate(), 5.0, 0.001, "0-parameter function should work");

        // Test function with 1 parameter
        parser.addFunction ("double", [] (int, double x) { return x * 2.0; });
        parser.setEquation ("double(3)");
        expectWithinAbsoluteError (parser.evaluate(), 6.0, 0.001, "1-parameter function should work");

        // Test function with 2 parameters
        parser.addFunction ("add", [] (int, double x, double y) { return x + y; });
        parser.setEquation ("add(4, 5)");
        expectWithinAbsoluteError (parser.evaluate(), 9.0, 0.001, "2-parameter function should work");

        // Test function with 3 parameters
        parser.addFunction ("sum3", [] (int, double x, double y, double z) { return x + y + z; });
        parser.setEquation ("sum3(1, 2, 3)");
        expectWithinAbsoluteError (parser.evaluate(), 6.0, 0.001, "3-parameter function should work");

        // Test function with 4 parameters
        parser.addFunction ("sum4", [] (int, double a, double b, double c, double d) { return a + b + c + d; });
        parser.setEquation ("sum4(1, 2, 3, 4)");
        expectWithinAbsoluteError (parser.evaluate(), 10.0, 0.001, "4-parameter function should work");
    }

    void testErrorHandling()
    {
        beginTest ("Error Handling");

        EquationParser parser ("2 + * 3");
        expect (parser.hasError(), "Should detect syntax error");
        expect (parser.getError().isNotEmpty(), "Should have error message");

        parser.setEquation ("undefined_var + 5");
        parser.evaluate();
        expect (parser.hasError(), "Should detect undefined variable");

        parser.setEquation ("2 + 3");
        parser.evaluate();
        expect (!parser.hasError(), "Should clear error on valid equation");
    }

    void testUsedVariables()
    {
        beginTest ("Used Variables");

        EquationParser parser;
        double x = 1.0, y = 2.0, z = 3.0;

        parser.addVariable ("x", &x);
        parser.addVariable ("y", &y);
        parser.addVariable ("z", &z);

        parser.setEquation ("x + y");
        auto vars = parser.getUsedVariables();
        expect (vars.contains ("x"), "Should detect x is used");
        expect (vars.contains ("y"), "Should detect y is used");
        expect (!vars.contains ("z"), "Should detect z is not used");

        parser.setEquation ("x * y + z");
        vars = parser.getUsedVariables();
        expect (vars.contains ("x"), "Should detect x is used");
        expect (vars.contains ("y"), "Should detect y is used");
        expect (vars.contains ("z"), "Should detect z is used");
    }
};

static EquationParserTests equationParserTests;

#endif

