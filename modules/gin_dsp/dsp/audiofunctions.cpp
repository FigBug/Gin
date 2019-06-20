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
                       p->incPhase (note);
                       return lookupTables->processSine (p->phase);
                   });
    p.addFunction ("saw", [this] (int id, double note)
                   {
                       auto p = getFuncParams<OscState> (id, sampleRate);
                       p->incPhase (note);
                       return lookupTables->processSawUp (note, p->phase);
                   });
    p.addFunction ("sawdown", [this] (int id, double note)
                   {
                       auto p = getFuncParams<OscState> (id, sampleRate);
                       p->incPhase (note);
                       return lookupTables->processSawDown (note, p->phase);
                   });
    p.addFunction ("pulse", [this] (int id, double note, double pw)
                   {
                       auto p = getFuncParams<OscState> (id, sampleRate);
                       p->incPhase (note);
                       return lookupTables->processPulse (note, p->phase, pw);
                   });
    p.addFunction ("square", [this] (int id, double note)
                   {
                       auto p = getFuncParams<OscState> (id, sampleRate);
                       p->incPhase (note);
                       return lookupTables->processSquare (note, p->phase);
                   });
    p.addFunction ("triangle", [this] (int id, double note)
                   {
                       auto p = getFuncParams<OscState> (id, sampleRate);
                       p->incPhase (note);
                       return lookupTables->processTriangle (note, p->phase);
                   });
    p.addFunction ("noise", [this] (int, double note)
                   {
                       return noise();
                   });
}

void AudioFunctionHost::addFilterFunctions (gin::EquationParser& p)
{
    p.addFunction ("hp", [this] (int id, double v, double freq, double res)
                   {
                       auto p = getFuncParams<HPState> (id, sampleRate);
                       float q = 0.70710678118655f / (1.0f - (res) * 0.99f);
                       return p->process (v, freq, q);
                   });
    p.addFunction ("lp", [this] (int id, double v, double freq, double res)
                   {
                       auto p = getFuncParams<LPState> (id, sampleRate);
                       float q = 0.70710678118655f / (1.0f - (res) * 0.99f);
                       return p->process (v, freq, q);
                   });
    p.addFunction ("notch", [this] (int id, double v, double freq, double res)
                   {
                       auto p = getFuncParams<NotchState> (id, sampleRate);
                       float q = 0.70710678118655f / (1.0f - (res) * 0.99f);
                       return p->process (v, freq, q);
                   });
    p.addFunction ("bp", [this] (int id, double v, double freq, double res)
                   {
                       auto p = getFuncParams<BPState> (id, sampleRate);
                       float q = 0.70710678118655f / (1.0f - (res) * 0.99f);
                       return p->process (v, freq, q);
                   });
}
