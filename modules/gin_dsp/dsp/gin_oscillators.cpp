/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
void StereoOscillator::noteOn (float p)
{
    phase = p >= 0 ? p : juce::Random::getSystemRandom().nextFloat();
}
