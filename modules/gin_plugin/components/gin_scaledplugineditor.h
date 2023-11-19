#pragma once

//==============================================================================
/** If you want your plugin editor to scale, just wrap it in this
*/
class ScaledPluginEditor : public juce::AudioProcessorEditor
{
public:
    ScaledPluginEditor (juce::AudioProcessorEditor* editor_, juce::ValueTree state_ = {})
        : juce::AudioProcessorEditor (editor_->processor), editor (editor_)
    {
        auto w = editor->getWidth();
        auto h = editor->getHeight();

        setSize (w, h);
        setResizable (true, true);
        constrainer.setSizeLimits (w / 4, h / 4, w * 4, h * 4);
        constrainer.setFixedAspectRatio (double (w) / h);
        setConstrainer (&constrainer);

        addAndMakeVisible (frame);
        frame.addAndMakeVisible (*editor);
        frame.setBounds (getLocalBounds());

        setLookAndFeel (&editor->getLookAndFeel());

        if (state_.isValid())
            if (auto instance = state_.getChildWithName ("instance"); instance.isValid())
                if (instance.hasProperty ("editorScale"))
                    if (auto scale = (float)instance.getProperty ("editorScale"); scale > 0.0f)
                        setSize (int (w * scale), int (h * scale));

        state = state_;
    }

    ~ScaledPluginEditor() override
    {
        processor.editorBeingDeleted (this);
        setLookAndFeel (nullptr);
    }

    void setScale (float scale)
    {
        auto w = editor->getWidth();
        auto h = editor->getHeight();

        setSize (int (w * scale), int (h * scale));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);
    }

    void resized() override
    {
        auto scale = std::min (float (getWidth()) / editor->getWidth(), float (getHeight()) / editor->getHeight());
        frame.setTransform(juce::AffineTransform().scale (scale));

        if (state.isValid())
            state.getOrCreateChildWithName ("instance", nullptr).setProperty ("editorScale", scale, nullptr);
    }

    juce::Component frame;
    std::unique_ptr<juce::AudioProcessorEditor> editor;
    juce::ComponentBoundsConstrainer constrainer;

    juce::ValueTree state;
};
