//==============================================================================
MidiLearnOverlay::MidiLearnOverlay (MidiLearn& ml)
    : midiLearn (ml)
{
    setInterceptsMouseClicks (false, false);
    startTimerHz (30);
    setAlwaysOnTop (true);
}

MidiLearnOverlay::~MidiLearnOverlay()
{
    stopTimer();
}

bool MidiLearnOverlay::hitTest (int, int)
{
    return false;
}

void MidiLearnOverlay::timerCallback()
{
    auto learnParam = midiLearn.getLearningParameter();
    bool isLearning = learnParam != nullptr;

    // Detect when learning completes (was learning, now not learning)
    if (wasLearning && ! isLearning && targetComponent != nullptr)
    {
        successPhase = 1.0f;
    }

    wasLearning = isLearning;

    if (isLearning)
    {
        auto newTarget = findComponentForParameter (learnParam);

        if (newTarget != targetComponent)
        {
            targetComponent = newTarget;
            phase = 0.0f;
        }

        phase += 0.05f;
        if (phase > 1.0f)
            phase = 0.0f;

        repaint();
    }
    else if (successPhase > 0.0f)
    {
        // Fade out over 1 second (at 30Hz, that's 30 frames)
        successPhase -= 1.0f / 30.0f;
        if (successPhase <= 0.0f)
        {
            successPhase = 0.0f;
            targetComponent = nullptr;
        }

        phase += 0.05f;
        if (phase > 1.0f)
            phase = 0.0f;

        repaint();
    }
    else if (targetComponent != nullptr)
    {
        targetComponent = nullptr;
        repaint();
    }
}

juce::Component* MidiLearnOverlay::findComponentForParameter (gin::Parameter* param)
{
    if (param == nullptr || getParentComponent() == nullptr)
        return nullptr;

    std::function<juce::Component* (juce::Component*)> findRecursive = [&] (juce::Component* comp) -> juce::Component*
    {
        if (auto pc = dynamic_cast<ParamComponent*> (comp))
            if (pc->getUid() == param->getUid())
                return pc;

        for (auto child : comp->getChildren())
            if (auto found = findRecursive (child))
                return found;

        return nullptr;
    };

    return findRecursive (getParentComponent());
}

void MidiLearnOverlay::paint (juce::Graphics& g)
{
    auto c = targetComponent.getComponent();
    if (c == nullptr)
        return;

    const auto correctedPhase = std::clamp (phase * 1.3f, 0.0f, 1.0f);
    auto newAlpha = (1.0f - correctedPhase) * (1.0f - correctedPhase);

    // Apply success fade
    if (successPhase > 0.0f)
        newAlpha *= successPhase;

    if (newAlpha < 0.01f)
        return;

    const auto b = getLocalArea (c, c->getLocalBounds()).toFloat();

    const auto stateCol = successPhase > 0.0f ? juce::Colours::limegreen : juce::Colours::gold;

    const auto thickness = juce::jmap (correctedPhase, 0.0f, 9.0f);
    const auto expansion = 14.0f - thickness / 2.0f;

    const auto bckBright = std::clamp (findColour (GinLookAndFeel::backgroundColourId).getPerceivedBrightness() - 0.25f, 0.0f, 1.0f);
    const auto bckCol = juce::Colours::black.withAlpha (bckBright * newAlpha);

    // Check if target contains a rotary slider
    juce::Slider* rotarySlider = nullptr;
    std::function<juce::Slider* (juce::Component*)> findRotarySlider = [&] (juce::Component* comp) -> juce::Slider*
    {
        // Ignore ModulationDepthSlider
        if (dynamic_cast<ModulationDepthSlider*> (comp) != nullptr)
            return nullptr;

        if (auto sld = dynamic_cast<juce::Slider*> (comp))
            if (sld->getSliderStyle() == juce::Slider::RotaryVerticalDrag ||
                sld->getSliderStyle() == juce::Slider::RotaryHorizontalDrag ||
                sld->getSliderStyle() == juce::Slider::RotaryHorizontalVerticalDrag ||
                sld->getSliderStyle() == juce::Slider::Rotary)
                return sld;

        for (auto child : comp->getChildren())
            if (auto found = findRotarySlider (child))
                return found;

        return nullptr;
    };

    rotarySlider = findRotarySlider (c);

    if (rotarySlider != nullptr)
    {
        const auto sliderBounds = getLocalArea (rotarySlider, rotarySlider->getLocalBounds()).toFloat();
        const auto br = sliderBounds.expanded (2.0f + thickness / 2.0f);

        if (bckCol.getAlpha() > 0)
        {
            g.setColour (bckCol);
            g.drawEllipse (br, thickness);
        }

        g.setColour (stateCol.withMultipliedAlpha (newAlpha));
        g.drawEllipse (br, thickness);

        g.setColour (stateCol.interpolatedWith (juce::Colours::white, 0.5f).withMultipliedAlpha (newAlpha));
        g.drawEllipse (br, thickness / 2.0f);

        g.setColour (stateCol.interpolatedWith (juce::Colours::white, 0.75f).withMultipliedAlpha (newAlpha));
        g.drawEllipse (br, thickness / 4.0f);
    }
    else
    {
        // Find a control (ComboBox, Slider, or Button) to draw around
        std::function<juce::Component* (juce::Component*)> findControl = [&] (juce::Component* comp) -> juce::Component*
        {
            if (dynamic_cast<juce::ComboBox*> (comp) != nullptr ||
                dynamic_cast<juce::Slider*> (comp) != nullptr ||
                dynamic_cast<juce::Button*> (comp) != nullptr)
                return comp;

            for (auto child : comp->getChildren())
                if (auto found = findControl (child))
                    return found;

            return nullptr;
        };

        auto control = findControl (c);
        auto controlBounds = control != nullptr
            ? getLocalArea (control, control->getLocalBounds()).toFloat()
            : b;

        const auto br = controlBounds.expanded (2.0f + thickness / 2.0f);
        const auto radius = std::min (br.getWidth(), br.getHeight()) / 2.0f;

        if (bckCol.getAlpha() > 0)
        {
            g.setColour (bckCol);
            g.drawRoundedRectangle (br, radius, thickness);
        }

        g.setColour (stateCol.withMultipliedAlpha (newAlpha));
        g.drawRoundedRectangle (br, radius, thickness);

        g.setColour (stateCol.interpolatedWith (juce::Colours::white, 0.5f).withMultipliedAlpha (newAlpha));
        g.drawRoundedRectangle (br, radius, thickness / 2.0f);

        g.setColour (stateCol.interpolatedWith (juce::Colours::white, 0.75f).withMultipliedAlpha (newAlpha));
        g.drawRoundedRectangle (br, radius, thickness / 4.0f);
    }
}
