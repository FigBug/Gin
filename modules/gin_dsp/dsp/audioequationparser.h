/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

class AudioEquationParser : public EquationParser,
                            public AudioFunctionHost
{
public:
    AudioEquationParser() = default;
    
    void addConstants();
    void addOscillatorFunctions();
    void addSynthFilterFunctions();
    void addEffectFilterFunctions();
};
