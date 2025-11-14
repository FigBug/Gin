#pragma once

/*==============================================================================

 Copyright 2023 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

/**
    Animated grid container with drag-and-drop reordering support.

    ComponentGrid arranges child components in a row or column with smooth animated
    transitions. Components can be reordered via drag-and-drop, making it ideal for
    customizable toolbars, tab bars, or any UI where users can rearrange elements.

    Key Features:
    - Automatic grid layout (horizontal or vertical)
    - Smooth animations when reordering
    - Drag-and-drop reordering with visual feedback
    - Configurable spacing between items
    - Callbacks for drag events and order changes
    - Component dragging across grid boundaries

    The grid handles layout and animation automatically. When children are
    reordered via drag-and-drop, the grid smoothly animates them to their new
    positions.

    Usage:
    @code
    ComponentGrid grid("ToolbarGrid", ComponentGrid::horizontal);
    grid.setGap(5); // 5 pixel spacing

    // Add components
    grid.addAndMakeVisible(new Button("Tool 1"));
    grid.addAndMakeVisible(new Button("Tool 2"));
    grid.addAndMakeVisible(new Button("Tool 3"));

    // Listen for order changes
    grid.onOrderChanged = [](int oldIndex, int newIndex) {
        // Save new order to preferences
    };

    // Optional: Control when dragging can start
    grid.onDragStart = [](const MouseEvent& e) {
        return e.mods.isLeftButtonDown(); // Only left-click dragging
    };
    @endcode

    @see ComponentAnimator
*/
class ComponentGrid : public juce::Component,
                      private juce::Timer
{
public:
    enum Orientation
    {
        vertical,
        horizontal,
    };

    ComponentGrid (const juce::String& name = {}, Orientation o = horizontal);
    ~ComponentGrid() override;

    void timerCallback() override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;
    void resized() override;

    bool isDragInProgress();

    void setGap (int);
    void setOrientation (Orientation);

    static bool isGridDrag (juce::var);
    static int getDragIndex (juce::var);

    std::function<bool(const juce::MouseEvent&)>    onDragStart;
    std::function<void (int, int)>                  onOrderChanged;
    std::function<void (int, int)>                  onDragFinished;

protected:
    void layoutAnimated();

    int componentIndex (juce::Component& c);
    juce::Array<juce::Rectangle<int>> getComponentRects();

    bool                            dragStarted = false;
    bool                            dragging = false;
    bool                            dragOut = false;
    Orientation                     orientation = horizontal;
    int                             gap = 1;
    juce::Array<juce::Component*>   originalOrder;
    juce::ComponentAnimator         animator;

    struct DragInfo : public juce::ReferenceCountedObject
    {
        ComponentGrid* grid = nullptr;
        Component* originalComponent = nullptr;
        int currentIndex = 0;
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComponentGrid)
};
