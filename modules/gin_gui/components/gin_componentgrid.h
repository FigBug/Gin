#pragma once

/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
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
    /** Grid layout orientation */
    enum Orientation
    {
        vertical,       ///< Items arranged vertically (top to bottom)
        horizontal,     ///< Items arranged horizontally (left to right)
    };

    /** Creates a ComponentGrid with the specified name and orientation.

        @param name  Name for the grid component
        @param o     Layout orientation (horizontal or vertical)
    */
    ComponentGrid (const juce::String& name = {}, Orientation o = horizontal);

    /** Destructor */
    ~ComponentGrid() override;

    /** Checks if a drag operation is currently in progress.

        @returns true if an item is being dragged
    */
    bool isDragInProgress();

    /** Sets the gap/spacing between grid items.

        @param gap  Spacing in pixels between items
    */
    void setGap (int gap);

    /** Sets the grid layout orientation.

        @param o  New orientation (horizontal or vertical)
    */
    void setOrientation (Orientation o);

    /** Checks if a drag source represents a ComponentGrid drag operation.

        @param dragSourceDetails  Drag source information from JUCE drag-and-drop
        @returns true if this is a grid drag operation
    */
    static bool isGridDrag (juce::var dragSourceDetails);

    /** Gets the index of the dragged component from drag source details.

        @param dragSourceDetails  Drag source information from JUCE drag-and-drop
        @returns The index of the component being dragged, or -1 if not a grid drag
    */
    static int getDragIndex (juce::var dragSourceDetails);

    /** Callback invoked when a drag operation starts.

        Return false to prevent dragging. Default behavior allows all drags.

        Example:
        @code
        grid.onDragStart = [](const MouseEvent& e) {
            return e.mods.isLeftButtonDown(); // Only left-click dragging
        };
        @endcode

        @param MouseEvent  The mouse event that initiated the drag
        @returns true to allow dragging, false to prevent it
    */
    std::function<bool(const juce::MouseEvent&)>    onDragStart;

    /** Callback invoked when items are reordered via drag-and-drop.

        Called after the drag completes and items have been reordered.

        Example:
        @code
        grid.onOrderChanged = [](int oldIndex, int newIndex) {
            DBG("Item moved from " << oldIndex << " to " << newIndex);
        };
        @endcode

        @param oldIndex  Original position of the dragged item
        @param newIndex  New position after reordering
    */
    std::function<void (int, int)>                  onOrderChanged;

    /** Callback invoked when a drag operation completes.

        Called when the mouse is released, whether or not reordering occurred.

        @param oldIndex  Original position of the dragged item
        @param newIndex  Final position (may be same as oldIndex if no reorder)
    */
    std::function<void (int, int)>                  onDragFinished;

    /** @internal Timer callback for animations */
    void timerCallback() override;

    /** @internal Mouse down handler for drag initiation */
    void mouseDown (const juce::MouseEvent& e) override;

    /** @internal Mouse drag handler for reordering */
    void mouseDrag (const juce::MouseEvent& e) override;

    /** @internal Mouse up handler for drag completion */
    void mouseUp (const juce::MouseEvent& e) override;

    /** @internal Resized handler for layout updates */
    void resized() override;

protected:
    /** @internal Performs animated layout of all child components */
    void layoutAnimated();

    /** @internal Gets the index of a component in the grid
        @param c  The component to find
        @returns The component's index, or -1 if not found
    */
    int componentIndex (juce::Component& c);

    /** @internal Calculates the target rectangles for all components
        @returns Array of rectangles for each component in order
    */
    juce::Array<juce::Rectangle<int>> getComponentRects();

    bool                            dragStarted = false;    ///< @internal True if drag gesture started
    bool                            dragging = false;       ///< @internal True during active drag
    bool                            dragOut = false;        ///< @internal True if dragged outside grid
    Orientation                     orientation = horizontal; ///< @internal Current layout orientation
    int                             gap = 1;                ///< @internal Spacing between items in pixels
    juce::Array<juce::Component*>   originalOrder;          ///< @internal Original order before drag
    juce::ComponentAnimator         animator;               ///< @internal Handles smooth animations

    /** @internal Drag operation information passed via JUCE drag-and-drop system */
    struct DragInfo : public juce::ReferenceCountedObject
    {
        ComponentGrid* grid = nullptr;              ///< Grid the drag originated from
        Component* originalComponent = nullptr;     ///< Component being dragged
        int currentIndex = 0;                       ///< Current index during drag
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComponentGrid)
};
