/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class PointTests : public juce::UnitTest
{
public:
    PointTests() : juce::UnitTest ("Point", "gin") {}

    void runTest() override
    {
        testDefaultConstruction();
        testConstruction();
        testJuceConversion();
    }

private:
    void testDefaultConstruction()
    {
        beginTest ("Default Construction Is Origin");

        gin::Point<float> pf;
        expectWithinAbsoluteError (pf.getX(), 0.0f, 1.0e-9f, "Default x is 0");
        expectWithinAbsoluteError (pf.getY(), 0.0f, 1.0e-9f, "Default y is 0");

        gin::Point<int> pi;
        expectEquals (pi.getX(), 0, "Default int x is 0");
        expectEquals (pi.getY(), 0, "Default int y is 0");
    }

    void testConstruction()
    {
        beginTest ("Construction With Coordinates");

        gin::Point<int> p (3, -7);
        expectEquals (p.getX(), 3, "x coordinate");
        expectEquals (p.getY(), -7, "y coordinate");
        expectEquals (p.x, 3, "public x member");
        expectEquals (p.y, -7, "public y member");
    }

    void testJuceConversion()
    {
        beginTest ("Conversion To And From juce::Point");

       #if JUCE_MODULE_AVAILABLE_juce_graphics
        juce::Point<float> jp (1.5f, 2.5f);
        gin::Point<float> gp (jp);
        expectWithinAbsoluteError (gp.getX(), 1.5f, 1.0e-6f, "x from juce::Point");
        expectWithinAbsoluteError (gp.getY(), 2.5f, 1.0e-6f, "y from juce::Point");

        juce::Point<float> back = gp.toJuce();
        expectWithinAbsoluteError (back.x, 1.5f, 1.0e-6f, "toJuce x");
        expectWithinAbsoluteError (back.y, 2.5f, 1.0e-6f, "toJuce y");
       #endif
    }
};

static PointTests pointTests;

#endif
