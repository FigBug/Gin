//==============================================================================
#if GIN_UNIT_TESTS

class ComponentMapTests : public juce::UnitTest
{
public:
    ComponentMapTests() : juce::UnitTest ("ComponentMap", "gin")
    {
    }

    void runTest() override
    {
        const juce::MessageManagerLock mmLock;
        runAllTests();
    }

private:
    void runAllTests()
    {
        testBasicMapCreation();
        testFindComponent();
        testGetComponentPath();
        testPathCaseInsensitivity();
        testAddComponent();
        testClearMap();
        testDump();
        testNestedHierarchy();
        testComponentsWithoutIDs();
        testCreateMapWithPredicate();
        testLayoutIDPriority();
        testComponentIDPriority();
        testNameFallback();
        testFindRootComponent();
        testMultipleLevelNesting();
        testDynamicallyAddedComponents();
    }

    // Helper component class for testing
    class TestParent : public juce::Component
    {
    public:
        TestParent()
        {
            setComponentID ("root");

            child1.setComponentID ("child1");
            addAndMakeVisible (child1);

            child2.setComponentID ("child2");
            addAndMakeVisible (child2);

            child3.setComponentID ("child3");
            addAndMakeVisible (child3);

            // Nested children
            grandchild1.setComponentID ("grandchild1");
            child1.addAndMakeVisible (grandchild1);

            grandchild2.setComponentID ("grandchild2");
            child1.addAndMakeVisible (grandchild2);
        }

        juce::Component child1;
        juce::Component child2;
        juce::Component child3;
        juce::Component grandchild1;
        juce::Component grandchild2;
    };

    void testBasicMapCreation()
    {
        beginTest ("Basic Map Creation");

        TestParent parent;
        ComponentMap map (parent);

        map.createMap();

        // Verify we can find the children
        auto* found1 = map.findComponent ("/child1");
        auto* found2 = map.findComponent ("/child2");
        auto* found3 = map.findComponent ("/child3");

        expect (found1 == &parent.child1, "Should find child1");
        expect (found2 == &parent.child2, "Should find child2");
        expect (found3 == &parent.child3, "Should find child3");
    }

    void testFindComponent()
    {
        beginTest ("Find Component");

        TestParent parent;
        ComponentMap map (parent);
        map.createMap();

        // Test finding direct children
        auto* child = map.findComponent ("/child1");
        expect (child != nullptr, "Should find child1");
        expect (child == &parent.child1, "Should find correct child1 reference");

        // Test finding nested children
        auto* grandchild = map.findComponent ("/child1/grandchild1");
        expect (grandchild != nullptr, "Should find grandchild1");
        expect (grandchild == &parent.grandchild1, "Should find correct grandchild1 reference");

        // Test non-existent path
        auto* notFound = map.findComponent ("/nonexistent");
        expect (notFound == nullptr, "Should return nullptr for non-existent path");
    }

    void testGetComponentPath()
    {
        beginTest ("Get Component Path");

        TestParent parent;
        ComponentMap map (parent);
        map.createMap();

        // Test direct child paths
        auto path1 = map.getComponentPath (parent.child1);
        expectEquals (path1, juce::String ("/child1"), "Path for child1 should be /child1");

        auto path2 = map.getComponentPath (parent.child2);
        expectEquals (path2, juce::String ("/child2"), "Path for child2 should be /child2");

        // Test nested child paths
        auto path3 = map.getComponentPath (parent.grandchild1);
        expectEquals (path3, juce::String ("/child1/grandchild1"), "Path for grandchild1 should be /child1/grandchild1");

        auto path4 = map.getComponentPath (parent.grandchild2);
        expectEquals (path4, juce::String ("/child1/grandchild2"), "Path for grandchild2 should be /child1/grandchild2");
    }

    void testPathCaseInsensitivity()
    {
        beginTest ("Path Case Insensitivity");

        TestParent parent;
        ComponentMap map (parent);
        map.createMap();

        // Test various case combinations
        auto* found1 = map.findComponent ("/CHILD1");
        auto* found2 = map.findComponent ("/Child1");
        auto* found3 = map.findComponent ("/cHiLd1");

        expect (found1 == &parent.child1, "Should find child1 with uppercase");
        expect (found2 == &parent.child1, "Should find child1 with mixed case");
        expect (found3 == &parent.child1, "Should find child1 with weird case");

        // Test nested paths are case-insensitive
        auto* nested = map.findComponent ("/CHILD1/GRANDCHILD1");
        expect (nested == &parent.grandchild1, "Should find grandchild1 with uppercase path");
    }

    void testAddComponent()
    {
        beginTest ("Add Component");

        TestParent parent;
        ComponentMap map (parent);

        // Add components manually instead of using createMap
        map.addComponent (parent.child1);
        map.addComponent (parent.child2);

        auto* found1 = map.findComponent ("/child1");
        auto* found2 = map.findComponent ("/child2");

        expect (found1 == &parent.child1, "Should find manually added child1");
        expect (found2 == &parent.child2, "Should find manually added child2");

        // Child3 was not added, should not be found
        auto* found3 = map.findComponent ("/child3");
        expect (found3 == nullptr, "Should not find child3 that was not added");
    }

    void testClearMap()
    {
        beginTest ("Clear Map");

        TestParent parent;
        ComponentMap map (parent);
        map.createMap();

        // Verify components are in the map
        auto* found = map.findComponent ("/child1");
        expect (found != nullptr, "Should find child1 before clear");

        // Clear the map
        map.clearMap();

        // Verify components are no longer found
        found = map.findComponent ("/child1");
        expect (found == nullptr, "Should not find child1 after clear");

        found = map.findComponent ("/child2");
        expect (found == nullptr, "Should not find child2 after clear");
    }

    void testDump()
    {
        beginTest ("Dump");

        TestParent parent;
        ComponentMap map (parent);
        map.createMap();

        auto paths = map.dump();

        // Should have all components in the dump
        expect (paths.size() > 0, "Dump should contain paths");
        expect (paths.contains ("/child1"), "Should contain /child1");
        expect (paths.contains ("/child2"), "Should contain /child2");
        expect (paths.contains ("/child3"), "Should contain /child3");
        expect (paths.contains ("/child1/grandchild1"), "Should contain /child1/grandchild1");
        expect (paths.contains ("/child1/grandchild2"), "Should contain /child1/grandchild2");

        // Paths should be sorted
        for (int i = 1; i < paths.size(); i++)
        {
            expect (paths[i - 1].compareIgnoreCase (paths[i]) <= 0, "Paths should be sorted");
        }
    }

    void testNestedHierarchy()
    {
        beginTest ("Nested Hierarchy");

        TestParent parent;
        ComponentMap map (parent);
        map.createMap();

        // Test finding components at different nesting levels
        auto* level1 = map.findComponent ("/child1");
        expect (level1 != nullptr, "Should find level 1 component");

        auto* level2 = map.findComponent ("/child1/grandchild1");
        expect (level2 != nullptr, "Should find level 2 component");

        // Verify the parent-child relationship is maintained in paths
        auto path = map.getComponentPath (parent.grandchild1);
        expect (path.contains ("child1"), "Grandchild path should contain parent ID");
        expect (path.contains ("grandchild1"), "Grandchild path should contain own ID");
    }

    void testComponentsWithoutIDs()
    {
        beginTest ("Components Without IDs");

        juce::Component parent;
        juce::Component child1;
        juce::Component child2;

        child2.setComponentID ("child2");

        parent.addAndMakeVisible (child1);  // No ID
        parent.addAndMakeVisible (child2);  // Has ID

        ComponentMap map (parent);
        map.createMap();

        // Child1 has no ID, should not be in map
        auto paths = map.dump();
        expect (! paths.contains ("/child1"), "Should not contain component without ID");
        expect (paths.contains ("/child2"), "Should contain component with ID");

        // Try to add component without ID manually
        map.addComponent (child1);
        paths = map.dump();
        expect (! paths.contains ("/child1"), "Should still not contain component without ID after manual add");
    }

    void testCreateMapWithPredicate()
    {
        beginTest ("Create Map With Predicate");

        TestParent parent;
        ComponentMap map (parent);

        // Create map but skip child1's children
        map.createMap ([&parent] (juce::Component& c) -> bool
        {
            return &c != &parent.child1;  // Don't traverse into child1
        });

        // Child1 itself should be in the map
        auto* found1 = map.findComponent ("/child1");
        expect (found1 != nullptr, "Should find child1");

        // But child1's children should not be in the map
        auto* notFound = map.findComponent ("/child1/grandchild1");
        expect (notFound == nullptr, "Should not find grandchild1 when predicate blocks traversal");

        // Other children should still be found
        auto* found2 = map.findComponent ("/child2");
        expect (found2 != nullptr, "Should find child2");
    }

    void testLayoutIDPriority()
    {
        beginTest ("LayoutID Priority");

        juce::Component parent;
        juce::Component child;

        child.setComponentID ("componentID");
        child.setName ("componentName");
        child.getProperties().set ("layoutID", "layoutID");

        parent.addAndMakeVisible (child);

        ComponentMap map (parent);
        map.createMap();

        // layoutID should take precedence
        auto path = map.getComponentPath (child);
        expect (path.contains ("layoutid"), "Should use layoutID (case-insensitive)");

        auto* found = map.findComponent ("/layoutID");
        expect (found == &child, "Should find component by layoutID");
    }

    void testComponentIDPriority()
    {
        beginTest ("ComponentID Priority");

        juce::Component parent;
        juce::Component child;

        child.setComponentID ("componentID");
        child.setName ("componentName");
        // No layoutID property set

        parent.addAndMakeVisible (child);

        ComponentMap map (parent);
        map.createMap();

        // componentID should be used when layoutID is not set
        auto path = map.getComponentPath (child);
        expect (path.contains ("componentid"), "Should use componentID (case-insensitive)");

        auto* found = map.findComponent ("/componentID");
        expect (found == &child, "Should find component by componentID");
    }

    void testNameFallback()
    {
        beginTest ("Name Fallback");

        juce::Component parent;
        juce::Component child;

        child.setName ("componentName");
        // No componentID or layoutID set

        parent.addAndMakeVisible (child);

        ComponentMap map (parent);
        map.createMap();

        // Name should be used as fallback
        auto path = map.getComponentPath (child);
        expect (path.contains ("componentname"), "Should use name as fallback (case-insensitive)");

        auto* found = map.findComponent ("/componentName");
        expect (found == &child, "Should find component by name");
    }

    void testFindRootComponent()
    {
        beginTest ("Find Root Component");

        TestParent parent;
        ComponentMap map (parent);
        map.createMap();

        // Empty string should return the root component
        auto* root = map.findComponent ("");
        expect (root == &parent, "Empty path should return parent component");
    }

    void testMultipleLevelNesting()
    {
        beginTest ("Multiple Level Nesting");

        juce::Component root;
        juce::Component level1;
        juce::Component level2;
        juce::Component level3;

        level1.setComponentID ("level1");
        level2.setComponentID ("level2");
        level3.setComponentID ("level3");

        root.addAndMakeVisible (level1);
        level1.addAndMakeVisible (level2);
        level2.addAndMakeVisible (level3);

        ComponentMap map (root);
        map.createMap();

        // Test deep nesting
        auto* found = map.findComponent ("/level1/level2/level3");
        expect (found == &level3, "Should find deeply nested component");

        auto path = map.getComponentPath (level3);
        expectEquals (path, juce::String ("/level1/level2/level3"), "Path should show full hierarchy");

        // Test intermediate levels
        auto* mid = map.findComponent ("/level1/level2");
        expect (mid == &level2, "Should find intermediate level component");
    }

    void testDynamicallyAddedComponents()
    {
        beginTest ("Dynamically Added Components");

        juce::Component parent;
        juce::Component child1;
        child1.setComponentID ("child1");
        parent.addAndMakeVisible (child1);

        ComponentMap map (parent);
        map.createMap();

        // Verify child1 is in the map
        auto* found = map.findComponent ("/child1");
        expect (found != nullptr, "Should find child1 initially");

        // Add a new component dynamically
        juce::Component child2;
        child2.setComponentID ("child2");
        parent.addAndMakeVisible (child2);

        // Child2 should not be in map yet
        auto* notYetFound = map.findComponent ("/child2");
        expect (notYetFound == nullptr, "Should not find child2 before adding to map");

        // Add child2 to the map
        map.addComponent (child2);

        // Now child2 should be found
        auto* nowFound = map.findComponent ("/child2");
        expect (nowFound == &child2, "Should find child2 after adding to map");

        // Original child1 should still be there
        found = map.findComponent ("/child1");
        expect (found == &child1, "Should still find child1");
    }
};

static ComponentMapTests componentMapTests;

#endif
