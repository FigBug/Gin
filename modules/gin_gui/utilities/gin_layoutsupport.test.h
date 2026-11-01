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
        testCenterDeltaPositioning();
        testEdgePositioning();
        testBoundsSpecialModes();
        testSizeProperties();
        testExpressions();
        testGetFunctions();
        testConstants();
        testMacros();
        testGridLayout();
        testGridLayoutWithConstants();
        testNestedComponents();
        testMultipleIds();
        testRangeSyntax();
        testConditionalComponents();
        testVisibility();
        testAccessibilityProperties();
        testCustomProperties();
        testLayoutChangedCallback();
        testComponentFactory();
        testOptionalComponents();
        testInset();
        testParentCenterConstants();
        testParentHalfSizeConstants();
        testPostWidthHeight();
        testPostWidthHeightWithMinMax();
        testPostWidthOnly();
        testPostHeightOnly();
        testTransitionSpecs();
        testTransitionPresets();
        testTransitionInheritance();
        testTransitionDefaultPreset();
        testTransitionHotReload();
        testLayoutTransitionNotShowing();
        testAnimatedNotShowing();
        testAlpha();
        testInterpolators();
        testCrossfadeNotShowing();
        testTransitionDescendants();
        testScale();
        testImageEffects();
        testTransitionsFromCode();
        testStates();
        testShadows();
        testGlobalSwitch();
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

    void testGridLayoutWithConstants()
    {
        beginTest ("Grid Layout With Constants");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "constants": [
                { "gridCols": 2 },
                { "gridRows": 2 },
                { "gridColGap": 10 },
                { "gridRowGap": 10 }
            ],
            "components": [
                {
                    "grid": "button0,button1,button2,button3",
                    "bounds": "0,0,400,200",
                    "cols": "gridCols",
                    "rows": "gridRows",
                    "colGap": "gridColGap",
                    "rowGap": "gridRowGap"
                }
            ]
        })json";

        layout.setLayout (json);

        // Grid should position components in 2x2 layout with gaps using constants
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

    void testCenterDeltaPositioning()
    {
        beginTest ("Center Delta Positioning (cxd/cyd)");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "button0", "x": 10, "y": 10, "w": 100, "h": 30 },
                { "id": "button1", "cxd": 20 },
                { "id": "button2", "cyd": 20 }
            ]
        })json";

        layout.setLayout (json);

        // All buttons are 100x30 (button1/button2 inherit from previous)
        // button0 center X = 10 + 100/2 = 60
        // button1 should have center at 60 + 20 = 80, so x = 80 - 100/2 = 30
        int expectedX1 = (10 + 100 / 2) + 20 - 100 / 2;
        expectEquals (comp.buttons[1]->getX(), expectedX1, "cxd should position relative to previous center X");

        // button0 center Y = 10 + 30/2 = 25
        // button2 should have center at 25 + 20 = 45, so y = 45 - 30/2 = 30
        int expectedY2 = (10 + 30 / 2) + 20 - 30 / 2;
        expectEquals (comp.buttons[2]->getY(), expectedY2, "cyd should position relative to previous center Y");
    }

    void testGetFunctions()
    {
        beginTest ("Get Functions (getX, getY, getW, getH, getR, getB, getCX, getCY)");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "button0", "x": 50, "y": 100, "w": 200, "h": 60 },
                { "id": "button1", "x": "getX('button0')", "y": "getY('button0') + getH('button0') + 10", "w": "getW('button0')", "h": 30 },
                { "id": "button2", "x": "getR('button0') + 10", "y": "getB('button0') - 30", "w": 80, "h": 30 },
                { "id": "button3", "cx": "getCX('button0')", "cy": "getCY('button0')", "w": 100, "h": 40 }
            ]
        })json";

        layout.setLayout (json);

        // button1: x = getX(button0) = 50, y = getY(button0) + getH(button0) + 10 = 100 + 60 + 10 = 170
        expectEquals (comp.buttons[1]->getX(), 50, "getX should return component X");
        expectEquals (comp.buttons[1]->getY(), 170, "getY + getH should work in expression");
        expectEquals (comp.buttons[1]->getWidth(), 200, "getW should return component width");

        // button2: x = getR(button0) + 10 = (50 + 200) + 10 = 260, y = getB(button0) - 30 = (100 + 60) - 30 = 130
        expectEquals (comp.buttons[2]->getX(), 260, "getR should return right edge");
        expectEquals (comp.buttons[2]->getY(), 130, "getB should return bottom edge");

        // button3: centered on button0's center
        // button0 center: (50 + 200/2, 100 + 60/2) = (150, 130)
        // button3: cx=150, cy=130, w=100, h=40 -> x = 150-50 = 100, y = 130-20 = 110
        expectEquals (comp.buttons[3]->getX(), 100, "getCX should return center X");
        expectEquals (comp.buttons[3]->getY(), 110, "getCY should return center Y");
    }

    void testOptionalComponents()
    {
        beginTest ("Optional Components");

        TestComponent comp;
        LayoutSupport layout (comp);

        // Reference a non-existent component with optional: true - should not assert
        juce::String json = R"json({
            "components": [
                { "id": "nonExistentComponent", "optional": true, "x": 10, "y": 10, "w": 100, "h": 30 },
                { "id": "button0", "x": 20, "y": 20, "w": 100, "h": 30 }
            ]
        })json";

        layout.setLayout (json);

        // button0 should still be positioned correctly even though nonExistentComponent wasn't found
        expectEquals (comp.buttons[0]->getX(), 20, "Layout should continue after optional missing component");
        expectEquals (comp.buttons[0]->getY(), 20, "Layout should continue after optional missing component");
    }

    void testInset()
    {
        beginTest ("Inset Positioning");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "container", "x": 50, "y": 50, "w": 300, "h": 200 },
                { "id": "button0", "inset": "container", "border": 10 },
                { "id": "button1", "inset": "container" }
            ]
        })json";

        layout.setLayout (json);

        // button0 should be inset from container with 10px border
        // container: (50, 50, 300, 200)
        // button0: (50+10, 50+10, 300-20, 200-20) = (60, 60, 280, 180)
        expectEquals (comp.buttons[0]->getX(), 60, "Inset X should include border");
        expectEquals (comp.buttons[0]->getY(), 60, "Inset Y should include border");
        expectEquals (comp.buttons[0]->getWidth(), 280, "Inset width should subtract border*2");
        expectEquals (comp.buttons[0]->getHeight(), 180, "Inset height should subtract border*2");

        // button1 should be inset with default border (0)
        expectEquals (comp.buttons[1]->getX(), 50, "Inset X with default border should match container");
        expectEquals (comp.buttons[1]->getY(), 50, "Inset Y with default border should match container");
        expectEquals (comp.buttons[1]->getWidth(), 300, "Inset width with default border should match container");
        expectEquals (comp.buttons[1]->getHeight(), 200, "Inset height with default border should match container");
    }

    void testParentCenterConstants()
    {
        beginTest ("Parent Center Constants (parCX, parCY)");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "button0", "x": "parCX", "y": "parCY", "w": 100, "h": 30 },
                { "id": "button1", "cx": "parCX", "cy": "parCY", "w": 100, "h": 30 }
            ]
        })json";

        layout.setLayout (json);

        // comp is 800x600, so center is at (400, 300)
        int expectedCX = 400;
        int expectedCY = 300;

        // button0: top-left at parent center
        expectEquals (comp.buttons[0]->getX(), expectedCX, "parCX should be parent center X (400)");
        expectEquals (comp.buttons[0]->getY(), expectedCY, "parCY should be parent center Y (300)");

        // button1: centered at parent center
        // x = cx - w/2 = 400 - 50 = 350
        // y = cy - h/2 = 300 - 15 = 285
        expectEquals (comp.buttons[1]->getX(), 350, "Component centered using parCX should be at 350");
        expectEquals (comp.buttons[1]->getY(), 285, "Component centered using parCY should be at 285");
    }

    void testParentHalfSizeConstants()
    {
        beginTest ("Parent Half Size Constants (parW2, parH2)");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "button0", "x": 0, "y": 0, "w": "parW2", "h": "parH2" },
                { "id": "button1", "x": "parW2", "y": "parH2", "w": "parW2", "h": "parH2" },
                { "id": "button2", "cx": "parW2", "cy": "parH2", "w": 100, "h": 30 }
            ]
        })json";

        layout.setLayout (json);

        // comp is 800x600, so parW2 = 400, parH2 = 300
        int expectedW2 = 400;
        int expectedH2 = 300;

        // button0: top-left quadrant
        expectEquals (comp.buttons[0]->getWidth(), expectedW2, "parW2 should be half parent width (400)");
        expectEquals (comp.buttons[0]->getHeight(), expectedH2, "parH2 should be half parent height (300)");

        // button1: bottom-right quadrant
        expectEquals (comp.buttons[1]->getX(), expectedW2, "Should start at parW2");
        expectEquals (comp.buttons[1]->getY(), expectedH2, "Should start at parH2");
        expectEquals (comp.buttons[1]->getWidth(), expectedW2, "Should use parW2 for width");
        expectEquals (comp.buttons[1]->getHeight(), expectedH2, "Should use parH2 for height");

        // button2: centered at (parW2, parH2) which is parent center
        expectEquals (comp.buttons[2]->getX(), 350, "Centered at parW2 should be at 350");
        expectEquals (comp.buttons[2]->getY(), 285, "Centered at parH2 should be at 285");
    }

    void testPostWidthHeight()
    {
        beginTest ("Post Width and Height (pW, pH)");

        TestComponent comp;
        LayoutSupport layout (comp);

        // Add children to container
        juce::Component child1, child2, child3;
        child1.setComponentID ("child1");
        child2.setComponentID ("child2");
        child3.setComponentID ("child3");
        comp.container.addAndMakeVisible (child1);
        comp.container.addAndMakeVisible (child2);
        comp.container.addAndMakeVisible (child3);

        juce::String json = R"json({
            "components": [
                {
                    "id": "container",
                    "x": 10,
                    "y": 10,
                    "w": 500,
                    "h": 400,
                    "pW": "maxX + 20",
                    "pH": "maxY + 20",
                    "components": [
                        { "id": "child1", "x": 10, "y": 10, "w": 100, "h": 50 },
                        { "id": "child2", "x": 120, "y": 10, "w": 100, "h": 50 },
                        { "id": "child3", "x": 10, "y": 70, "w": 150, "h": 80 }
                    ]
                }
            ]
        })json";

        layout.setLayout (json);

        // Children bounds:
        // child1: (10, 10, 100, 50) -> right=110, bottom=60
        // child2: (120, 10, 100, 50) -> right=220, bottom=60
        // child3: (10, 70, 150, 80) -> right=160, bottom=150
        // maxX = 220, maxY = 150
        // pW = maxX + 20 = 240
        // pH = maxY + 20 = 170

        expectEquals (comp.container.getWidth(), 240, "Container width should be maxX + 20");
        expectEquals (comp.container.getHeight(), 170, "Container height should be maxY + 20");
    }

    void testPostWidthHeightWithMinMax()
    {
        beginTest ("Post Width/Height with minX, minY, maxX, maxY constants");

        TestComponent comp;
        LayoutSupport layout (comp);

        // Add children to container
        juce::Component child1, child2;
        child1.setComponentID ("child1");
        child2.setComponentID ("child2");
        comp.container.addAndMakeVisible (child1);
        comp.container.addAndMakeVisible (child2);

        juce::String json = R"json({
            "components": [
                {
                    "id": "container",
                    "x": 0,
                    "y": 0,
                    "w": 1000,
                    "h": 1000,
                    "pW": "maxX - minX + 40",
                    "pH": "maxY - minY + 40",
                    "components": [
                        { "id": "child1", "x": 20, "y": 30, "w": 100, "h": 50 },
                        { "id": "child2", "x": 50, "y": 100, "w": 200, "h": 60 }
                    ]
                }
            ]
        })json";

        layout.setLayout (json);

        // Children bounds:
        // child1: (20, 30, 100, 50) -> right=120, bottom=80
        // child2: (50, 100, 200, 60) -> right=250, bottom=160
        // minX = 20, minY = 30
        // maxX = 250, maxY = 160
        // pW = maxX - minX + 40 = 250 - 20 + 40 = 270
        // pH = maxY - minY + 40 = 160 - 30 + 40 = 170

        expectEquals (comp.container.getWidth(), 270, "Container width should be maxX - minX + 40");
        expectEquals (comp.container.getHeight(), 170, "Container height should be maxY - minY + 40");
    }

    void testPostWidthOnly()
    {
        beginTest ("Post Width Only (pW without pH)");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::Component child1;
        child1.setComponentID ("child1");
        comp.container.addAndMakeVisible (child1);

        juce::String json = R"json({
            "components": [
                {
                    "id": "container",
                    "x": 0,
                    "y": 0,
                    "w": 500,
                    "h": 300,
                    "pW": "maxX + 10",
                    "components": [
                        { "id": "child1", "x": 10, "y": 10, "w": 200, "h": 100 }
                    ]
                }
            ]
        })json";

        layout.setLayout (json);

        // child1: right = 210
        // pW = maxX + 10 = 210 + 10 = 220
        // pH not specified, so height remains 300

        expectEquals (comp.container.getWidth(), 220, "Container width should be adjusted by pW");
        expectEquals (comp.container.getHeight(), 300, "Container height should remain unchanged");
    }

    void testPostHeightOnly()
    {
        beginTest ("Post Height Only (pH without pW)");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::Component child1;
        child1.setComponentID ("child1");
        comp.container.addAndMakeVisible (child1);

        juce::String json = R"json({
            "components": [
                {
                    "id": "container",
                    "x": 0,
                    "y": 0,
                    "w": 500,
                    "h": 300,
                    "pH": "maxY + 15",
                    "components": [
                        { "id": "child1", "x": 10, "y": 10, "w": 200, "h": 180 }
                    ]
                }
            ]
        })json";

        layout.setLayout (json);

        // child1: bottom = 190
        // pH = maxY + 15 = 190 + 15 = 205
        // pW not specified, so width remains 500

        expectEquals (comp.container.getWidth(), 500, "Container width should remain unchanged");
        expectEquals (comp.container.getHeight(), 205, "Container height should be adjusted by pH");
    }

    void testTransitionSpecs()
    {
        beginTest ("Transition Specs");

        auto t = Transition::fromSpec ("250 inOutQuad");
        expect (t.has_value(), "Spec should parse");
        expectEquals (t->durationMs, 250.0);
        expect (t->easing == Transition::findEasing ("inOutQuad"), "Easing should be inOutQuad");
        expect (! t->isInstant(), "Should not be instant");

        expectWithinAbsoluteError (t->getProgress (0.0), 0.0f, 0.0001f);
        expectWithinAbsoluteError (t->getProgress (62.5), 0.125f, 0.0001f);
        expectWithinAbsoluteError (t->getProgress (125.0), 0.5f, 0.0001f);
        expectWithinAbsoluteError (t->getProgress (250.0), 1.0f, 0.0001f);
        expectWithinAbsoluteError (t->getProgress (1000.0), 1.0f, 0.0001f);

        auto linear = Transition::fromSpec ("  100  ");
        expect (linear.has_value(), "Duration alone should parse");
        expect (linear->easing == Transition::findEasing ("linear"), "Missing easing should be linear");
        expectWithinAbsoluteError (linear->getProgress (25.0), 0.25f, 0.0001f);

        auto none = Transition::fromSpec ("none");
        expect (none.has_value() && none->isInstant(), "none should be instant");

        auto zero = Transition::fromSpec ("0 outCubic");
        expect (zero.has_value() && zero->isInstant(), "Zero duration should be instant");
        expectWithinAbsoluteError (zero->getProgress (0.0), 1.0f, 0.0001f);

        expect (! Transition::fromSpec ("100 wobble").has_value(), "Unknown easing should fail");
        expect (! Transition::fromSpec ("quick").has_value(), "A name is not a spec");
        expect (! Transition::fromSpec ("").has_value(), "Empty spec should fail");
        expect (! Transition::fromSpec ("100 inQuad extra").has_value(), "Extra tokens should fail");

        for (auto name : { "linear", "inQuad", "outQuad", "inOutQuad", "inCubic", "outCubic", "inOutCubic", "outExpo", "inOutExpo", "inOutBack" })
            expect (Transition::findEasing (name) != nullptr, juce::String ("Easing should exist: ") + name);

        expect (Transition::findEasing ("InQuad") == nullptr, "Easing names are case sensitive");
    }

    void testTransitionPresets()
    {
        beginTest ("Transition Presets");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "transitions": { "quick": "100 outQuad", "glide": "650 inOutCubic" },
            "components": [
                { "id": "button0", "x": 10, "y": 20, "w": 100, "h": 30 }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (comp.buttons[0]->getX(), 10, "Layout should be unchanged by transitions");

        auto quick = Transition::resolve (*comp.buttons[0], "quick");
        expectEquals (quick.durationMs, 100.0);
        expect (quick.easing == Transition::findEasing ("outQuad"));

        expectEquals (Transition::resolve (comp, "glide").durationMs, 650.0, "Presets resolve from the parent itself");
        expectEquals (Transition::resolve (comp, "30 linear").durationMs, 30.0, "Inline specs resolve");
        expect (Transition::resolve (comp, "none").isInstant(), "none resolves instant");

        juce::Component outside;
        expect (Transition::forProperty (outside, "bounds").isInstant(), "Components outside the layout don't animate");
        expect (Transition::forProperty (*comp.buttons[0], "bounds").isInstant(), "Presets alone animate nothing");
    }

    void testTransitionInheritance()
    {
        beginTest ("Transition Inheritance");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::Component child1, child2, child3, grandChild;
        child1.setComponentID ("child1");
        child2.setComponentID ("child2");
        child3.setComponentID ("child3");
        grandChild.setComponentID ("grandChild");
        comp.container.addAndMakeVisible (child1);
        comp.container.addAndMakeVisible (child2);
        comp.container.addAndMakeVisible (child3);
        child3.addAndMakeVisible (grandChild);

        juce::String json = R"json({
            "transitions": { "quick": "100 outQuad", "glide": "650 inOutCubic" },
            "components": [
                { "id": "container", "x": 0, "y": 0, "w": 400, "h": 400, "transition": "glide",
                    "components": [
                        { "id": "child1", "x": 0, "y": 0, "w": 10, "h": 10 },
                        { "id": "child2", "x": 0, "y": 0, "w": 10, "h": 10, "transition": "none" },
                        { "id": "child3", "x": 0, "y": 0, "w": 10, "h": 10, "transition": { "alpha": "quick", "colour": "none", "bounds": "40 linear" },
                            "components": [
                                { "id": "grandChild", "x": 0, "y": 0, "w": 5, "h": 5 }
                            ]
                        }
                    ]
                },
                { "id": "header", "x": 0, "y": 0, "w": 10, "h": 10 }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (Transition::forProperty (comp.container, "bounds").durationMs, 650.0, "A string applies to the component");
        expectEquals (Transition::forProperty (child1, "bounds").durationMs, 650.0, "Children inherit");
        expectEquals (Transition::forProperty (child1, "alpha").durationMs, 650.0, "A string applies to every property");
        expect (Transition::forProperty (child2, "bounds").isInstant(), "none overrides the inherited transition");

        expectEquals (Transition::forProperty (child3, "alpha").durationMs, 100.0, "Per property presets");
        expectEquals (Transition::forProperty (child3, "bounds").durationMs, 40.0, "Per property inline specs");
        expect (Transition::forProperty (child3, "colour").isInstant(), "Per property none");
        expectEquals (Transition::forProperty (child3, "scale").durationMs, 650.0, "Properties not listed inherit");

        expectEquals (Transition::forProperty (grandChild, "alpha").durationMs, 100.0, "Grandchildren inherit per property");
        expectEquals (Transition::forProperty (grandChild, "scale").durationMs, 650.0, "Grandchildren inherit from further up");

        expect (Transition::forProperty (comp.header, "bounds").isInstant(), "Siblings don't inherit");
    }

    void testTransitionDefaultPreset()
    {
        beginTest ("Transition Default Preset");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "transitions": { "default": "250 inOutQuad" },
            "components": [
                { "id": "button0", "x": 10, "y": 20, "w": 100, "h": 30 },
                { "id": "button1", "x": 10, "y": 60, "w": 100, "h": 30, "transition": "none" }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (Transition::forProperty (*comp.buttons[0], "bounds").durationMs, 250.0, "default applies without a transition");
        expectEquals (Transition::forProperty (*comp.buttons[2], "alpha").durationMs, 250.0, "default applies to components the json doesn't list");
        expect (Transition::forProperty (*comp.buttons[1], "bounds").isInstant(), "none overrides default");
    }

    void testTransitionHotReload()
    {
        beginTest ("Transition Hot Reload");

        TestComponent comp;
        LayoutSupport layout (comp);

        layout.setLayout (juce::String (R"json({
            "transitions": { "quick": "100 outQuad" },
            "components": [
                { "id": "button0", "x": 10, "y": 20, "w": 100, "h": 30, "transition": "quick" }
            ]
        })json"));

        expectEquals (Transition::forProperty (*comp.buttons[0], "bounds").durationMs, 100.0);

        layout.setLayout (juce::String (R"json({
            "components": [
                { "id": "button0", "x": 10, "y": 20, "w": 100, "h": 30 }
            ]
        })json"));

        expect (Transition::forProperty (*comp.buttons[0], "bounds").isInstant(), "A removed transition no longer applies");
        expect (! comp.getProperties().contains (Transition::presetsProperty), "Removed presets no longer apply");
    }

    void testLayoutTransitionNotShowing()
    {
        beginTest ("Layout Transition Not Showing");

        TestComponent comp;
        LayoutSupport layout (comp);

        layout.setConstant ("open", 0);

        juce::String json = R"json({
            "transitions": { "glide": "650 inOutCubic" },
            "components": [
                { "id": "button0", "x": "10 + open * 100", "y": 20, "w": 100, "h": 30, "transition": "glide" },
                { "id": "button1", "x": "getR('button0') + 5", "y": 20, "w": 100, "h": 30 }
            ]
        })json";

        layout.setLayout (json);
        expectEquals (comp.buttons[0]->getX(), 10);

        layout.setConstant ("open", 1);
        layout.setLayout (json);

        expectEquals (comp.buttons[0]->getX(), 110, "Components that aren't showing jump to the new bounds");
        expectEquals (comp.buttons[1]->getX(), 215, "Expressions see the final bounds");
    }

    void testAnimatedNotShowing()
    {
        beginTest ("Animated Not Showing");

        juce::Component owner;
        Animated<float> alpha (owner, "alpha", 1.0f);

        auto changes = 0;
        alpha.onChange = [&] { changes++; };

        alpha.setTarget (0.0f, "250 inOutQuad");

        expectWithinAbsoluteError (alpha.get(), 0.0f, 0.0001f, "A target applies at once while the owner isn't showing");
        expect (! alpha.isAnimating());
        expectEquals (changes, 1);

        alpha.setTarget (0.0f, "250 inOutQuad");
        expectEquals (changes, 1, "The same target changes nothing");

        Animated<juce::Colour> colour (owner, "colour", juce::Colours::black);
        colour.setValue (juce::Colours::white);
        expect (colour.get() == juce::Colours::white);

        Animated<juce::Rectangle<int>> bounds (owner, "bounds");
        bounds.setTarget ({ 1, 2, 3, 4 });
        expect (bounds.get() == juce::Rectangle<int> (1, 2, 3, 4));
        expect (bounds.getTarget() == juce::Rectangle<int> (1, 2, 3, 4));
    }

    void testAlpha()
    {
        beginTest ("Alpha");

        TestComponent comp;
        LayoutSupport layout (comp);

        layout.setConstant ("shown", 0);

        juce::String json = R"json({
            "transitions": { "quick": "100 inOutQuad" },
            "components": [
                { "id": "button0", "x": 0, "y": 0, "w": 10, "h": 10, "alpha": "shown", "transition": { "alpha": "quick" } },
                { "id": "button1", "x": 0, "y": 0, "w": 10, "h": 10, "alpha": 0.5 },
                { "id": "button2", "x": 0, "y": 0, "w": 10, "h": 10, "alpha": "shown * 0.25 + 0.5" }
            ]
        })json";

        layout.setLayout (json);

        expect (! comp.buttons[0]->isVisible(), "Alpha 0 hides");
        // Components keep their alpha in 8 bits
        expectWithinAbsoluteError (comp.buttons[1]->getAlpha(), 0.5f, 0.005f, "Fractional alpha");
        expect (comp.buttons[1]->isVisible());
        expectWithinAbsoluteError (comp.buttons[2]->getAlpha(), 0.5f, 0.005f, "Alpha expressions");

        layout.setConstant ("shown", 1);
        layout.setLayout (json);

        expect (comp.buttons[0]->isVisible(), "Alpha above 0 shows a hidden component");
        expectWithinAbsoluteError (comp.buttons[0]->getAlpha(), 1.0f, 0.005f, "Not showing: the alpha applies at once");
        expectWithinAbsoluteError (comp.buttons[2]->getAlpha(), 0.75f, 0.005f);
        expectEquals (comp.buttons[3]->getAlpha(), 1.0f, "Components without alpha keep theirs");

        layout.setConstant ("shown", 0);
        layout.setLayout (json);

        expect (! comp.buttons[0]->isVisible(), "Alpha 0 hides again");
    }

    void testInterpolators()
    {
        beginTest ("Interpolators");

        expectWithinAbsoluteError (Interpolator<float>::blend (10.0f, 20.0f, 0.25f), 12.5f, 0.0001f);
        expectWithinAbsoluteError (Interpolator<double>::blend (0.0, 4.0, 0.5f), 2.0, 0.0001);

        auto r = Interpolator<juce::Rectangle<int>>::blend ({ 0, 0, 10, 10 }, { 10, 20, 30, 10 }, 0.5f);
        expect (r == juce::Rectangle<int> (5, 10, 20, 10), "Rectangles blend their edges");

        auto f = Interpolator<juce::Font>::blend (juce::Font (juce::FontOptions (10.0f)), juce::Font (juce::FontOptions (20.0f)), 0.5f);
        expectWithinAbsoluteError (f.getHeight(), 15.0f, 0.001f, "Fonts blend their height");

        juce::ColourGradient a (juce::Colours::black, 0.0f, 0.0f, juce::Colours::white, 1.0f, 0.0f, false);
        juce::ColourGradient b (juce::Colours::red, 0.0f, 0.0f, juce::Colours::blue, 0.0f, 1.0f, false);
        b.addColour (0.5, juce::Colours::lime);

        auto mid = Interpolator<juce::ColourGradient>::blend (a, b, 0.5f);

        expectEquals (mid.getNumColours(), 3, "Stops are the union of both");
        expect (mid.getColour (0) == juce::Colours::black.interpolatedWith (juce::Colours::red, 0.5f));
        expect (mid.getColour (1) == a.getColourAtPosition (0.5).interpolatedWith (juce::Colours::lime, 0.5f), "Missing stops are resampled");
        expectWithinAbsoluteError (mid.point2.x, 0.5f, 0.0001f, "End points blend");
        expectWithinAbsoluteError (mid.point2.y, 0.5f, 0.0001f);

        expect (Interpolator<juce::ColourGradient>::blend (a, b, 1.0f).getColour (2) == juce::Colours::blue);
    }

    void testCrossfadeNotShowing()
    {
        beginTest ("Crossfade Not Showing");

        juce::Component owner;
        Crossfade<juce::String> text (owner, "text", "Off");

        expect (text.getCurrent() == "Off");
        expectEquals (text.getProgress(), 1.0f);

        text.setTarget ("On", "120 inOutQuad");

        expect (text.getCurrent() == "On", "A target applies at once while the owner isn't showing");
        expect (text.getPrevious() == "On");
        expectEquals (text.getProgress(), 1.0f);

        text.updateTarget ("Again");
        expect (text.getCurrent() == "Again");

        auto sides = 0;
        text.draw ([&] (const juce::String& s, float opacity)
        {
            sides++;
            expect (s == "Again");
            expectEquals (opacity, 1.0f);
        });
        expectEquals (sides, 1, "Without a fade only the current value paints");

        Animated<float> level (owner, "level", 0.0f);
        level.updateTarget (0.5f);
        expectEquals (level.get(), 0.5f, "updateTarget jumps without a running transition");

        level.followTarget (0.75f, "100 linear");
        expectEquals (level.get(), 0.75f, "followTarget applies at once while the owner isn't showing");
    }

    void testTransitionDescendants()
    {
        beginTest ("Transition Descendants");

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::Component child1, grandChild;
        child1.setComponentID ("child1");
        grandChild.setComponentID ("grandChild");
        comp.container.addAndMakeVisible (child1);
        child1.addAndMakeVisible (grandChild);

        juce::String json = R"json({
            "transitions": { "quick": "100 outQuad", "glide": "650 inOutCubic" },
            "components": [
                { "id": "container", "x": 0, "y": 0, "w": 400, "h": 400, "transition": "glide",
                    "components": [
                        { "id": "child1", "x": 0, "y": 0, "w": 10, "h": 10,
                          "transition": { "alpha": "quick", "bounds": "glide", "descendants": { "bounds": "none" } },
                            "components": [
                                { "id": "grandChild", "x": 0, "y": 0, "w": 5, "h": 5 }
                            ]
                        }
                    ]
                },
                { "id": "header", "x": 0, "y": 0, "w": 10, "h": 10, "transition": { "bounds": "quick", "descendants": "none" } }
            ]
        })json";

        layout.setLayout (json);

        expectEquals (Transition::forProperty (child1, "bounds").durationMs, 650.0, "descendants doesn't apply to the component");
        expectEquals (Transition::forProperty (child1, "alpha").durationMs, 100.0);
        expect (Transition::forProperty (grandChild, "bounds").isInstant(), "descendants overrides for the children");
        expectEquals (Transition::forProperty (grandChild, "alpha").durationMs, 650.0, "Properties descendants doesn't list continue upwards");
        expectEquals (Transition::forProperty (comp.header, "bounds").durationMs, 100.0);
    }

    void testScale()
    {
        beginTest ("Scale");

        auto origin = [this] (const juce::String& s, float x, float y)
        {
            auto o = parseTransformOrigin (s);
            expect (o.has_value(), "Origin should parse: " + s);

            if (o)
            {
                expectWithinAbsoluteError (o->x, x, 0.0001f, s);
                expectWithinAbsoluteError (o->y, y, 0.0001f, s);
            }
        };

        origin ("center", 0.5f, 0.5f);
        origin ("top left", 0.0f, 0.0f);
        origin ("left top", 0.0f, 0.0f);
        origin ("bottom", 0.5f, 1.0f);
        origin ("right", 1.0f, 0.5f);
        origin ("center right", 1.0f, 0.5f);
        origin ("0.25 1", 0.25f, 1.0f);
        origin ("25% 100%", 0.25f, 1.0f);
        origin ("0", 0.0f, 0.0f);
        origin ("left 75%", 0.0f, 0.75f);

        expect (! parseTransformOrigin ("middle").has_value());
        expect (! parseTransformOrigin ("left right").has_value());
        expect (! parseTransformOrigin ("").has_value());
        expect (! parseTransformOrigin ("1 2 3").has_value());

        // The pivot stays put, the rest scales away from it
        const juce::Rectangle<int> b (100, 50, 40, 20);
        const auto t = scaleTransform (b, 2.0f);

        auto check = [&] (juce::Point<float> p, juce::Point<float> expected, const juce::String& what)
        {
            const auto q = p.transformedBy (t);
            expectWithinAbsoluteError (q.x, expected.x, 0.001f, what);
            expectWithinAbsoluteError (q.y, expected.y, 0.001f, what);
        };

        check ({ 120.0f, 60.0f }, { 120.0f, 60.0f }, "The centre stays");
        check ({ 100.0f, 50.0f }, { 80.0f, 40.0f }, "The top left moves away from the centre");
        check ({ 140.0f, 70.0f }, { 160.0f, 80.0f }, "The bottom right moves away from the centre");

        const auto topLeft = scaleTransform (b, 2.0f, { 0.0f, 0.0f });
        const auto q = juce::Point<float> (100.0f, 50.0f).transformedBy (topLeft);
        expectWithinAbsoluteError (q.x, 100.0f, 0.001f, "A top left origin keeps the top left");
        expect (scaleTransform (b, 1.0f).isIdentity(), "Scale 1 is the identity");

        // The layout's entry, not showing: at once
        TestComponent comp;
        LayoutSupport layout (comp);

        layout.setConstant ("zoom", 0);

        juce::String json = R"json({
            "components": [
                { "id": "button0", "x": 100, "y": 50, "w": 40, "h": 20, "scale": "1 + zoom", "transition": { "scale": "60 inOutQuad" } },
                { "id": "button1", "x": 0, "y": 0, "w": 40, "h": 20, "scale": 0.5, "origin": "top left" }
            ]
        })json";

        layout.setLayout (json);

        expect (! comp.buttons[0]->isTransformed(), "Scale 1 leaves no transform");
        expect (comp.buttons[1]->getTransform() == scaleTransform (comp.buttons[1]->getBounds(), 0.5f, { 0.0f, 0.0f }), "origin applies");

        layout.setConstant ("zoom", 1);
        layout.setLayout (json);

        expect (comp.buttons[0]->getTransform() == scaleTransform (comp.buttons[0]->getBounds(), 2.0f), "Not showing: the scale applies at once");

        comp.buttons[0]->setBounds (0, 0, 40, 20);
        expect (comp.buttons[0]->getTransform() == scaleTransform (comp.buttons[0]->getBounds(), 2.0f), "The pivot follows the bounds");

        AnimatedScale zoom (comp.footer);
        zoom.setTarget (1.5f);
        expect (comp.footer.getTransform() == scaleTransform (comp.footer.getBounds(), 1.5f), "AnimatedScale from code");
        zoom.setValue (1.0f);
        expect (! comp.footer.isTransformed(), "Back to 1 clears the transform");
    }

    void testImageEffects()
    {
        beginTest ("Image Filters");

        expect (ImageFilter::fromSpec ("contrast 30 saturation 130").has_value());
        expect (ImageFilter::fromSpec ("greyscale brightness -20").has_value());
        expect (ImageFilter::fromSpec ("").has_value() && ImageFilter::fromSpec ("")->isEmpty(), "Empty = no filter");
        expect (! ImageFilter::fromSpec ("contrast").has_value(), "Missing value");
        expect (! ImageFilter::fromSpec ("contrast high").has_value(), "Malformed value");
        expect (! ImageFilter::fromSpec ("glow 3").has_value(), "Unknown name");
        expect (! ImageFilter::fromSpec ("gamma 0").has_value(), "Gamma must be above 0");
        expect (*ImageFilter::fromSpec ("invert") == *ImageFilter::fromSpec (" invert "));

        juce::Image img (juce::Image::ARGB, 32, 32, true);
        {
            juce::Graphics g (img);
            g.fillAll (juce::Colours::white);
        }

        juce::Component owner;
        AnimatedImageFilter filter (owner);

        filter.setImage (MipMap (img));
        expect (filter.getFilteredImage() == filter.getImage(), "No filter: the same levels");

        filter.setFilter ("invert");
        expect (! (filter.getFilteredImage() == filter.getImage()), "A filter builds its own levels");
        expect (filter.getFilteredImage().getImage().getPixelAt (3, 3) == juce::Colours::black, "Applied to the full size image");

        const auto built = filter.getFilteredImage();
        filter.setFilter ("invert");
        expect (filter.getFilteredImage() == built, "The same filter builds nothing again");

        filter.amount.setTarget (1.0f, "150 outQuad");
        expectEquals (filter.amount.get(), 1.0f, "Not showing: the amount applies at once");

        juce::Image out (juce::Image::ARGB, 32, 32, true);
        {
            juce::Graphics g (out);
            filter.draw (g, { 0.0f, 0.0f, 32.0f, 32.0f });
        }
        expect (out.getPixelAt (16, 16) == juce::Colours::black, "Amount 1 draws the filter");
    }

    void testStates()
    {
        beginTest ("States");

        auto parse = [] (const char* json) { return juce::JSON::parse (juce::String (json)); };

        // Merging: pressed over hover key by key, disabled alone
        const auto states = parse (R"({ "hover": { "scale": 1.05, "alpha": 0.8 }, "pressed": { "scale": 0.98 }, "disabled": { "alpha": 0.5 } })");

        expect (States::merge (states, false, false, false).isVoid(), "Nothing active: void");
        expectWithinAbsoluteError (float (States::merge (states, true, false, false)["scale"]), 1.05f, 0.0001f);
        expectWithinAbsoluteError (float (States::merge (states, true, true, false)["scale"]), 0.98f, 0.0001f, "Pressed wins");
        expectWithinAbsoluteError (float (States::merge (states, true, true, false)["alpha"]), 0.8f, 0.0001f, "Hover keys pressed doesn't set stay");
        expect (! States::merge (states, true, true, true).hasProperty ("scale"), "Disabled replaces hover and pressed");
        expectWithinAbsoluteError (float (States::merge (states, false, false, true)["alpha"]), 0.5f, 0.0001f);

        // The children of a container, also ones added later; no compounding further down
        TestComponent comp;
        LayoutSupport layout (comp);

        juce::Component tile1, tile2, inner;
        tile1.setComponentID ("tile1");
        comp.container.addAndMakeVisible (tile1);
        tile1.addAndMakeVisible (inner);

        layout.setLayout (juce::String (R"json({
            "components": [
                { "id": "container", "x": 0, "y": 0, "w": 400, "h": 400,
                  "children": { "states": { "disabled": { "scale": 0.5 } } },
                    "components": [ { "id": "tile1", "x": 0, "y": 0, "w": 100, "h": 100 } ]
                }
            ]
        })json"));

        expect (! tile1.isTransformed());

        tile1.setEnabled (false);
        expect (tile1.getTransform() == scaleTransform (tile1.getBounds(), 0.5f), "A child takes the container's states");
        expect (! inner.isTransformed(), "States don't reach the children's children");

        tile2.setBounds (100, 0, 100, 100);
        comp.container.addAndMakeVisible (tile2);
        tile2.setEnabled (false);
        expect (tile2.getTransform() == scaleTransform (tile2.getBounds(), 0.5f), "A child added later takes them too");

        tile1.setEnabled (true);
        expect (! tile1.isTransformed(), "Leaving the state returns to the base value");

        // A child's own states win
        States::set (tile2, parse (R"({ "disabled": { "scale": 0.25 } })"));
        expect (tile2.getTransform() == scaleTransform (tile2.getBounds(), 0.25f), "Own states over the container's");
        expect (States::get (tile2).isObject() && States::get (tile1).isVoid(), "get returns only own states");
        expect (States::getForChildren (comp.container).isObject());

        States::set (tile2, {});
        expect (tile2.getTransform() == scaleTransform (tile2.getBounds(), 0.5f), "Cleared: back to the container's");

        // The same from code
        juce::Component grid, codeTile;
        States::setForChildren (grid, parse (R"({ "disabled": { "alpha": 0 } })"));
        grid.addAndMakeVisible (codeTile);
        codeTile.setEnabled (false);
        expect (! codeTile.isVisible(), "Code-set child states, alpha 0 hides");
        codeTile.setEnabled (true);
        expect (codeTile.isVisible() && juce::exactlyEqual (codeTile.getAlpha(), 1.0f), "Back to the base alpha");

        comp.container.removeChildComponent (&tile2);
        expect (! tile2.isTransformed(), "A child that leaves drops the container's states");
    }

    void testShadows()
    {
        beginTest ("Shadows");

        // Parsing
        auto two = BoxShadow::parse ("0 4 8 0.45, 0 12 30 0.6");
        expect (two.has_value() && two->size() == 2);
        expect ((*two)[1].offset == juce::Point<float> (0.0f, 12.0f));
        expectEquals ((*two)[1].blur, 30.0f);
        expectEquals (int ((*two)[0].colour.getAlpha()), juce::roundToInt (0.45f * 255.0f));

        auto full = BoxShadow::parse ("1 2 3 0.5 4 #ff0000");
        expect (full.has_value() && (*full)[0].spread == 4.0f && (*full)[0].colour.getRed() == 255 && (*full)[0].colour.getAlpha() == 128);
        expect (BoxShadow::parse ("1 2 3 1 red").has_value());
        expect (BoxShadow::parse ("none")->empty() && BoxShadow::parse ("")->empty());
        expect (BoxShadow::parse (juce::JSON::parse ("[ \"0 1 2 0.5\", \"0 3 4 0.5\" ]"))->size() == 2, "An array of layers");
        expect (! BoxShadow::parse ("0 4 8").has_value(), "Too few values");
        expect (! BoxShadow::parse ("0 4 8 0.5 2 red 9").has_value(), "Too many values");
        expect (! BoxShadow::parse ("0 4 8 0.5 nocolour").has_value(), "Unknown colour");

        // Blending pads a missing layer with a clear copy
        auto mid = Interpolator<ShadowList>::blend ({ (*two)[0] }, *two, 0.5f);
        expect (mid.size() == 2);
        expectEquals (int (mid[1].colour.getAlpha()), juce::roundToInt ((*two)[1].colour.getAlpha() * 0.5f), "The new layer fades in");
        expect (mid[1].offset == (*two)[1].offset, "at its own geometry");

        // The parent paints, through the renderer
        struct Call { juce::Rectangle<float> bounds; ShadowSpec spec; };
        std::vector<Call> calls;

        BoxShadow::setRenderer ([&] (juce::Graphics&, const juce::Path& outline, const ShadowSpec& s)
        {
            calls.push_back ({ outline.getBounds(), s });
        });

        TestComponent comp;
        LayoutSupport layout (comp);

        juce::String json = R"json({
            "components": [
                { "id": "container", "x": 0, "y": 0, "w": 400, "h": 400, "pH": "maxY + 10",
                    "components": [
                        { "id": "tile", "x": 20, "y": 30, "w": 100, "h": 50, "boxShadow": "0 4 8 0.5, 0 12 30 0.6", "borderRadius": 10,
                          "states": { "disabled": { "boxShadow": "0 2 4 1" } } }
                    ]
                }
            ]
        })json";

        juce::Component tile;
        tile.setComponentID ("tile");
        comp.container.addAndMakeVisible (tile);

        layout.setLayout (json);

        expect (comp.container.getChildComponent (0) != &tile, "A layer goes in first, at the back");
        expect (comp.container.getChildComponent (0)->getProperties()[LayoutSupport::excludeFromLayout], "Left out of the layout");
        expectEquals (comp.container.getHeight(), 90, "maxY ignores the layer");

        juce::Image img (juce::Image::ARGB, 400, 400, true);
        auto paintShadows = [&]
        {
            calls.clear();
            juce::Graphics g (img);
            BoxShadow::paintChildShadows (g, comp.container);
        };

        paintShadows();
        expect (calls.size() == 2, "One call per layer");
        expect (calls.size() == 2 && calls[0].bounds == juce::Rectangle<float> (20.0f, 30.0f, 100.0f, 50.0f), "The outline is the child's bounds");
        expect (calls.size() == 2 && calls[1].spec.blur == 30.0f);

        tile.setAlpha (0.5f);
        paintShadows();
        expect (calls.size() == 2 && std::abs (calls[0].spec.colour.getFloatAlpha() - 0.25f) < 0.01f, "The child's alpha fades it");
        tile.setAlpha (1.0f);

        AnimatedScale zoom (tile);
        zoom.setTarget (2.0f);
        paintShadows();
        expect (calls.size() == 2 && calls[0].bounds == juce::Rectangle<float> (-30.0f, 5.0f, 200.0f, 100.0f), "The outline follows the scale");
        zoom.setValue (1.0f);

        tile.setEnabled (false);
        paintShadows();
        expect (calls.size() == 1 && calls[0].spec.blur == 4.0f, "States set the shadow");
        tile.setEnabled (true);

        tile.setVisible (false);
        paintShadows();
        expect (calls.empty(), "A hidden child casts none");
        tile.setVisible (true);

        BoxShadow::setRenderer (comp.container, [&] (juce::Graphics&, const juce::Path&, const ShadowSpec&) { calls.push_back ({}); calls.push_back ({}); });
        paintShadows();
        expect (calls.size() == 4, "A container's own renderer wins");
        BoxShadow::setRenderer (comp.container, nullptr);

        BoxShadow::setRenderer (nullptr);
        paintShadows();
        expect (calls.empty(), "No renderer, no shadows");

        // From code
        juce::Component parent, child;
        parent.setSize (100, 100);
        BoxShadow::set (child, "0 1 2 0.5");
        BoxShadow::setRadius (child, 3.0f);
        parent.addAndMakeVisible (child);
        expect (parent.getNumChildComponents() == 2, "A code-set shadow gets the layer when the child joins a parent");
    }

    void testGlobalSwitch()
    {
        beginTest ("Global Switch");

        expect (Transition::isGlobalEnabled() == (GIN_TRANSITIONS_ENABLED_BY_DEFAULT != 0), "Starts as the config macro says");

        const auto wasEnabled = Transition::isGlobalEnabled();
        Transition::setGlobalEnabled (true);

        // The rest needs a component on screen, so transitions really run, and a desktop
        // window belongs to the message thread, not to a thread holding its lock: macOS
        // throws outright when one is made anywhere else
        if (! juce::MessageManager::existsAndIsCurrentThread())
        {
            logMessage ("Not the message thread, skipped");
            Transition::setGlobalEnabled (wasEnabled);
            return;
        }

        juce::Component window;
        window.setBounds (0, 0, 50, 50);
        window.addToDesktop (0);
        window.setVisible (true);

        if (! window.isShowing())
        {
            logMessage ("No desktop, skipped");
            Transition::setGlobalEnabled (wasEnabled);
            return;
        }

        auto repaints = 0;
        Animated<float> value (window, "value", 0.0f);
        value.onChange = [&] { repaints++; };

        value.setTarget (1.0f, "10000 linear");
        expect (value.isAnimating(), "On: a transition runs");
        expect (value.get() < 1.0f);

        Transition::setGlobalEnabled (false);
        expect (! value.isAnimating(), "Switching off ends a running transition");
        expectEquals (value.get(), 1.0f, "at its target");

        repaints = 0;
        value.setTarget (0.0f, "10000 linear");
        expect (! value.isAnimating(), "Off: nothing runs, so nothing ticks");
        expectEquals (value.get(), 0.0f, "Off: the target applies at once");
        expectEquals (repaints, 1, "once");

        Crossfade<juce::String> text (window, "text", "a");
        text.setTarget ("b", "10000 linear");
        expect (! text.isAnimating() && text.getProgress() == 1.0f && text.getCurrent() == "b", "Crossfades too");

        Transition::setGlobalEnabled (true);
        value.setTarget (1.0f, "10000 linear");
        expect (value.isAnimating(), "On again: transitions run again");
        value.setValue (1.0f);

        window.removeFromDesktop();
        Transition::setGlobalEnabled (wasEnabled);
    }

    void testTransitionsFromCode()
    {
        beginTest ("Transitions From Code");

        // The same tree, once from json, once from code
        TestComponent comp;
        LayoutSupport layout (comp);

        juce::Component jsonChild;
        jsonChild.setComponentID ("child1");
        comp.container.addAndMakeVisible (jsonChild);

        layout.setLayout (juce::String (R"json({
            "transitions": { "quick": "100 outQuad", "glide": "650 inOutCubic" },
            "components": [
                { "id": "container", "x": 0, "y": 0, "w": 400, "h": 400,
                  "transition": { "bounds": "glide", "descendants": { "scale": "quick", "alpha": "none" } },
                    "components": [ { "id": "child1", "x": 0, "y": 0, "w": 10, "h": 10 } ]
                }
            ]
        })json"));

        juce::Component root, grid;
        root.addAndMakeVisible (grid);

        Transition::setPresets (root, R"({ "quick": "100 outQuad", "glide": "650 inOutCubic" })");
        Transition::set (grid, juce::JSON::parse (R"({ "bounds": "glide", "descendants": { "scale": "quick", "alpha": "none" } })"));

        // Added after the calls
        juce::Component tile;
        grid.addAndMakeVisible (tile);

        for (auto property : { "bounds", "scale", "alpha", "colour" })
        {
            const auto fromJson = Transition::forProperty (jsonChild, property);
            const auto fromCode = Transition::forProperty (tile, property);

            expectEquals (fromCode.durationMs, fromJson.durationMs, juce::String ("Same duration: ") + property);
            expect (fromCode.easing == fromJson.easing, juce::String ("Same easing: ") + property);
        }

        expectEquals (Transition::forProperty (tile, "scale").durationMs, 100.0, "A tile added later inherits");
        expectEquals (Transition::forProperty (grid, "bounds").durationMs, 650.0);
        expectEquals (Transition::resolve (tile, "glide").durationMs, 650.0, "Code presets resolve by name");

        expect (Transition::get (grid).isObject());
        expect (Transition::getPresets (root).hasProperty ("quick"));
        expect (Transition::get (comp.container).isObject(), "The layout writes the same property");

        juce::var object (new juce::DynamicObject());
        object.getDynamicObject()->setProperty ("hover", "60 inOutQuad");
        Transition::setPresets (root, object);
        expectEquals (Transition::resolve (tile, "hover").durationMs, 60.0, "Presets as an object");

        Transition::set (grid, {});
        Transition::setPresets (root, {});
        expect (Transition::get (grid).isVoid() && Transition::getPresets (root).isVoid(), "A void var clears");
        expect (Transition::forProperty (tile, "scale").isInstant(), "Cleared: nothing animates");
    }
};

static LayoutSupportTests layoutSupportTests;

#endif
