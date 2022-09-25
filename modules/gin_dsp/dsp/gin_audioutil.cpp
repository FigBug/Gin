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
