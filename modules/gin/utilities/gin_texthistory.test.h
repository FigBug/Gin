/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class TextHistoryTests : public juce::UnitTest
{
public:
    TextHistoryTests() : juce::UnitTest ("Text History", "gin") {}

    void runTest() override
    {
        testBasicUndoRedo();
        testMultipleEdits();
        testBranchingHistory();
        testHistoryLimit();
        testDuplicateText();
        testStateChanged();
    }

private:
    void testBasicUndoRedo()
    {
        beginTest ("Basic Undo/Redo");

        TextHistory history;

        expect (!history.canUndo(), "Should not be able to undo initially");
        expect (!history.canRedo(), "Should not be able to redo initially");

        history.addText ("Hello");
        expect (history.getCurrentText() == "Hello", "Text should be 'Hello'");
        expect (history.canUndo(), "Should be able to undo");
        expect (!history.canRedo(), "Should not be able to redo");

        history.addText ("Hello World");
        expect (history.getCurrentText() == "Hello World", "Text should be 'Hello World'");

        history.undo();
        expect (history.getCurrentText() == "Hello", "After undo should be 'Hello'");
        expect (history.canRedo(), "Should be able to redo");

        history.redo();
        expect (history.getCurrentText() == "Hello World", "After redo should be 'Hello World'");
    }

    void testMultipleEdits()
    {
        beginTest ("Multiple Edits");

        TextHistory history;

        history.addText ("One");
        history.addText ("Two");
        history.addText ("Three");
        history.addText ("Four");

        expect (history.getCurrentText() == "Four", "Should be at 'Four'");

        history.undo();
        expect (history.getCurrentText() == "Three", "Should be 'Three'");

        history.undo();
        expect (history.getCurrentText() == "Two", "Should be 'Two'");

        history.undo();
        expect (history.getCurrentText() == "One", "Should be 'One'");

        expect (history.canUndo(), "Should still be able to undo");

        history.undo();
        expect (history.getCurrentText() == "", "Should be empty");
        expect (!history.canUndo(), "Should not be able to undo further");

        // Redo all the way forward
        history.redo();
        history.redo();
        history.redo();
        history.redo();

        expect (history.getCurrentText() == "Four", "Should be back at 'Four'");
        expect (!history.canRedo(), "Should not be able to redo further");
    }

    void testBranchingHistory()
    {
        beginTest ("Branching History");

        TextHistory history;

        history.addText ("A");
        history.addText ("B");
        history.addText ("C");

        // Undo twice
        history.undo();
        history.undo();

        expect (history.getCurrentText() == "A", "Should be at 'A'");

        // Create a new branch
        history.addText ("X");

        expect (!history.canRedo(), "After branching, redo should be unavailable");
        expect (history.getCurrentText() == "X", "Should be at 'X'");

        history.undo();
        expect (history.getCurrentText() == "A", "Should be back at 'A'");
    }

    void testHistoryLimit()
    {
        beginTest ("History Limit");

        TextHistory history;
        history.setHistoryLimit (3);

        history.addText ("1");
        history.addText ("2");
        history.addText ("3");
        history.addText ("4");
        history.addText ("5");

        // Should only remember last 3 changes
        expect (history.getCurrentText() == "5", "Should be at '5'");

        history.undo();
        expect (history.getCurrentText() == "4", "Should be at '4'");

        history.undo();
        expect (history.getCurrentText() == "3", "Should be at '3'");

        history.undo();
        expect (history.getCurrentText() == "2", "Should be at '2'");

        // Should not be able to undo further (limit reached)
        expect (!history.canUndo(), "Should not be able to undo past limit");
    }

    void testDuplicateText()
    {
        beginTest ("Duplicate Text");

        TextHistory history;

        history.addText ("Same");
        bool added1 = history.addText ("Same");

        expect (!added1, "Adding duplicate text should return false");
        expect (history.getCurrentText() == "Same", "Text should remain 'Same'");
        expect (!history.canUndo(), "Should not have created undo point");
    }

    void testStateChanged()
    {
        beginTest ("State Changed Callback");

        TextHistory history;
        int callbackCount = 0;

        history.onStateChanged = [&callbackCount]() { callbackCount++; };

        history.addText ("Test");
        expect (callbackCount == 1, "Callback should be called once");

        history.addText ("Test"); // Duplicate
        expect (callbackCount == 1, "Callback should not be called for duplicate");

        history.addText ("Changed");
        expect (callbackCount == 2, "Callback should be called again");

        history.undo();
        expect (callbackCount == 3, "Callback should be called on undo");

        history.redo();
        expect (callbackCount == 4, "Callback should be called on redo");
    }
};

static TextHistoryTests textHistoryTests;

#endif
