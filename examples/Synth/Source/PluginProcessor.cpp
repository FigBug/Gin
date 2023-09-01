#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthVoice.h"

static juce::String waveTextFunction (const gin::Parameter&, float v)
{
    switch ((gin::Wave)int (v))
    {
        case gin::Wave::silence:     return "Off";
        case gin::Wave::sine:        return "Sine";
        case gin::Wave::triangle:    return "Triangle";
        case gin::Wave::sawUp:       return "Saw (Up)";
        case gin::Wave::sawDown:     return "Saw (Down)";
        case gin::Wave::pulse:       return "Pulse";
        case gin::Wave::square:      return "Square";
        case gin::Wave::whiteNoise:  return "Noise";
        default:
            jassertfalse;
            return {};
    }
}

static juce::String filterTextFunction (const gin::Parameter&, float v)
{
    switch (int (v))
    {
        case 0: return "LP 12";
        case 1: return "LP 24";
        case 2: return "HP 12";
        case 3: return "HP 24";
        case 4: return "BP 12";
        case 5: return "BP 24";
        case 6: return "NT 12";
        case 7: return "NT 24";
        default:
            jassertfalse;
            return {};
    }
}

static juce::String freqTextFunction (const gin::Parameter&, float v)
{
    return juce::String (int (gin::getMidiNoteInHertz (v)));
}

//==============================================================================
void SynthAudioProcessor::OSCParams::setup (SynthAudioProcessor& p)
{
    juce::String id = "osc";
    juce::String nm = "OSC ";

    wave       = p.addIntParam (id + "wave",       nm + "Wave",        "Wave",      "", { 1.0, 7.0, 1.0, 1.0 }, 1.0, 0.0f, waveTextFunction);
    voices     = p.addIntParam (id + "unison",     nm + "Unison",      "Unison",    "", { 1.0, 8.0, 1.0, 1.0 }, 1.0, 0.0f);
    voicesTrns = p.addExtParam (id + "unisontrns", nm + "Unison Trns", "LTrans",    "st", { -36.0, 36.0, 1.0, 1.0 }, 0.0, 0.0f);
    detune     = p.addExtParam (id + "detune",     nm + "Detune",      "Detune",    "", { 0.0, 0.5, 0.0, 1.0 }, 0.0, 0.0f);
    spread     = p.addExtParam (id + "spread",     nm + "Spread",      "Spread",    "%", { -100.0, 100.0, 0.0, 1.0 }, 0.0, 0.0f);
}

//==============================================================================
void SynthAudioProcessor::FilterParams::setup (SynthAudioProcessor& p)
{
    juce::String id = "flt";
    juce::String nm = "FLT ";

    float maxFreq = float (gin::getMidiNoteFromHertz (20000.0));

    type             = p.addIntParam (id + "type",    nm + "Type",    "Type",  "", { 0.0, 7.0, 1.0, 1.0 }, 0.0, 0.0f, filterTextFunction);
    keyTracking      = p.addExtParam (id + "key",     nm + "Key",     "Key",   "%", { 0.0, 100.0, 0.0, 1.0 }, 0.0, 0.0f);
    velocityTracking = p.addExtParam (id + "vel",     nm + "Vel",     "Vel",   "%", { 0.0, 100.0, 0.0, 1.0 }, 0.0, 0.0f);
    frequency        = p.addExtParam (id + "freq",    nm + "Freq",    "Freq",  "Hz", { 0.0, maxFreq, 0.0, 1.0 }, 64.0, 0.0f, freqTextFunction);
    resonance        = p.addExtParam (id + "res",     nm + "Res",     "Res",   "", { 0.0, 100.0, 0.0, 1.0 }, 0.0, 0.0f);
    amount           = p.addExtParam (id + "amount",  nm + "Amount",  "Amnt",  "", { -1.0, 1.0, 0.0, 1.0 }, 0.0, 0.0f);
    attack           = p.addExtParam (id + "attack",  nm + "Attack",  "A",     "s", { 0.0, 60.0, 0.0, 0.2f }, 0.1f, 0.0f);
    decay            = p.addExtParam (id + "decay",   nm + "Decay",   "D",     "s", { 0.0, 60.0, 0.0, 0.2f }, 0.1f, 0.0f);
    sustain          = p.addExtParam (id + "sustain", nm + "Sustain", "S",     "%", { 0.0, 100.0, 0.0, 1.0 }, 80.0f, 0.0f);
    release          = p.addExtParam (id + "release", nm + "Release", "R",     "s", { 0.0, 60.0, 0.0, 0.2f }, 0.1f, 0.0f);

    sustain->conversionFunction          = [] (float in) { return in / 100.0f; };
    velocityTracking->conversionFunction = [] (float in) { return in / 100.0f; };
    keyTracking->conversionFunction      = [] (float in) { return in / 100.0f; };
}

//==============================================================================
void SynthAudioProcessor::ADSRParams::setup (SynthAudioProcessor& p)
{
    velocityTracking = p.addExtParam ("vel",     "Vel",     "Vel",   "", { 0.0, 100.0, 0.0, 1.0 }, 100.0, 0.0f);
    attack           = p.addExtParam ("attack",  "Attack",  "A",     "s", { 0.0, 60.0, 0.0, 0.2f },  0.1f, 0.0f);
    decay            = p.addExtParam ("decay",   "Decay",   "D",     "s", { 0.0, 60.0, 0.0, 0.2f },  0.1f, 0.0f);
    sustain          = p.addExtParam ("sustain", "Sustain", "S",     "%", { 0.0, 100.0, 0.0, 1.0 }, 80.0f, 0.0f);
    release          = p.addExtParam ("release", "Release", "R",     "s", { 0.0, 60.0, 0.0, 0.2f },  0.1f, 0.0f);

    sustain->conversionFunction          = [] (float in) { return in / 100.0f; };
    velocityTracking->conversionFunction = [] (float in) { return in / 100.0f; };
}

//==============================================================================
SynthAudioProcessor::SynthAudioProcessor()
{
    lf = std::make_unique<gin::CopperLookAndFeel>();

    enableLegacyMode();
    setVoiceStealingEnabled (true);

    oscParams.setup (*this);
    filterParams.setup (*this);
    adsrParams.setup (*this);

    for (int i = 0; i < 50; i++)
    {
        auto voice = new SynthVoice (*this, bandLimitedLookupTables);
        addVoice (voice);
    }

    setMono (false);
    setLegato (false);
    setGlissando (false);
    setPortamento (false);
    setGlideRate (0.3f);
    setNumVoices (50);
}

SynthAudioProcessor::~SynthAudioProcessor()
{
}

//==============================================================================
void SynthAudioProcessor::stateUpdated()
{
}

void SynthAudioProcessor::updateState()
{
}

void SynthAudioProcessor::reset()
{
    Processor::reset();
}

void SynthAudioProcessor::prepareToPlay (double newSampleRate, int newSamplesPerBlock)
{
    Processor::prepareToPlay (newSampleRate, newSamplesPerBlock);

    bandLimitedLookupTables.setSampleRate (newSampleRate);
    setCurrentPlaybackSampleRate (newSampleRate);
}

void SynthAudioProcessor::releaseResources()
{
}

void SynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    startBlock();
    setMPE (false);

    int pos = 0;
    int todo = buffer.getNumSamples();

    buffer.clear();

    while (todo > 0)
    {
        int thisBlock = std::min (todo, 32);
        renderNextBlock (buffer, midi, pos, thisBlock);

        pos += thisBlock;
        todo -= thisBlock;
    }

    endBlock (buffer.getNumSamples());
}

juce::Array<float> SynthAudioProcessor::getLiveFilterCutoff()
{
    juce::Array<float> values;

    for (auto v : voices)
    {
        if (v->isActive())
        {
            auto vav = dynamic_cast<SynthVoice*>(v);
            values.add (vav->getFilterCutoffNormalized());
        }
    }
    return values;
}

void SynthAudioProcessor::handleMidiEvent (const juce::MidiMessage& m)
{
    MPESynthesiser::handleMidiEvent (m);
}

void SynthAudioProcessor::handleController (int, int, int)
{
}

//==============================================================================
bool SynthAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SynthAudioProcessor::createEditor()
{
    return new SynthAudioProcessorEditor (*this);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthAudioProcessor();
}
