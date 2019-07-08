/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#include "adsr.h"

void AudioFunctionHost::addOscillatorFunctions (gin::EquationParser& parser)
{
    parser.addFunction ("sine", [this] (int id, double note)
                        {
                            auto p = getFuncParams<OscState> (id, sampleRate);
                            p->incPhase (float (note));
                            return lookupTables->processSine (p->phase);
                        });
    parser.addFunction ("saw", [this] (int id, double note)
                        {
                            auto p = getFuncParams<OscState> (id, sampleRate);
                            p->incPhase (float (note));
                            return lookupTables->processSawUp (float (note), p->phase);
                        });
    parser.addFunction ("sawdown", [this] (int id, double note)
                        {
                            auto p = getFuncParams<OscState> (id, sampleRate);
                            p->incPhase (float (note));
                            return lookupTables->processSawDown (float (note), p->phase);
                        });
    parser.addFunction ("pulse", [this] (int id, double note, double pw)
                        {
                            auto p = getFuncParams<OscState> (id, sampleRate);
                            p->incPhase (float (note));
                            return lookupTables->processPulse (float (note), p->phase, float (pw));
                        });
    parser.addFunction ("square", [this] (int id, double note)
                        {
                            auto p = getFuncParams<OscState> (id, sampleRate);
                            p->incPhase (float (note));
                            return lookupTables->processSquare (float (note), p->phase);
                        });
    parser.addFunction ("triangle", [this] (int id, double note)
                        {
                            auto p = getFuncParams<OscState> (id, sampleRate);
                            p->incPhase (float (note));
                            return lookupTables->processTriangle (float (note), p->phase);
                        });
    parser.addFunction ("noise", [] (int, double)
                        {
                            return noise();
                        });
}

void AudioFunctionHost::addFilterFunctions (gin::EquationParser& parser)
{
    parser.addFunction ("hp", [this] (int id, double v, double freq, double res)
                        {
                            auto p = getFuncParams<HPState> (id, sampleRate);
                            float q = 0.70710678118655f / (1.0f - float (res) * 0.99f);
                            return p->process (v, freq, q);
                        });
    parser.addFunction ("lp", [this] (int id, double v, double freq, double res)
                        {
                            auto p = getFuncParams<LPState> (id, sampleRate);
                            float q = 0.70710678118655f / (1.0f - float (res) * 0.99f);
                            return p->process (v, freq, q);
                        });
    parser.addFunction ("notch", [this] (int id, double v, double freq, double res)
                        {
                            auto p = getFuncParams<NotchState> (id, sampleRate);
                            float q = 0.70710678118655f / (1.0f - float (res) * 0.99f);
                            return p->process (v, freq, q);
                        });
    parser.addFunction ("bp", [this] (int id, double v, double freq, double res)
                        {
                            auto p = getFuncParams<BPState> (id, sampleRate);
                            float q = 0.70710678118655f / (1.0f - float (res) * 0.99f);
                            return p->process (v, freq, q);
                        });
}
