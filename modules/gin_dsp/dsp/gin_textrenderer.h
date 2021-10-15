/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2021 - Roland Rabien.

 ==============================================================================
 */


#pragma once

/** Render audio to ascii waveforms
*/
class TextRenderer
{
public:
    static juce::String renderBuffer (const juce::AudioSampleBuffer& buffer, int height, float samplesPerChar);

   #if JUCE_DEBUG
    static void debugPrintBuffer (const juce::AudioSampleBuffer& buffer, int height, float samplesPerChar);
   #endif
};
