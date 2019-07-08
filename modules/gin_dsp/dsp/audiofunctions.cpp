/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#include "adsr.h"

void AudioFunctionHost::addOscillatorFunctions (gin::EquationParser& p)
{
    p.addFunction ("sine", [this] (int id, double note)
                   {
                       auto p = getFuncParams<OscState> (id, sampleRate);
                       p->incPhase (float (note));
                       return lookupTables->processSine (p->phase);
                   });
    p.addFunction ("saw", [this] (int id, double note)
                   {
                       auto p = getFuncParams<OscState> (id, sampleRate);
                       p->incPhase (float (note));
                       return lookupTables->processSawUp (float (note), p->phase);
                   });
    p.addFunction ("sawdown", [this] (int id, double note)
                   {
                       auto p = getFuncParams<OscState> (id, sampleRate);
                       p->incPhase (float (note));
                       return lookupTables->processSawDown (float (note), p->phase);
                   });
    p.addFunction ("pulse", [this] (int id, double note, double pw)
                   {
                       auto p = getFuncParams<OscState> (id, sampleRate);
                       p->incPhase (float (note));
                       return lookupTables->processPulse (float (note), p->phase, float (pw));
                   });
    p.addFunction ("square", [this] (int id, double note)
                   {
                       auto p = getFuncParams<OscState> (id, sampleRate);
                       p->incPhase (float (note));
                       return lookupTables->processSquare (float (note), p->phase);
                   });
    p.addFunction ("triangle", [this] (int id, double note)
                   {
                       auto p = getFuncParams<OscState> (id, sampleRate);
                       p->incPhase (float (note));
                       return lookupTables->processTriangle (float (note), p->phase);
                   });
    p.addFunction ("noise", [] (int, double)
                   {
                       return noise();
                   });
}

void AudioFunctionHost::addFilterFunctions (gin::EquationParser& p)
{
    p.addFunction ("hp", [this] (int id, double v, double freq, double res)
                   {
                       auto p = getFuncParams<HPState> (id, sampleRate);
                       float q = 0.70710678118655f / (1.0f - float (res) * 0.99f);
                       return p->process (v, freq, q);
                   });
    p.addFunction ("lp", [this] (int id, double v, double freq, double res)
                   {
                       auto p = getFuncParams<LPState> (id, sampleRate);
                       float q = 0.70710678118655f / (1.0f - float (res) * 0.99f);
                       return p->process (v, freq, q);
                   });
    p.addFunction ("notch", [this] (int id, double v, double freq, double res)
                   {
                       auto p = getFuncParams<NotchState> (id, sampleRate);
                       float q = 0.70710678118655f / (1.0f - float (res) * 0.99f);
                       return p->process (v, freq, q);
                   });
    p.addFunction ("bp", [this] (int id, double v, double freq, double res)
                   {
                       auto p = getFuncParams<BPState> (id, sampleRate);
                       float q = 0.70710678118655f / (1.0f - float (res) * 0.99f);
                       return p->process (v, freq, q);
                   });
}
