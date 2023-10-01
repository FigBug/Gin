/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
void StereoOscillator::noteOn (float p)
{
    p >= 0 ? phase = p : juce::Random::getSystemRandom().nextFloat();
}
