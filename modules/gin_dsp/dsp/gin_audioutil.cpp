/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
void applyGain (juce::AudioSampleBuffer& buffer, juce::LinearSmoothedValue<float>& gain)
{
    if (gain.isSmoothing())
    {
        if (auto w = buffer.getArrayOfWritePointers())
        {
            for (int s = 0; s < buffer.getNumSamples(); s++)
            {
                float g = gain.getNextValue();
                for (int c = 0; c < buffer.getNumChannels(); c++)
                    w[c][s] *= g;
            }
        }
    }
    else
    {
        buffer.applyGain (gain.getTargetValue());
    }
}

void applyGain (juce::AudioSampleBuffer& buffer, int channel, juce::LinearSmoothedValue<float>& gain)
{
    if (gain.isSmoothing())
    {
        if (float* w = buffer.getWritePointer (channel))
            for (int s = 0; s < buffer.getNumSamples(); s++)
                w[s] *= gain.getNextValue();
    }
    else
    {
        buffer.applyGain (channel, 0, buffer.getNumSamples(), gain.getTargetValue());
    }
}

void clip (juce::AudioSampleBuffer& buffer, float low, float high)
{
    if (auto w = buffer.getArrayOfWritePointers())
    {
        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            for (int c = 0; c < buffer.getNumChannels(); c++)
                w[c][s] = juce::jlimit (low, high, w[c][s]);
        }
    }
}

//==============================================================================
juce::String getMidiMessageType (const juce::MidiMessage& msg)
{
    if (msg.isNoteOn()) return "Note On";
    if (msg.isNoteOff()) return "Note Off";
    if (msg.isSysEx()) return "SysEx";
    if (msg.isProgramChange()) return "Program Change";
    if (msg.isPitchWheel()) return "Pitch Wheel";
    if (msg.isAftertouch()) return "Aftertouch";
    if (msg.isChannelPressure()) return "Channel Pressure";
    if (msg.isController()) return "Controller";
    if (msg.isAllNotesOff()) return "All Notes Off";
    if (msg.isAllSoundOff()) return "All Sound Off";
    if (msg.isMetaEvent()) return "Meta";
    if (msg.isTrackMetaEvent()) return "Track Meta";
    if (msg.isEndOfTrackMetaEvent()) return "End of Track Meta";
    if (msg.isTrackNameEvent()) return "Track Name";
    if (msg.isTextMetaEvent()) return "Text Meta";
    if (msg.isTempoMetaEvent()) return "Tempo Meta";
    if (msg.isTimeSignatureMetaEvent()) return "Time Sig";
    if (msg.isKeySignatureMetaEvent()) return "Key Sig";
    if (msg.isMidiChannelMetaEvent()) return "Midi Channel";
    if (msg.isActiveSense()) return "Active Sense";
    if (msg.isMidiStart()) return "Midi Start";
    if (msg.isMidiStop()) return "Midi Stop";
    if (msg.isMidiClock()) return "Midi Clock";
    if (msg.isSongPositionPointer()) return "Song Pos";
    if (msg.isQuarterFrame()) return "Quarter Frame";
    if (msg.isFullFrame()) return "Full Frame";
    if (msg.isMidiMachineControlMessage()) return "MMC";

    return "Unknown";
}

//==============================================================================
#if JUCE_UNIT_TESTS

class AudioUtilTests : public juce::UnitTest
{
public:
    AudioUtilTests() : juce::UnitTest ("Audio Utilities", "gin_dsp") {}

    void runTest() override
    {
        testApplyGain();
        testApplyGainSingleChannel();
        testClip();
        testMidiMessageType();
    }

private:
    void testApplyGain()
    {
        beginTest ("Apply Gain");

        juce::AudioBuffer<float> buffer (2, 100);
        buffer.clear();

        // Fill with test values
        for (int c = 0; c < buffer.getNumChannels(); c++)
            for (int s = 0; s < buffer.getNumSamples(); s++)
                buffer.setSample (c, s, 0.5f);

        // Test non-smoothing gain
        juce::LinearSmoothedValue<float> gain;
        gain.setCurrentAndTargetValue (2.0f);

        applyGain (buffer, gain);

        expectWithinAbsoluteError (buffer.getSample (0, 0), 1.0f, 0.001f,
                                   "Gain of 2x should double the signal");
        expectWithinAbsoluteError (buffer.getSample (1, 50), 1.0f, 0.001f,
                                   "All samples should be affected");

        // Test smoothing gain
        buffer.clear();
        for (int c = 0; c < buffer.getNumChannels(); c++)
            buffer.setSample (c, 0, 1.0f);

        gain.reset (44100.0, 0.1);
        gain.setCurrentAndTargetValue (1.0f);
        gain.setTargetValue (0.5f);

        expect (gain.isSmoothing(), "Should be smoothing");
        applyGain (buffer, gain);

        // First sample should be close to 1.0 (initial gain)
        expectWithinAbsoluteError (buffer.getSample (0, 0), 1.0f, 0.1f,
                                   "First sample should have initial gain");
    }

    void testApplyGainSingleChannel()
    {
        beginTest ("Apply Gain Single Channel");

        juce::AudioBuffer<float> buffer (2, 100);
        buffer.clear();

        // Fill channel 0 with 0.5, channel 1 with 0.8
        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            buffer.setSample (0, s, 0.5f);
            buffer.setSample (1, s, 0.8f);
        }

        juce::LinearSmoothedValue<float> gain;
        gain.setCurrentAndTargetValue (2.0f);

        // Apply gain only to channel 0
        applyGain (buffer, 0, gain);

        expectWithinAbsoluteError (buffer.getSample (0, 0), 1.0f, 0.001f,
                                   "Channel 0 should be affected");
        expectWithinAbsoluteError (buffer.getSample (1, 0), 0.8f, 0.001f,
                                   "Channel 1 should be unchanged");
    }

    void testClip()
    {
        beginTest ("Clip");

        juce::AudioBuffer<float> buffer (2, 10);
        buffer.clear();

        // Fill with values that exceed limits
        buffer.setSample (0, 0, 1.5f);
        buffer.setSample (0, 1, -1.5f);
        buffer.setSample (0, 2, 0.5f);
        buffer.setSample (1, 0, 2.0f);
        buffer.setSample (1, 1, -2.0f);

        clip (buffer, -1.0f, 1.0f);

        expectWithinAbsoluteError (buffer.getSample (0, 0), 1.0f, 0.001f,
                                   "Should clip high values");
        expectWithinAbsoluteError (buffer.getSample (0, 1), -1.0f, 0.001f,
                                   "Should clip low values");
        expectWithinAbsoluteError (buffer.getSample (0, 2), 0.5f, 0.001f,
                                   "Should not affect values within range");
        expectWithinAbsoluteError (buffer.getSample (1, 0), 1.0f, 0.001f,
                                   "Should clip in all channels");
    }

    void testMidiMessageType()
    {
        beginTest ("MIDI Message Type");

        expect (getMidiMessageType (juce::MidiMessage::noteOn (1, 60, 0.5f)) == "Note On",
                "Should identify Note On");
        expect (getMidiMessageType (juce::MidiMessage::noteOff (1, 60)) == "Note Off",
                "Should identify Note Off");
        expect (getMidiMessageType (juce::MidiMessage::programChange (1, 42)) == "Program Change",
                "Should identify Program Change");
        expect (getMidiMessageType (juce::MidiMessage::pitchWheel (1, 8192)) == "Pitch Wheel",
                "Should identify Pitch Wheel");
        expect (getMidiMessageType (juce::MidiMessage::aftertouchChange (1, 60, 100)) == "Aftertouch",
                "Should identify Aftertouch");
        expect (getMidiMessageType (juce::MidiMessage::channelPressureChange (1, 100)) == "Channel Pressure",
                "Should identify Channel Pressure");
        expect (getMidiMessageType (juce::MidiMessage::controllerEvent (1, 7, 100)) == "Controller",
                "Should identify Controller");
        expect (getMidiMessageType (juce::MidiMessage::allNotesOff (1)) == "All Notes Off",
                "Should identify All Notes Off");
        expect (getMidiMessageType (juce::MidiMessage::allSoundOff (1)) == "All Sound Off",
                "Should identify All Sound Off");
        expect (getMidiMessageType (juce::MidiMessage::midiStart()) == "Midi Start",
                "Should identify MIDI Start");
        expect (getMidiMessageType (juce::MidiMessage::midiStop()) == "Midi Stop",
                "Should identify MIDI Stop");
        expect (getMidiMessageType (juce::MidiMessage::midiClock()) == "Midi Clock",
                "Should identify MIDI Clock");
    }
};

static AudioUtilTests audioUtilTests;

#endif
