#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::String modeTextFunction (const gin::Parameter&, float v)
{
    if ((int (v)) == 0)
        return "Linear";
    return "3dB";
}

static juce::String onOffTextFunction (const gin::Parameter&, float v)
{
    if (int (v) == 0)
        return "On";
    return "Off";
}

static juce::String panTextFunction (const gin::Parameter&, float v)
{
    if (juce::String (v, 2) == "0.00")
        return "C";
    if (v < 0)
        return juce::String (-v, 2) + "L";
    return juce::String (v, 2) + "R";
}

//==============================================================================
static gin::ProcessorOptions getOpts()
{
    gin::ProcessorOptions opts;
    return opts;
}

EffectAudioProcessor::EffectAudioProcessor()
    : gin::Processor (false, getOpts())
{
    levelParam  = addExtParam ("level",  "Level",  {}, "dB", {-100.0f, 5.0f, 0.0f, 5.0f}, 0.0f, 0.05f);
    panParam    = addExtParam ("pan",    "Pan",    {}, {},   {-1.0f,   1.0f, 0.0f, 1.0},  0.0f, 0.05f,  panTextFunction);
    modeParam   = addIntParam ("mode",   "Mode",   {}, {},   { 0.0f,   1.0f, 0.0f, 1.0},  0.0f, 0.0f,   modeTextFunction);
    invertParam = addIntParam ("invert", "Invert", {}, {},   { 0.0f,   1.0f, 0.0f, 1.0},  0.0f, 0.0f,   onOffTextFunction);

    levelParam->conversionFunction = [] (float in) { return juce::Decibels::decibelsToGain (in); };

    init();
}

EffectAudioProcessor::~EffectAudioProcessor()
{
}

//==============================================================================
void EffectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto getGains = [] (float gain, float pan, int mode) -> std::pair<float, float>
    {
        if (mode == 0)
        {
            const float pv = pan * gain;

            return { gain - pv, gain + pv };
        }
        else
        {
            pan = (pan + 1.0f) * 0.5f;

            return
            {
                gain * std::sin ((1.0f - pan) * juce::MathConstants<float>::halfPi),
                gain * std::sin (pan * juce::MathConstants<float>::halfPi)
            };
        }
    };

    const auto mode   = modeParam->getUserValueInt();
    const auto inv    = invertParam->getUserValueBool() ? -1.0f : 1.0f;

    const auto numSamps = buffer.getNumSamples();
    auto pos = 0;

    while (isSmoothing() && pos < numSamps)
    {
        auto gain = levelParam->getProcValue (1);
        auto pan = panParam->getProcValue (1);

        auto [left, right] = getGains (gain, pan, mode);

        buffer.applyGain (0, pos, 1, left  * inv);
        buffer.applyGain (1, pos, 1, right * inv);

        pos++;
    }

    if (pos < numSamps)
    {
        auto todo = numSamps - pos;

        auto gain = levelParam->getProcValue (todo);
        auto pan = panParam->getProcValue (todo);

        auto [left, right] = getGains (gain, pan, mode);

        buffer.applyGain (0, pos, todo, left  * inv);
        buffer.applyGain (1, pos, todo, right * inv);
    }
}

//==============================================================================
bool EffectAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* EffectAudioProcessor::createEditor()
{
    return new EffectAudioProcessorEditor (*this);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EffectAudioProcessor();
}
