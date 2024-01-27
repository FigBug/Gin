#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthVoice.h"

static juce::String subTextFunction (const gin::Parameter&, float v)
{
    switch (int (v))
    {
        case 0: return "Sine";
        case 1: return "Triangle";
        case 2: return "Saw";
        case 3: return "Pulse";
        default:
            jassertfalse;
            return {};
    }
}

static juce::String lfoTextFunction (const gin::Parameter&, float v)
{
    switch ((gin::LFO::WaveShape)int (v))
    {
        case gin::LFO::WaveShape::none:          return "None";
        case gin::LFO::WaveShape::sine:          return "Sine";
        case gin::LFO::WaveShape::triangle:      return "Triangle";
        case gin::LFO::WaveShape::sawUp:         return "Saw Up";
        case gin::LFO::WaveShape::sawDown:       return "Saw Down";
        case gin::LFO::WaveShape::square:        return "Square";
        case gin::LFO::WaveShape::squarePos:     return "Square+";
        case gin::LFO::WaveShape::sampleAndHold: return "S&H";
        case gin::LFO::WaveShape::noise:         return "Noise";
        case gin::LFO::WaveShape::stepUp3:       return "Step Up 3";
        case gin::LFO::WaveShape::stepUp4:       return "Step Up 4";
        case gin::LFO::WaveShape::stepup8:       return "Step Up 8";
        case gin::LFO::WaveShape::stepDown3:     return "Step Down 3";
        case gin::LFO::WaveShape::stepDown4:     return "Step Down 4";
        case gin::LFO::WaveShape::stepDown8:     return "Step Down 8";
        case gin::LFO::WaveShape::pyramid3:      return "Pyramid 3";
        case gin::LFO::WaveShape::pyramid5:      return "Pyramid 5";
        case gin::LFO::WaveShape::pyramid9:      return "Pyramid 9";
        default:
            jassertfalse;
            return {};
    }
}

static juce::String enableTextFunction (const gin::Parameter&, float v)
{
    return v > 0.0f ? "On" : "Off";
}

static juce::String durationTextFunction (const gin::Parameter&, float v)
{
    return gin::NoteDuration::getNoteDurations()[size_t (v)].getName();
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

static juce::String glideModeTextFunction (const gin::Parameter&, float v)
{
    switch (int (v))
    {
        case 0: return "Off";
        case 1: return "Glissando";
        case 2: return "Portamento";
        default:
            jassertfalse;
            return {};
    }
}

//==============================================================================
void SynthAudioProcessor::OSCParams::setup (SynthAudioProcessor& p)
{
    juce::String id = "osc";
    juce::String nm = "OSC";

    wave       = p.addIntParam (id + "wave",       nm + "Wave",        "Wave",      "", { 0.0, 3.0, 1.0, 1.0 }, 1.0, 0.0f, subTextFunction);
    enable     = p.addIntParam (id + "enable",     nm + "Enable",      "Enable",    "", { 0.0, 1.0, 1.0, 1.0 }, 1.0f, 0.0f);
    voices     = p.addIntParam (id + "unison",     nm + "Unison",      "Unison",    "", { 1.0, 8.0, 1.0, 1.0 }, 1.0, 0.0f);
    tune       = p.addExtParam (id + "tune",       nm + "Tune",        "Tune",      "st", { -36.0, 36.0, 1.0, 1.0 }, 0.0, 0.0f);
    finetune   = p.addExtParam (id + "finetune",   nm + "Fine Tune",   "Fine",      "ct", { -100.0, 100.0, 0.0, 1.0 }, 0.0, 0.0f);
    level      = p.addExtParam (id + "level",      nm + "Level",       "Level",     "db", { -100.0, 0.0, 1.0, 4.0 }, 0.0, 0.0f);
    detune     = p.addExtParam (id + "detune",     nm + "Detune",      "Detune",    "", { 0.0, 0.5, 0.0, 1.0 }, 0.0, 0.0f);
    spread     = p.addExtParam (id + "spread",     nm + "Spread",      "Spread",    "%", { -100.0, 100.0, 0.0, 1.0 }, 0.0, 0.0f);
    pan        = p.addExtParam (id + "pan",        nm + "Pan",         "Pan",       "", { -1.0, 1.0, 0.0, 1.0 }, 0.0, 0.0f);

    level->conversionFunction = [] (float in)   { return juce::Decibels::decibelsToGain (in); };
}

//==============================================================================
void SynthAudioProcessor::FilterParams::setup (SynthAudioProcessor& p)
{
    juce::String id = "flt";
    juce::String nm = "FLT ";

    float maxFreq = float (gin::getMidiNoteFromHertz (20000.0));

    enable           = p.addIntParam (id + "enable",  nm + "Enable",  "",      "", { 0.0, 1.0, 1.0, 1.0 }, 1.0f, 0.0f);
    type             = p.addIntParam (id + "type",    nm + "Type",    "Type",  "", { 0.0, 7.0, 1.0, 1.0 }, 0.0, 0.0f, filterTextFunction);
    keyTracking      = p.addExtParam (id + "key",     nm + "Key",     "Key",   "%", { 0.0, 100.0, 0.0, 1.0 }, 0.0, 0.0f);
    frequency        = p.addExtParam (id + "freq",    nm + "Freq",    "Freq",  "Hz", { 0.0, maxFreq, 0.0, 1.0 }, 64.0, 0.0f, freqTextFunction);
    resonance        = p.addExtParam (id + "res",     nm + "Res",     "Res",   "", { 0.0, 100.0, 0.0, 1.0 }, 0.0, 0.0f);

    keyTracking->conversionFunction      = [] (float in) { return in / 100.0f; };
}

//==============================================================================
void SynthAudioProcessor::LFOParams::setup (SynthAudioProcessor& p)
{
    juce::String id = "lfo";
    juce::String nm = "LFO";

    auto notes = gin::NoteDuration::getNoteDurations();

    enable           = p.addIntParam (id + "enable",  nm + "Enable",  "Enable", "", { 0.0, 1.0, 1.0, 1.0 }, 0.0f, 0.0f, enableTextFunction);
    sync             = p.addIntParam (id + "sync",    nm + "Sync",    "Sync",   "", { 0.0, 1.0, 1.0, 1.0 }, 0.0, 0.0f, enableTextFunction);
    wave             = p.addIntParam (id + "wave",    nm + "Wave",    "Wave",   "", { 1.0, 17.0, 1.0, 1.0 }, 1.0, 0.0f, lfoTextFunction);
    rate             = p.addExtParam (id + "rate",    nm + "Rate",    "Rate",   "Hz", { 0.0, 50.0, 0.0, 0.3f }, 10.0, 0.0f);
    beat             = p.addIntParam (id + "beat",    nm + "Beat",    "Beat",   "", { 0.0, float (notes.size() - 1), 1.0, 1.0 }, 13.0, 0.0f, durationTextFunction);
    depth            = p.addExtParam (id + "depth",   nm + "Depth",   "Depth",  "", { -1.0, 1.0, 0.0, 1.0 }, 1.0, 0.0f);
    phase            = p.addExtParam (id + "phase",   nm + "Phase",   "Phase",  "", { -1.0, 1.0, 0.0, 1.0 }, 0.0, 0.0f);
    offset           = p.addExtParam (id + "offset",  nm + "Offset",  "Offset", "", { -1.0, 1.0, 0.0, 1.0 }, 0.0, 0.0f);
    fade             = p.addExtParam (id + "fade",    nm + "Fade",    "Fade",   "s", { -60.0, 60.0, 0.0, 0.2f, true }, 0.0f, 0.0f);
    delay            = p.addExtParam (id + "delay",   nm + "Delay",   "Delay",  "s", { 0.0, 60.0, 0.0, 0.2f }, 0.0f, 0.0f);
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
void SynthAudioProcessor::GlobalParams::setup (SynthAudioProcessor& p)
{
    mono        = p.addIntParam ("mono",    "Mono",       "",      "",   { 0.0, 1.0, 0.0, 1.0 }, 0.0, 0.0f, enableTextFunction);
    glideMode   = p.addIntParam ("gMode",   "Glide Mode", "Glide", "",   { 0.0, 2.0, 0.0, 1.0 }, 0.0f, 0.0f, glideModeTextFunction);
    glideRate   = p.addExtParam ("gRate",   "Glide Rate", "Rate",  "s",   { 0.001f, 20.0, 0.0, 0.2f }, 0.3f, 0.0f);
    legato      = p.addIntParam ("legato",  "Legato",     "",      "",   { 0.0, 1.0, 0.0, 1.0 }, 0.0, 0.0f, enableTextFunction);
    level       = p.addExtParam ("level",   "Level",      "",      "db", { -100.0, 0.0, 1.0, 4.0f }, 0.0, 0.0f);
    voices      = p.addIntParam ("voices",  "Voices",     "",      "",   { 2.0, 40.0, 1.0, 1.0 }, 40.0f, 0.0f);
    mpe         = p.addIntParam ("mpe",     "MPE",        "",      "",   { 0.0, 1.0, 1.0, 1.0 }, 0.0f, 0.0f, enableTextFunction);

    level->conversionFunction     = [] (float in) { return juce::Decibels::decibelsToGain (in); };
}

//==============================================================================
void SynthAudioProcessor::DelayParams::setup (SynthAudioProcessor& p)
{
    enable = p.addIntParam ("dlEnable",    "Enable",     "",   "", { 0.0, 1.0, 1.0, 1.0 }, 0.0f, 0.0f, enableTextFunction);

    float mxd = float (gin::NoteDuration::getNoteDurations().size()) - 1.0f;

    sync  = p.addExtParam ("dlSync",  "Sync",      "", "",   {   0.0f,   1.0f, 1.0f, 1.0f},    0.0f, 0.0f, enableTextFunction);
    time  = p.addExtParam ("dlTime",  "Delay",     "", "",   {   0.0f, 120.0f, 0.0f, 0.3f},    1.0f, 0.0f);
    beat  = p.addExtParam ("dlBeat",  "Delay",     "", "",   {   0.0f,    mxd, 1.0f, 1.0f},   13.0f, 0.0f, durationTextFunction);
    fb    = p.addExtParam ("dlFb",    "FB",        "", "dB", {-100.0f,   0.0f, 0.0f, 5.0f},  -10.0f, 0.1f);
    cf    = p.addExtParam ("dlCf",    "CF",        "", "dB", {-100.0f,   0.0f, 0.0f, 5.0f}, -100.0f, 0.1f);
    mix   = p.addExtParam ("dlMix",   "Mix",       "", "%",  {   0.0f, 100.0f, 0.0f, 1.0f},    0.5f, 0.1f);

    delay = p.addIntParam ("dlDelay", "Delay",     "", "",   {   0.0f, 120.0f, 0.0f, 1.0f},    1.0f, {0.2f, gin::SmoothingType::eased});

    fb->conversionFunction  = [] (float in) { return juce::Decibels::decibelsToGain (in); };
    cf->conversionFunction  = [] (float in) { return juce::Decibels::decibelsToGain (in); };
    mix->conversionFunction = [] (float in) { return in / 100.0f; };
}

//==============================================================================
SynthAudioProcessor::SynthAudioProcessor() : gin::Processor (false)
{
    enableLegacyMode();
    setVoiceStealingEnabled (true);

    oscParams.setup (*this);
    filterParams.setup (*this);
    lfoParams.setup (*this);
    adsrParams.setup (*this);
    globalParams.setup (*this);
    delayParams.setup (*this);

    for (int i = 0; i < 50; i++)
    {
        auto voice = new SynthVoice (*this);
        modMatrix.addVoice (voice);
        addVoice (voice);
    }

    setupModMatrix();
    init();
}

SynthAudioProcessor::~SynthAudioProcessor()
{
}

//==============================================================================
void SynthAudioProcessor::setupModMatrix()
{
    modSrcPressure  = modMatrix.addPolyModSource ("mpep", "MPE Pressure", false);
    modSrcTimbre    = modMatrix.addPolyModSource ("mpet", "MPE Timbre", false);

    modScrPitchBend = modMatrix.addMonoModSource ("pb", "Pitch Bend", true);

    modSrcNote      = modMatrix.addPolyModSource ("note", "MIDI Note Number", false);
    modSrcVelocity  = modMatrix.addPolyModSource ("vel", "MIDI Velocity", false);

    modSrcMonoLFO   = modMatrix.addMonoModSource ("mlfo", "LFO", true);
    modSrcLFO       = modMatrix.addPolyModSource ("lfo", "LFO", true);
    
    auto firstMonoParam = globalParams.mono;
    bool polyParam = true;
    for (auto pp : getPluginParameters())
    {
        if (pp == firstMonoParam)
            polyParam = false;

        if (! pp->isInternal())
            modMatrix.addParameter (pp, polyParam);
    }

    modMatrix.build();
}

void SynthAudioProcessor::reset()
{
    Processor::reset();

    stereoDelay.reset();
    modLFO.reset();
}

void SynthAudioProcessor::stateUpdated()
{
    modMatrix.stateUpdated (state);
}

void SynthAudioProcessor::updateState()
{
    modMatrix.updateState (state);
}

void SynthAudioProcessor::prepareToPlay (double newSampleRate, int newSamplesPerBlock)
{
    Processor::prepareToPlay (newSampleRate, newSamplesPerBlock);

    setCurrentPlaybackSampleRate (newSampleRate);

    modMatrix.setSampleRate (newSampleRate);

    stereoDelay.setSampleRate (newSampleRate);
    modLFO.setSampleRate (newSampleRate);

    analogTables.setSampleRate (newSampleRate);
}

void SynthAudioProcessor::releaseResources()
{
}

void SynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    if (presetLoaded)
    {
        presetLoaded = false;
        turnOffAllVoices (false);
    }

    startBlock();
    setMPE (globalParams.mpe->isOn());

    playhead = getPlayHead();

    int pos = 0;
    int todo = buffer.getNumSamples();

    buffer.clear();

    setMono (globalParams.mono->isOn());
    setLegato (globalParams.legato->isOn());
    setGlissando (globalParams.glideMode->getProcValue() == 1.0f);
    setPortamento (globalParams.glideMode->getProcValue() == 2.0f);
    setGlideRate (globalParams.glideRate->getProcValue());
    setNumVoices (int (globalParams.voices->getProcValue()));

    while (todo > 0)
    {
        int thisBlock = std::min (todo, 32);

        updateParams (thisBlock);

        renderNextBlock (buffer, midi, pos, thisBlock);

        auto bufferSlice = gin::sliceBuffer (buffer, pos, thisBlock);
        applyEffects (bufferSlice);

        modMatrix.finishBlock (thisBlock);

        pos += thisBlock;
        todo -= thisBlock;
    }

    playhead = nullptr;

    if (buffer.getNumSamples() <= scopeFifo.getFreeSpace())
        scopeFifo.write (buffer);

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

void SynthAudioProcessor::applyEffects (juce::AudioSampleBuffer& buffer)
{
    // Apply Delay
    if (delayParams.enable->isOn())
        stereoDelay.process (buffer);

    // Output gain
    outputGain.process (buffer);
}

void SynthAudioProcessor::updateParams (int newBlockSize)
{
    // Update Mono LFOs
    if (lfoParams.enable->isOn())
    {
        gin::LFO::Parameters params;

        float freq = 0;
        if (lfoParams.sync->getProcValue() > 0.0f)
            freq = 1.0f / gin::NoteDuration::getNoteDurations()[size_t (lfoParams.beat->getProcValue())].toSeconds (playhead);
        else
            freq = modMatrix.getValue (lfoParams.rate);

        params.waveShape = (gin::LFO::WaveShape) int (lfoParams.wave->getProcValue());
        params.frequency = freq;
        params.phase     = modMatrix.getValue (lfoParams.phase);
        params.offset    = modMatrix.getValue (lfoParams.offset);
        params.depth     = modMatrix.getValue (lfoParams.depth);
        params.delay     = 0;
        params.fade      = 0;

        modLFO.setParameters (params);
        modLFO.process (newBlockSize);

        modMatrix.setMonoValue (modSrcMonoLFO, modLFO.getOutput());
    }
    else
    {
        modMatrix.setMonoValue (modSrcMonoLFO, 0);
    }
    
    // Update Delay
    if (delayParams.enable->isOn())
    {
        if (delayParams.sync->isOn())
        {
            auto& duration = gin::NoteDuration::getNoteDurations()[(size_t)delayParams.beat->getUserValueInt()];
            delayParams.delay->setUserValue (duration.toSeconds (getPlayHead()));
        }
        else
        {
            delayParams.delay->setUserValue (delayParams.time->getUserValue());
        }

        stereoDelay.setParams (delayParams.delay->getUserValue(),
                               modMatrix.getValue (delayParams.mix),
                               modMatrix.getValue (delayParams.fb),
                               modMatrix.getValue (delayParams.cf));
    }

    // Output gain
    outputGain.setGain (modMatrix.getValue (globalParams.level));
}

void SynthAudioProcessor::handleMidiEvent (const juce::MidiMessage& m)
{
    MPESynthesiser::handleMidiEvent (m);

    if (m.isPitchWheel())
        modMatrix.setMonoValue (modScrPitchBend, float (m.getPitchWheelValue()) / 0x2000 - 1.0f);
}

//==============================================================================
bool SynthAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SynthAudioProcessor::createEditor()
{
    return new gin::ScaledPluginEditor (new SynthAudioProcessorEditor (*this), state);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthAudioProcessor();
}
