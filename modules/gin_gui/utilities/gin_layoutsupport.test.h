//==============================================================================
#if GIN_UNIT_TESTS

class LayoutSupportTests : public juce::UnitTest
{
public:
    LayoutSupportTests() : juce::UnitTest ("LayoutSupport", "gin")
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

        testBasicLayout();
        testSetLayoutFromString();
        testIsLayoutSet();
        testSetConstant();
        testAbsolutePositioning();
        testRelativePositioning();
        testCenterPositioning();
        testEdgePositioning();
        testBoundsSpecialModes();
        testSizeProperties();
        testExpressions();
        testConstants();
        testMacros();
        testGridLayout();
        testNestedComponents();
        testMultipleIds();
        testRangeSyntax();
        testConditionalComponents();
        testVisibility();
        testAccessibilityProperties();
        testCustomProperties();
        testLayoutChangedCallback();
        testComponentFactory();
    }

    // Helper component class
    class TestComponent : public juce::Component
    {
    public:
        TestComponent()
        {
            setSize (800, 600);

            for (int i = 0; i < 10; i++)
            {
                auto* button = buttons.add (new juce::TextButton());
                button->setComponentID ("button" + juce::String (i));
                addAndMakeVisible (button);
            }

            container.setComponentID ("container");
            addAndMakeVisible (container);

            header.setComponentID ("header");
            addAndMakeVisible (header);

            content.setComponentID ("content");
            addAndMakeVisible (content);

            footer.setComponentID ("footer");
            addAndMakeVisible (footer);
        }

        juce::OwnedArray<juce::TextButton> buttons;
        juce::Component container;
        juce::Component header;
        juce::Component content;
        juce::Component footer;
    };

    void testBasicLayout()
    {
        beginTest ("Basic Layout");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "button0", "x": 10, "y": 20, "w": 100, "h": 30 }
            ]
        })json";

        layout.setLayout (json);

        expect (layout.isLayoutSet(), "Layout should be set");
        expectEquals (comp.buttons[0]->getX(), 10, "X position should be 10");
        expectEquals (comp.buttons[0]->getY(), 20, "Y position should be 20");
        expectEquals (comp.buttons[0]->getWidth(), 100, "Width should be 100");
        expectEquals (comp.buttons[0]->getHeight(), 30, "Height should be 30");
    }

    void testSetLayoutFromString()
    {
        beginTest ("Set Layout From String");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "button0", "x": 5, "y": 10, "w": 50, "h": 25 }
            ]
        })json";

        layout.setLayout (json);

        expect (layout.isLayoutSet(), "Layout should be set");
        expectEquals (comp.buttons[0]->getX(), 5);
        expectEquals (comp.buttons[0]->getY(), 10);
    }

    void testIsLayoutSet()
    {
        beginTest ("Is Layout Set");

        TestComponent comp;
        LayoutSupport layout (comp);

        expect (! layout.isLayoutSet(), "Layout should not be set initially");

        juce::String json = R"json({ "components": [] })json";
        layout.setLayout (json);

        expect (layout.isLayoutSet(), "Layout should be set after setLayout");
    }

    void testSetConstant()
    {
        beginTest ("Set Constant");

        TestComponent comp;
        LayoutSupport layout (comp);

        layout.setConstant ("customMargin", 15);

        juce::String json = R"json({
            "components": [
                { "id": "button0", "x": "customMargin", "y": "customMargin", "w": 100, "h": 30 }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (comp.buttons[0]->getX(), 15, "Should use custom constant");
        expectEquals (comp.buttons[0]->getY(), 15, "Should use custom constant");
    }

    void testAbsolutePositioning()
    {
        beginTest ("Absolute Positioning");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "button0", "x": 10, "y": 20, "w": 100, "h": 30 },
                { "id": "button1", "xy": "50,60", "w": 80, "h": 25 }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (comp.buttons[0]->getX(), 10);
        expectEquals (comp.buttons[0]->getY(), 20);
        expectEquals (comp.buttons[1]->getX(), 50);
        expectEquals (comp.buttons[1]->getY(), 60);
    }

    void testRelativePositioning()
    {
        beginTest ("Relative Positioning");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "button0", "x": 10, "y": 10, "w": 100, "h": 30 },
                { "id": "button1", "xd": 10, "y": 10, "w": 100, "h": 30 },
                { "id": "button2", "x": 10, "yd": 10, "w": 100, "h": 30 }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (comp.buttons[0]->getX(), 10);
        expectEquals (comp.buttons[1]->getX(), 120, "Should be positioned 10px after previous right edge");
        expectEquals (comp.buttons[2]->getY(), 50, "Should be positioned 10px below previous bottom edge");
    }

    void testCenterPositioning()
    {
        beginTest ("Center Positioning");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "button0", "cx": "parW / 2", "cy": "parH / 2", "w": 100, "h": 30 }
            ]
        })json";

        layout.setLayout (json);

        int expectedX = 800 / 2 - 100 / 2;
        int expectedY = 600 / 2 - 30 / 2;

        expectEquals (comp.buttons[0]->getX(), expectedX, "Should be centered horizontally");
        expectEquals (comp.buttons[0]->getY(), expectedY, "Should be centered vertically");
    }

    void testEdgePositioning()
    {
        beginTest ("Edge Positioning");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "button0", "r": "parW - 10", "y": 10, "w": 100, "h": 30 },
                { "id": "button1", "x": 10, "b": "parH - 10", "w": 100, "h": 30 },
                { "id": "button2", "x": 10, "r": "parW - 10", "y": 10, "h": 30 }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (comp.buttons[0]->getRight(), 790, "Right edge should be 10px from parent right");
        expectEquals (comp.buttons[1]->getBottom(), 590, "Bottom edge should be 10px from parent bottom");
        expectEquals (comp.buttons[2]->getWidth(), 780, "Width should be calculated from x and r");
    }

    void testBoundsSpecialModes()
    {
        beginTest ("Bounds Special Modes");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "container", "bounds": "parent" },
                { "id": "button0", "x": 10, "y": 10, "w": 100, "h": 30 },
                { "id": "button1", "bounds": "prev" },
                { "id": "button2", "bounds": "reduced,20" },
                { "id": "button3", "bounds": "10,20,100,50" }
            ]
        })json";

        layout.setLayout (json);

        expect (comp.container.getBounds() == juce::Rectangle<int> (0, 0, 800, 600), "Should fill parent");
        expect (comp.buttons[1]->getBounds() == comp.buttons[0]->getBounds(), "Should match previous");
        expect (comp.buttons[2]->getBounds() == juce::Rectangle<int> (20, 20, 760, 560), "Should be reduced by 20");
        expect (comp.buttons[3]->getBounds() == juce::Rectangle<int> (10, 20, 100, 50), "Should use explicit bounds");
    }

    void testSizeProperties()
    {
        beginTest ("Size Properties");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "constants": [
                { "buttonSize": { "w": 120, "h": 35 } }
            ],
            "components": [
                { "id": "button0", "x": 10, "y": 10, "w": 100, "h": 30 },
                { "id": "button1", "x": 10, "y": 50, "size": "buttonSize" }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (comp.buttons[0]->getWidth(), 100);
        expectEquals (comp.buttons[0]->getHeight(), 30);
        expectEquals (comp.buttons[1]->getWidth(), 120, "Should use size constant width");
        expectEquals (comp.buttons[1]->getHeight(), 35, "Should use size constant height");
    }

    void testExpressions()
    {
        beginTest ("Expressions");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "constants": [
                { "margin": 10 }
            ],
            "components": [
                { "id": "button0", "x": "margin * 2", "y": "margin + 5", "w": "parW - margin * 2", "h": 30 }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (comp.buttons[0]->getX(), 20, "Expression should evaluate to 20");
        expectEquals (comp.buttons[0]->getY(), 15, "Expression should evaluate to 15");
        expectEquals (comp.buttons[0]->getWidth(), 780, "Expression should evaluate to 780");
    }

    void testConstants()
    {
        beginTest ("Constants");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "constants": [
                { "margin": 10 },
                { "buttonHeight": 30 }
            ],
            "components": [
                { "id": "button0", "x": "margin", "y": "margin", "w": 100, "h": "buttonHeight" }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (comp.buttons[0]->getX(), 10, "Should use margin constant");
        expectEquals (comp.buttons[0]->getHeight(), 30, "Should use buttonHeight constant");
    }

    void testMacros()
    {
        beginTest ("Macros");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "macros": [
                { "id": "standardButton", "w": 100, "h": 30, "tip": "P1" }
            ],
            "components": [
                { "id": "button0", "macro": "standardButton('Test tooltip')", "x": 10, "y": 10 }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (comp.buttons[0]->getWidth(), 100, "Should use macro width");
        expectEquals (comp.buttons[0]->getHeight(), 30, "Should use macro height");
    }

    void testGridLayout()
    {
        beginTest ("Grid Layout");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                {
                    "grid": "button0,button1,button2,button3",
                    "bounds": "0,0,400,200",
                    "cols": 2,
                    "rows": 2,
                    "colGap": 10,
                    "rowGap": 10
                }
            ]
        })json";

        layout.setLayout (json);

        // Grid should position components in 2x2 layout with gaps
        expect (comp.buttons[0]->getX() >= 0 && comp.buttons[0]->getX() < 200, "Button 0 should be in left column");
        expect (comp.buttons[1]->getX() >= 200, "Button 1 should be in right column");
        expect (comp.buttons[0]->getY() >= 0 && comp.buttons[0]->getY() < 100, "Button 0 should be in top row");
        expect (comp.buttons[2]->getY() >= 100, "Button 2 should be in bottom row");
    }

    void testNestedComponents()
    {
        beginTest ("Nested Components");

        TestComponent comp;
        LayoutSupport layout (comp);

        // Add a child component to container
        juce::Component child;
        child.setComponentID ("child");
        comp.container.addAndMakeVisible (child);

        juce::String json = R"json({
            "components": [
                {
                    "id": "container",
                    "bounds": "parent",
                    "components": [
                        { "id": "child", "x": 10, "y": 10, "w": "parW - 20", "h": "parH - 20" }
                    ]
                }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (child.getX(), 10);
        expectEquals (child.getY(), 10);
        expectEquals (child.getWidth(), 780, "Should use parent (container) width");
        expectEquals (child.getHeight(), 580, "Should use parent (container) height");
    }

    void testMultipleIds()
    {
        beginTest ("Multiple IDs");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "button0,button1,button2", "x": "10 + idIdx * 110", "y": 10, "w": 100, "h": 30 }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (comp.buttons[0]->getX(), 10, "Button 0 should be at x=10");
        expectEquals (comp.buttons[1]->getX(), 120, "Button 1 should be at x=120");
        expectEquals (comp.buttons[2]->getX(), 230, "Button 2 should be at x=230");
    }

    void testRangeSyntax()
    {
        beginTest ("Range Syntax");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "button[0..4]", "x": "10 + idIdx * 50", "y": 10, "w": 40, "h": 30 }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (comp.buttons[0]->getX(), 10);
        expectEquals (comp.buttons[1]->getX(), 60);
        expectEquals (comp.buttons[2]->getX(), 110);
        expectEquals (comp.buttons[3]->getX(), 160);
        expectEquals (comp.buttons[4]->getX(), 210);
    }

    void testConditionalComponents()
    {
        beginTest ("Conditional Components");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "constants": [
                { "showButton": 0 }
            ],
            "components": [
                { "id": "button0", "if": "showButton", "x": 10, "y": 10, "w": 100, "h": 30 },
                { "id": "button1", "if": "1", "x": 10, "y": 50, "w": 100, "h": 30 }
            ]
        })json";

        layout.setLayout (json);

        // Button0 should not have been positioned (if: 0)
        // Button1 should have been positioned (if: 1)
        expectEquals (comp.buttons[0]->getY(), 0, "Button0 should not be positioned when showButton=0");
        expectEquals (comp.buttons[1]->getY(), 50, "Button1 should be positioned when if=1");
    }

    void testVisibility()
    {
        beginTest ("Visibility");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "button0", "x": 10, "y": 10, "w": 100, "h": 30, "visible": false },
                { "id": "button1", "x": 10, "y": 50, "w": 100, "h": 30, "visible": true }
            ]
        })json";

        layout.setLayout (json);

        expect (! comp.buttons[0]->isVisible(), "Button0 should be hidden");
        expect (comp.buttons[1]->isVisible(), "Button1 should be visible");
    }

    void testAccessibilityProperties()
    {
        beginTest ("Accessibility Properties");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "button0", "x": 10, "y": 10, "w": 100, "h": 30, "title": "Close Button", "tip": "Click to close" }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (comp.buttons[0]->getTitle(), juce::String ("Close Button"), "Should set accessibility title");
        expectEquals (comp.buttons[0]->getTooltip(), juce::String ("Click to close"), "Should set tooltip");
    }

    void testCustomProperties()
    {
        beginTest ("Custom Properties");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                {
                    "id": "button0",
                    "x": 10,
                    "y": 10,
                    "w": 100,
                    "h": 30,
                    "properties": {
                        "customProp": "testValue",
                        "numericProp": 42
                    }
                }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (comp.buttons[0]->getProperties()["customProp"].toString(), juce::String ("testValue"), "Should set string property");
        expectEquals (int (comp.buttons[0]->getProperties()["numericProp"]), 42, "Should set numeric property");
    }

    void testLayoutChangedCallback()
    {
        beginTest ("Layout Changed Callback");

        TestComponent comp;
        LayoutSupport layout (comp);

        bool callbackCalled = false;
        layout.layoutChanged = [&callbackCalled]()
        {
            callbackCalled = true;
        };

        juce::String json = R"json({ "components": [] })json";
        layout.setLayout (json);

        expect (callbackCalled, "Layout changed callback should be called");
    }

    void testComponentFactory()
    {
        beginTest ("Component Factory");

        TestComponent comp;

        // Create layout with factory
        LayoutSupport layout (comp, [&comp] (const juce::String& factoryName) -> std::pair<juce::Component*, bool>
        {
            if (factoryName == "textButton")
            {
                auto* btn = new juce::TextButton();
                btn->setComponentID ("dynamicButton");
                comp.addAndMakeVisible (btn);
                return { btn, false };
            }
            return { nullptr, false };
        });

        juce::String json = R"json({
            "components": [
                { "id": "dynamicButton", "factory": "textButton", "x": 10, "y": 10, "w": 100, "h": 30 }
            ]
        })json";

        layout.setLayout (json);

        // Find the dynamically created component
        juce::Component* found = nullptr;
        for (int i = 0; i < comp.getNumChildComponents(); i++)
        {
            auto* child = comp.getChildComponent (i);
            if (child->getComponentID() == "dynamicButton")
            {
                found = child;
                break;
            }
        }

        expect (found != nullptr, "Dynamic component should be created");
        if (found)
        {
            expectEquals (found->getX(), 10, "Dynamic component should be positioned");
            expectEquals (found->getWidth(), 100, "Dynamic component should be sized");
        }
    }
};

static LayoutSupportTests layoutSupportTests;

#endif
