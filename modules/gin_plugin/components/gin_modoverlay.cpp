//==============================================================================
ModOverlay::ModOverlay (ModMatrix& mm)
    : modMatrix (mm)
{
    setInterceptsMouseClicks (false, false);
    setAlwaysOnTop (true);

    startTimerHz (60);
}

ModOverlay::~ModOverlay()
{
    stopTimer();
}

bool ModOverlay::hitTest (int, int)
{
    return false;
}

void ModOverlay::timerCallback()
{
    isDragging = false;

    if (auto dnd = juce::DragAndDropContainer::findParentDragContainerFor (this))
        if (dnd->getNumCurrentDrags() == 1)
            if (dnd->getCurrentDragDescription().toString().startsWith ("modSrc"))
                isDragging = true;

    if (! isDragging)
    {
        repaint();
        return;
    }

    // Get mouse position in our coordinate space
    auto mousePos = juce::Desktop::getInstance().getMainMouseSource().getScreenPosition();
    mousePoint = getLocalPoint (nullptr, mousePos);

    // Find the active source button
    if (auto* srcButton = findActiveSourceButton())
    {
        auto buttonBounds = getLocalArea (srcButton, srcButton->getLocalBounds());
        sourcePoint = buttonBounds.getCentre().toFloat();
    }

    repaint();
}

juce::Component* ModOverlay::findActiveSourceButton()
{
    if (getParentComponent() == nullptr)
        return nullptr;

    juce::String sourceId;

    if (auto dnd = juce::DragAndDropContainer::findParentDragContainerFor (this))
        if (dnd->getNumCurrentDrags() == 1)
            if (dnd->getCurrentDragDescription().toString().startsWith ("modSrc"))
                sourceId = dnd->getCurrentDragDescription().toString().substring (6);

    std::function<juce::Component* (juce::Component*)> findRecursive = [&] (juce::Component* comp) -> juce::Component*
    {
        if (auto srcBtn = dynamic_cast<ModulationSourceButton*> (comp))
            if (juce::String (srcBtn->getSource().id) == sourceId && srcBtn->isDragging())
                return srcBtn;

        for (auto child : comp->getChildren())
            if (auto found = findRecursive (child))
                return found;

        return nullptr;
    };

    return findRecursive (getParentComponent());
}

bool ModOverlay::isOverValidDestination()
{
    if (getParentComponent() == nullptr)
        return false;

    auto screenPos = juce::Desktop::getInstance().getMainMouseSource().getScreenPosition();

    auto getSourceDetails = [&] () -> juce::DragAndDropTarget::SourceDetails
    {
        if (auto dnd = juce::DragAndDropContainer::findParentDragContainerFor (this))
            if (dnd->getNumCurrentDrags() == 1)
                if (dnd->getCurrentDragDescription().toString().startsWith ("modSrc"))
                    return { dnd->getCurrentDragDescription(), nullptr, {} };

        return { juce::var(), nullptr, {} };
    };

    std::function<bool (juce::Component*)> findRecursive = [&] (juce::Component* comp) -> bool
    {
        if (auto knob = dynamic_cast<Knob*> (comp))
        {
            auto localPos = knob->getLocalPoint (nullptr, screenPos);
            if (knob->getLocalBounds().contains (localPos.roundToInt()) && knob->isInterestedInDragSource (getSourceDetails()))
                return true;
        }

        for (auto child : comp->getChildren())
            if (findRecursive (child))
                return true;

        return false;
    };

    return findRecursive (getParentComponent());
}

void ModOverlay::paint (juce::Graphics& g)
{
    if (! isDragging)
        return;

    auto srcButton = findActiveSourceButton();
    if (srcButton == nullptr)
        return;

    // Determine cable color based on whether mouse is over valid destination
    auto isValid = isOverValidDestination();
    auto cableColour = isValid ? findColour (GinLookAndFeel::accentColourId).withAlpha (0.9f) : juce::Colours::grey.withAlpha (0.6f);

    auto glowColour = isValid ? findColour (GinLookAndFeel::accentColourId).withAlpha (0.3f) : juce::Colours::transparentBlack;

    // Calculate cable slack based on distance
    auto distance = sourcePoint.getDistanceFrom (mousePoint);
    auto slack = std::max (20.0f, distance * 0.3f);

    // Ensure points aren't at the same x position (catenary needs different x values)
    auto x0 = sourcePoint.x;
    auto y0 = sourcePoint.y;
    auto x1 = mousePoint.x;
    auto y1 = mousePoint.y;

    if (std::abs (x1 - x0) < 1.0f)
        x1 = x0 + 1.0f;

    // Create catenary path
    juce::Path cablePath;

    try
    {
        gin::Catenary catenary (x0, y0, x1, y1, slack, 3, true);

        auto startX = std::min (x0, x1);
        auto endX = std::max (x0, x1);
        auto step = (endX - startX) / 50.0f;

        bool first = true;
        for (auto x = startX; x <= endX; x += step)
        {
            auto y = catenary.calcY (x);
            if (first)
            {
                cablePath.startNewSubPath (x, y);
                first = false;
            }
            else
            {
                cablePath.lineTo (x, y);
            }
        }

        // Make sure we reach the end point
        cablePath.lineTo (endX, catenary.calcY (endX));
    }
    catch (...)
    {
        // Fall back to a simple line if catenary calculation fails
        cablePath.startNewSubPath (sourcePoint);
        cablePath.lineTo (mousePoint);
    }

    // Draw glow
    if (glowColour.getAlpha() > 0)
    {
        g.setColour (glowColour);
        g.strokePath (cablePath, juce::PathStrokeType (6.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Draw cable
    g.setColour (cableColour);
    g.strokePath (cablePath, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Draw end circles
    g.fillEllipse (sourcePoint.x - 4, sourcePoint.y - 4, 8, 8);
    g.fillEllipse (mousePoint.x - 4, mousePoint.y - 4, 8, 8);
}
