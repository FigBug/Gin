/*==============================================================================

 Copyright 2023 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

ComponentGrid::ComponentGrid (const juce::String& name, Orientation o)
    : juce::Component (name), orientation (o)
{
    setWantsKeyboardFocus (false);
    addMouseListener (this, true);
}

ComponentGrid::~ComponentGrid()
{
}

void ComponentGrid::setGap (int g)
{
    gap = g;
    resized();
}

void ComponentGrid::setOrientation (Orientation o)
{
    orientation = o;
    resized();
}

bool ComponentGrid::isDragInProgress()
{
    return dragging;
}

void ComponentGrid::resized()
{
    auto areas = getComponentRects();

    for (auto i = 0; i < getNumChildComponents(); i++)
        getChildComponent(i)->setBounds (areas[i]);
}

void ComponentGrid::layoutAnimated()
{
    if (auto dnd = juce::DragAndDropContainer::findParentDragContainerFor (this))
    {
        if (auto info = dynamic_cast<DragInfo*> (dnd->getCurrentDragDescription().getObject()); info != nullptr && dragging)
        {
            auto areas = getComponentRects();

            animator.cancelAllAnimations (true);
            for (auto i = 0; i < getNumChildComponents(); i++)
                if (auto c = getChildComponent(i); c != info->originalComponent)
                    animator.animateComponent (c, areas[i], 1.0f, 250, false, 1.0, 1.0);
        }
    }
}

void ComponentGrid::mouseDown (const juce::MouseEvent&)
{
    dragging = false;
    dragStarted = false;
}

void ComponentGrid::mouseDrag (const juce::MouseEvent& e)
{
    if (auto dnd = juce::DragAndDropContainer::findParentDragContainerFor (this))
    {
        auto parent = dynamic_cast<juce::Component*> (dnd);
        auto findScale = [&] (juce::Component* c)
        {
            auto scale = 1.0f;
            while (c != nullptr && c != parent)
            {
                scale *= std::sqrt (std::abs (c->getTransform().getDeterminant()));
                c = c->getParentComponent();
            }
            return scale;
        };

        auto shouldDrag = onDragStart == nullptr || onDragStart (e);
        auto isDragging = dnd->isDragAndDropActive();

        if (! isDragging && shouldDrag && ! dragStarted && e.originalComponent != this)
        {
            auto idx = componentIndex (*e.originalComponent);
            jassert (idx >= 0 && idx < getNumChildComponents());
            if (auto c = getChildComponent (idx))
            {
                originalOrder = getChildren();

                auto di = new DragInfo();
                di->originalComponent = c;
                di->currentIndex = idx;

                auto re = e.getEventRelativeTo (c);

                const auto scale = 2.0f * findScale (c);

                auto snapshot = c->createComponentSnapshot (c->getLocalBounds(), true, scale);
                snapshot.multiplyAllAlphas (0.6f);

                juce::Point<int> offset { -int (re.getMouseDownX()), -int (re.getMouseDownY()) };

                dnd->startDragging (juce::var (di), c, juce::ScaledImage (snapshot, 2.0f), false, &offset);
                c->setVisible (false);

                dragging = true;
                dragStarted = true;
                startTimerHz (30);
            }
            else
            {
                jassertfalse;
            }
        }

        if (auto info = dynamic_cast<DragInfo*> (dnd->getCurrentDragDescription().getObject()); info != nullptr && dragging)
        {
            auto re = e.getEventRelativeTo (this);
            auto pt = re.getPosition();

            auto indexOver = -1;
            auto rects = getComponentRects();

            for (auto i = 0; i < rects.size(); i++)
            {
                auto srcRect = rects[info->currentIndex];
                auto destRect = rects[i];

                destRect = destRect.withSize (std::min (srcRect.getWidth(), destRect.getWidth()), std::min (srcRect.getHeight(), destRect.getHeight()));

                if (destRect.contains (pt.x, pt.y))
                {
                    indexOver = i;
                    break;
                }
            }

            if (indexOver >= 0)
            {
                if (indexOver != info->currentIndex)
                {
                    auto c = getChildComponent (info->currentIndex);
                    removeChildComponent (c);
                    addChildComponent (c, indexOver >= info->currentIndex ? indexOver : indexOver);

                    info->currentIndex = indexOver;
                    layoutAnimated();

                    if (onOrderChanged)
                        onOrderChanged();
                }
            }
        }
    }
}


void ComponentGrid::mouseUp (const juce::MouseEvent&)
{
    dragStarted = false;

    if (dragging)
    {
        for (auto c : getChildren())
            c->setVisible (true);

        dragging = false;

        stopTimer();

        animator.cancelAllAnimations (true);
        resized();

        if (onDragFinished)
            onDragFinished();
    }
}

void ComponentGrid::timerCallback()
{
    auto dnd = juce::DragAndDropContainer::findParentDragContainerFor (this);
    if (dnd == nullptr)
        return;

    if (! dnd->isDragAndDropActive() && dragging)
    {
        for (auto c : getChildren())
            c->setVisible (true);

        dragging = false;

        stopTimer();

        animator.cancelAllAnimations (true);
        resized();

        if (onDragFinished)
            onDragFinished();
    }
}

int ComponentGrid::componentIndex (juce::Component& c)
{
    auto child = &c;

    while (child != nullptr && child->getParentComponent() != this)
        child = child->getParentComponent();

    return getChildren().indexOf (child);
}

juce::Array<juce::Rectangle<int>> ComponentGrid::getComponentRects()
{
    auto rc = getLocalBounds();
    juce::Array<juce::Rectangle<int>> res;

    if (orientation == vertical)
    {
        for (auto c : getChildren())
        {
            res.add (rc.removeFromTop (c->getHeight()));
            rc.removeFromTop (gap);
        }
    }
    else
    {
        for (auto c : getChildren())
        {
            res.add (rc.removeFromLeft (c->getWidth()));
            rc.removeFromLeft (gap);
        }
    }

    return res;
}
