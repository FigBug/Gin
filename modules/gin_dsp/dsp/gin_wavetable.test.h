//==============================================================================
#if GIN_UNIT_TESTS

class WavetableTests : public juce::UnitTest
{
public:
    WavetableTests() : juce::UnitTest ("Wavetable", "gin_dsp") {}

    void runTest() override
    {
        testWavetableCreation();
        testAddingTables();
        testTableAccess();
        testTableCount();
        testArrayOperator();
        testGetUnchecked();
        testClear();
        testMoveConstructor();
        testMoveAssignment();
        testProcessBoundsChecking();
    }

private:
    // Helper to create a simple lookup table for testing
    gin::BandLimitedLookupTable* createSimpleTable()
    {
        // Create a sine wave function
        auto sineFunction = [] (float phase, float, float) -> float {
            return std::sin (phase * 2.0f * juce::MathConstants<float>::pi);
        };

        // Create table with sine wave at 44100 Hz
        return new gin::BandLimitedLookupTable (sineFunction, 44100.0f);
    }

    void testWavetableCreation()
    {
        beginTest ("Wavetable Creation");

        gin::Wavetable wavetable;

        expect (wavetable.getNumTables() == 0, "New wavetable should be empty");
        expect (wavetable.size() == 0, "Size should be 0");
    }

    void testAddingTables()
    {
        beginTest ("Adding Tables");

        gin::Wavetable wavetable;

        wavetable.addTable (createSimpleTable());
        expect (wavetable.getNumTables() == 1, "Should have 1 table");

        wavetable.addTable (createSimpleTable());
        expect (wavetable.getNumTables() == 2, "Should have 2 tables");

        wavetable.addTable (createSimpleTable());
        expect (wavetable.getNumTables() == 3, "Should have 3 tables");
    }

    void testTableAccess()
    {
        beginTest ("Table Access");

        gin::Wavetable wavetable;

        wavetable.addTable (createSimpleTable());
        wavetable.addTable (createSimpleTable());

        auto* table0 = wavetable.getTable (0);
        expect (table0 != nullptr, "Table 0 should exist");

        auto* table1 = wavetable.getTable (1);
        expect (table1 != nullptr, "Table 1 should exist");

        expect (table0 != table1, "Tables should be different objects");
    }

    void testTableCount()
    {
        beginTest ("Table Count");

        gin::Wavetable wavetable;

        expect (wavetable.getNumTables() == 0, "Should start with 0 tables");
        expect (wavetable.size() == 0, "Size should be 0");

        for (int i = 0; i < 5; ++i)
            wavetable.addTable (createSimpleTable());

        expect (wavetable.getNumTables() == 5, "Should have 5 tables");
        expect (wavetable.size() == 5, "Size should be 5");
    }

    void testArrayOperator()
    {
        beginTest ("Array Operator");

        gin::Wavetable wavetable;

        wavetable.addTable (createSimpleTable());
        wavetable.addTable (createSimpleTable());
        wavetable.addTable (createSimpleTable());

        // Test array subscript operator
        auto* table0 = wavetable[0];
        expect (table0 != nullptr, "Table [0] should exist");

        auto* table1 = wavetable[1];
        expect (table1 != nullptr, "Table [1] should exist");

        auto* table2 = wavetable[2];
        expect (table2 != nullptr, "Table [2] should exist");
    }

    void testGetUnchecked()
    {
        beginTest ("Get Unchecked");

        gin::Wavetable wavetable;

        wavetable.addTable (createSimpleTable());
        wavetable.addTable (createSimpleTable());

        // Test unchecked access
        auto* table0 = wavetable.getUnchecked (0);
        expect (table0 != nullptr, "Unchecked access to table 0 should work");

        auto* table1 = wavetable.getUnchecked (1);
        expect (table1 != nullptr, "Unchecked access to table 1 should work");
    }

    void testClear()
    {
        beginTest ("Clear");

        gin::Wavetable wavetable;

        wavetable.addTable (createSimpleTable());
        wavetable.addTable (createSimpleTable());
        wavetable.addTable (createSimpleTable());

        expect (wavetable.getNumTables() == 3, "Should have 3 tables");

        wavetable.clear();

        expect (wavetable.getNumTables() == 0, "Should have 0 tables after clear");
        expect (wavetable.size() == 0, "Size should be 0 after clear");
    }

    void testMoveConstructor()
    {
        beginTest ("Move Constructor");

        gin::Wavetable wavetable1;
        wavetable1.addTable (createSimpleTable());
        wavetable1.addTable (createSimpleTable());

        expect (wavetable1.getNumTables() == 2, "Original should have 2 tables");

        // Move construct
        gin::Wavetable wavetable2 (std::move (wavetable1));

        expect (wavetable2.getNumTables() == 2, "Moved wavetable should have 2 tables");
    }

    void testMoveAssignment()
    {
        beginTest ("Move Assignment");

        gin::Wavetable wavetable1;
        wavetable1.addTable (createSimpleTable());
        wavetable1.addTable (createSimpleTable());
        wavetable1.addTable (createSimpleTable());

        gin::Wavetable wavetable2;
        wavetable2 = std::move (wavetable1);

        expect (wavetable2.getNumTables() == 3, "Move-assigned wavetable should have 3 tables");
    }

    void testProcessBoundsChecking()
    {
        beginTest ("Process Bounds Checking");

        gin::Wavetable wavetable;
        wavetable.addTable (createSimpleTable());

        // Valid index should return non-zero value for sine wave
        float value1 = wavetable.process (0, 60.0f, 0.25f);
        expect (std::abs (value1) > 0.5f, "Should return non-zero value for valid index");

        // Invalid index (negative) should return 0
        float value2 = wavetable.process (-1, 60.0f, 0.25f);
        expectWithinAbsoluteError (value2, 0.0f, 0.001f, "Should return 0 for negative index");

        // Invalid index (too large) should return 0
        float value3 = wavetable.process (10, 60.0f, 0.25f);
        expectWithinAbsoluteError (value3, 0.0f, 0.001f, "Should return 0 for out-of-bounds index");

        // Test processLinear with bounds checking
        float value4 = wavetable.processLinear (0, 60.0f, 0.25f);
        expect (std::abs (value4) > 0.5f, "ProcessLinear should return non-zero for valid index");

        float value5 = wavetable.processLinear (-1, 60.0f, 0.25f);
        expectWithinAbsoluteError (value5, 0.0f, 0.001f, "ProcessLinear should return 0 for negative index");

        float value6 = wavetable.processLinear (10, 60.0f, 0.25f);
        expectWithinAbsoluteError (value6, 0.0f, 0.001f, "ProcessLinear should return 0 for out-of-bounds index");
    }
};

static WavetableTests wavetableTests;

#endif
