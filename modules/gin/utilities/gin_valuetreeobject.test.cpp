/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if JUCE_UNIT_TESTS

// Test classes for ValueTreeObject testing
class TestChildObject : public ValueTreeObject
{
public:
    TestChildObject (const juce::ValueTree& vt) : ValueTreeObject (vt) {}
};

class TestParentObject : public ValueTreeObject
{
public:
    TestParentObject (const juce::ValueTree& vt) : ValueTreeObject (vt) {}
};

class ValueTreeObjectTests : public juce::UnitTest
{
public:
    ValueTreeObjectTests() : juce::UnitTest ("ValueTreeObject", "gin") {}

    void runTest() override
    {
        testBasicConstruction();
        testChildManagement();
        testChildAddition();
        testChildRemoval();
        testChildReordering();
        testFindChildrenOfClass();
        testFindParentOfType();
    }

private:
    void setupFactory()
    {
        ValueTreeObject::factory = [] (const juce::Identifier& type, const juce::ValueTree& vt) -> ValueTreeObject*
        {
            if (type == juce::Identifier ("Parent"))
                return new TestParentObject (vt);
            else if (type == juce::Identifier ("Child"))
                return new TestChildObject (vt);
            return nullptr;
        };
    }

    void testBasicConstruction()
    {
        beginTest ("ValueTreeObject - Basic Construction");

        setupFactory();

        juce::ValueTree tree ("Parent");
        TestParentObject obj (tree);

        expect (obj.getState() == tree, "Should wrap the ValueTree");
        expectEquals (obj.getChildren().size(), 0, "Should have no children initially");
    }

    void testChildManagement()
    {
        beginTest ("ValueTreeObject - Child Management");

        setupFactory();

        juce::ValueTree parent ("Parent");
        juce::ValueTree child1 ("Child");
        juce::ValueTree child2 ("Child");

        parent.appendChild (child1, nullptr);
        parent.appendChild (child2, nullptr);

        TestParentObject obj (parent);

        expectEquals (obj.getChildren().size(), 2,
                     "Should create objects for existing children");

        auto& children = obj.getChildren();
        expect (dynamic_cast<TestChildObject*> (children[0]) != nullptr,
               "First child should be TestChildObject");
        expect (dynamic_cast<TestChildObject*> (children[1]) != nullptr,
               "Second child should be TestChildObject");
    }

    void testChildAddition()
    {
        beginTest ("ValueTreeObject - Child Addition");

        setupFactory();

        juce::ValueTree parent ("Parent");
        TestParentObject obj (parent);

        expectEquals (obj.getChildren().size(), 0, "Should start with no children");

        // Add a child to the ValueTree
        juce::ValueTree child ("Child");
        parent.appendChild (child, nullptr);

        expectEquals (obj.getChildren().size(), 1,
                     "Should automatically create child object");

        auto* childObj = dynamic_cast<TestChildObject*> (obj.getChildren()[0]);
        expect (childObj != nullptr, "Child should be of correct type");
        expect (childObj->getState() == child, "Child should wrap correct ValueTree");
    }

    void testChildRemoval()
    {
        beginTest ("ValueTreeObject - Child Removal");

        setupFactory();

        juce::ValueTree parent ("Parent");
        juce::ValueTree child ("Child");
        parent.appendChild (child, nullptr);

        TestParentObject obj (parent);
        expectEquals (obj.getChildren().size(), 1, "Should have one child");

        // Remove child from ValueTree
        parent.removeChild (child, nullptr);

        expectEquals (obj.getChildren().size(), 0,
                     "Should automatically remove child object");
    }

    void testChildReordering()
    {
        beginTest ("ValueTreeObject - Child Reordering");

        setupFactory();

        juce::ValueTree parent ("Parent");
        juce::ValueTree child1 ("Child");
        juce::ValueTree child2 ("Child");

        child1.setProperty ("id", 1, nullptr);
        child2.setProperty ("id", 2, nullptr);

        parent.appendChild (child1, nullptr);
        parent.appendChild (child2, nullptr);

        TestParentObject obj (parent);

        // Check initial order
        expectEquals ((int) obj.getChildren()[0]->getState().getProperty ("id"), 1,
                     "First child should be child1");
        expectEquals ((int) obj.getChildren()[1]->getState().getProperty ("id"), 2,
                     "Second child should be child2");

        // Move child2 to position 0
        parent.moveChild (1, 0, nullptr);

        expectEquals ((int) obj.getChildren()[0]->getState().getProperty ("id"), 2,
                     "First child should now be child2");
        expectEquals ((int) obj.getChildren()[1]->getState().getProperty ("id"), 1,
                     "Second child should now be child1");
    }

    void testFindChildrenOfClass()
    {
        beginTest ("ValueTreeObject - Find Children Of Class");

        setupFactory();

        juce::ValueTree parent ("Parent");
        juce::ValueTree child1 ("Child");
        juce::ValueTree child2 ("Child");
        juce::ValueTree child3 ("Child");

        parent.appendChild (child1, nullptr);
        parent.appendChild (child2, nullptr);
        parent.appendChild (child3, nullptr);

        TestParentObject obj (parent);

        auto childObjects = obj.findChildrenOfClass<TestChildObject>();
        expectEquals (childObjects.size(), 3,
                     "Should find all TestChildObject children");

        auto parentObjects = obj.findChildrenOfClass<TestParentObject>();
        expectEquals (parentObjects.size(), 0,
                     "Should not find TestParentObject children");
    }

    void testFindParentOfType()
    {
        beginTest ("ValueTreeObject - Find Parent Of Type");

        setupFactory();

        juce::ValueTree grandparent ("Parent");
        juce::ValueTree parent ("Parent");
        juce::ValueTree child ("Child");

        parent.appendChild (child, nullptr);
        grandparent.appendChild (parent, nullptr);

        TestParentObject gpObj (grandparent);

        // Get the child object (grandparent -> parent -> child)
        auto* parentObj = gpObj.getChildren()[0];
        auto* childObj = parentObj->getChildren()[0];

        auto* foundParent = childObj->findParentOfType<TestParentObject>();
        expect (foundParent != nullptr, "Should find parent of type TestParentObject");
        expect (foundParent == parentObj, "Should find immediate parent");
    }
};

static ValueTreeObjectTests valueTreeObjectTests;

#endif
