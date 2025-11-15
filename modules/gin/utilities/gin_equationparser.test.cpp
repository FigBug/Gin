//==============================================================================
#if GIN_UNIT_TESTS

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
        parser.evaluate();
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
        parser.evaluate();
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
