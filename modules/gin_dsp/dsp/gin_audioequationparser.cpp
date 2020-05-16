/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

void AudioEquationParser::addConstants()
{
    AudioFunctionHost::addConstants (*this);
}

void AudioEquationParser::addUtilities()
{
    AudioFunctionHost::addUtilities (*this);
}

void AudioEquationParser::addOscillatorFunctions()
{
    AudioFunctionHost::addOscillatorFunctions (*this);
}

void AudioEquationParser::addSynthFilterFunctions()
{
    AudioFunctionHost::addSynthFilterFunctions (*this);
}

void AudioEquationParser::addEffectFilterFunctions()
{
    AudioFunctionHost::addEffectFilterFunctions (*this);
}
