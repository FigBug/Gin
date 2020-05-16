/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

/** Enhances the EquationParser with math related
    functions like filters and oscilllators
*/
class AudioEquationParser : public EquationParser,
                            public AudioFunctionHost
{
public:
    AudioEquationParser() = default;
    
    void addConstants();
    void addUtilities();
    void addOscillatorFunctions();
    void addSynthFilterFunctions();
    void addEffectFilterFunctions();
};
