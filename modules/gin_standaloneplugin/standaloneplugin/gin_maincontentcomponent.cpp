
MainContentComponent::MainContentComponent (StandaloneFilterWindow& filterWindow)
    : owner (filterWindow), notification (this),
      editor (owner.getAudioProcessor()->hasEditor() ? owner.getAudioProcessor()->proc->createEditorIfNeeded()
                                                     : new juce::GenericAudioProcessorEditor (*owner.getAudioProcessor()))
{
    inputMutedValue.referTo (owner.pluginHolder->getMuteInputValue());

    if (editor != nullptr)
    {
        editor->addComponentListener (this);
        handleMovedOrResized();

        addAndMakeVisible (editor.get());
    }

    addChildComponent (notification);

    if (owner.pluginHolder->getProcessorHasPotentialFeedbackLoop())
    {
        inputMutedValue.addListener (this);
        shouldShowNotification = inputMutedValue.getValue();
    }

    inputMutedChanged (shouldShowNotification);
}

MainContentComponent::~MainContentComponent()
{
    if (editor != nullptr)
    {
        editor->removeComponentListener (this);
        owner.pluginHolder->processor->proc->editorBeingDeleted (editor.get());
        editor = nullptr;
    }
}

void MainContentComponent::resized()
{
    handleResized();
}

juce::ComponentBoundsConstrainer* MainContentComponent::getEditorConstrainer() const
{
    if (auto* e = editor.get())
        return e->getConstrainer();

    return nullptr;
}

juce::BorderSize<int> MainContentComponent::computeBorder() const
{
    const auto nativeFrame = [&]() -> juce::BorderSize<int>
    {
        if (auto* peer = owner.getPeer())
            if (const auto frameSize = peer->getFrameSizeIfPresent())
                return *frameSize;

        return {};
    }();

    return nativeFrame.addedTo (owner.getContentComponentBorder())
                      .addedTo (juce::BorderSize<int> { shouldShowNotification ? NotificationArea::height : 0, 0, 0,
                                                        shouldShowSidebar ? SideBarComponent::width : 0 });
}

void MainContentComponent::inputMutedChanged (bool newInputMutedValue)
{
    shouldShowNotification = newInputMutedValue;
    notification.setVisible (shouldShowNotification);

   #if JUCE_IOS || JUCE_ANDROID
    handleResized();
   #else
    if (editor != nullptr)
    {
        const int extraHeight = shouldShowNotification ? NotificationArea::height : 0;
        const int extraWidth = shouldShowSidebar ? SideBarComponent::width : 0;
        const auto rect = getSizeToContainEditor();
        setSize (rect.getWidth() + extraWidth, rect.getHeight() + extraHeight);
    }
   #endif
}

void MainContentComponent::valueChanged (juce::Value& value)
{
    inputMutedChanged (value.getValue());
}

void MainContentComponent::buttonClicked (juce::Button*)
{
   #if JUCE_IOS || JUCE_ANDROID
    owner.pluginHolder->getMuteInputValue().setValue (false);
   #else
    owner.pluginHolder->showAudioSettingsDialog();
   #endif
}

void MainContentComponent::handleResized()
{
    auto r = getLocalBounds();

    if (shouldShowNotification)
        notification.setBounds (r.removeFromTop (NotificationArea::height));

    if (shouldShowSidebar)
        sidebar.setBounds (r.removeFromRight (SideBarComponent::width));

    if (editor != nullptr)
    {
        const auto newPos = r.getTopLeft().toFloat().transformedBy (editor->getTransform().inverted());

        if (preventResizingEditor)
            editor->setTopLeftPosition (newPos.roundToInt());
        else
            editor->setBoundsConstrained (editor->getLocalArea (this, r.toFloat()).withPosition (newPos).toNearestInt());
    }
}

void MainContentComponent::handleMovedOrResized()
{
    const juce::ScopedValueSetter<bool> scope (preventResizingEditor, true);

    if (editor != nullptr)
    {
        auto rect = getSizeToContainEditor();

        setSize (rect.getWidth()  + (shouldShowSidebar      ? SideBarComponent::width  : 0),
                 rect.getHeight() + (shouldShowNotification ? NotificationArea::height : 0));
    }
}

void MainContentComponent::componentMovedOrResized (juce::Component&, bool, bool)
{
    handleMovedOrResized();
}

juce::Rectangle<int> MainContentComponent::getSizeToContainEditor() const
{
    if (editor != nullptr)
        return getLocalArea (editor.get(), editor->getLocalBounds());

    return {};
}
