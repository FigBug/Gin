#pragma once

/*==============================================================================

 Copyright 2023 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

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
