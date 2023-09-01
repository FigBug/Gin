/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

void AudioFunctionHost::setSampleRate (double sr)
{
    sampleRate = sr;
    funcStates.clear();
}

void AudioFunctionHost::addUtilities (gin::EquationParser& parser)
{
    parser.addFunction ("midiToHz", [] (int, double noteNumber)
                        {
                            return gin::getMidiNoteInHertz (noteNumber);
                        });
}

void AudioFunctionHost::addConstants (gin::EquationParser& parser)
{
    parser.addConstant ("_phi", 1.61803398875); // golden ratio
    parser.addConstant ("_PHI", 0.6180339887); // golden ratio conjugate
    parser.addConstant ("_deltaS", 2.41421356237); // silver ratio
    parser.addConstant ("_rho", 1.324717957244746025960908854); // plastic number
    parser.addConstant ("_q", 0.70710678118655); // default q
}

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
    parser.addFunction ("noise", [this] (int id, double)
                        {
                            auto p = getFuncParams<NoiseState> (id, sampleRate);
                            return p->process();
                        });
}

void AudioFunctionHost::addSynthFilterFunctions (gin::EquationParser& parser)
{
    parser.addFunction ("hp12", [this] (int id, double v, double freq, double res)
                        {
                            auto p = getFuncParams<HP12State> (id, sampleRate);
                            float q = Q / (1.0f - float (res) * 0.99f);
                            return p->process (v, freq, q);
                        });
    parser.addFunction ("lp12", [this] (int id, double v, double freq, double res)
                        {
                            auto p = getFuncParams<LP12State> (id, sampleRate);
                            float q = Q / (1.0f - float (res) * 0.99f);
                            return p->process (v, freq, q);
                        });
    parser.addFunction ("notch12", [this] (int id, double v, double freq, double res)
                        {
                            auto p = getFuncParams<Notch12State> (id, sampleRate);
                            float q = Q / (1.0f - float (res) * 0.99f);
                            return p->process (v, freq, q);
                        });
    parser.addFunction ("bp12", [this] (int id, double v, double freq, double res)
                        {
                            auto p = getFuncParams<BP12State> (id, sampleRate);
                            float q = Q / (1.0f - float (res) * 0.99f);
                            return p->process (v, freq, q);
                        });
    
    parser.addFunction ("hp24", [this] (int id, double v, double freq, double res)
                        {
                            auto p = getFuncParams<HP24State> (id, sampleRate);
                            float q = Q / (1.0f - float (res) * 0.99f);
                            return p->process (v, freq, q);
                        });
    parser.addFunction ("lp24", [this] (int id, double v, double freq, double res)
                        {
                            auto p = getFuncParams<LP24State> (id, sampleRate);
                            float q = Q / (1.0f - float (res) * 0.99f);
                            return p->process (v, freq, q);
                        });
    parser.addFunction ("notch24", [this] (int id, double v, double freq, double res)
                        {
                            auto p = getFuncParams<Notch24State> (id, sampleRate);
                            float q = Q / (1.0f - float (res) * 0.99f);
                            return p->process (v, freq, q);
                        });
    parser.addFunction ("bp24", [this] (int id, double v, double freq, double res)
                        {
                            auto p = getFuncParams<BP24State> (id, sampleRate);
                            float q = Q / (1.0f - float (res) * 0.99f);
                            return p->process (v, freq, q);
                        });
}

void AudioFunctionHost::addEffectFilterFunctions (gin::EquationParser& parser)
{
    parser.addFunction ("hp12", [this] (int id, double v, double freq, double q)
                        {
                            auto p = getFuncParams<HP12State> (id, sampleRate);
                            return p->process (v, freq, q);
                        });
    parser.addFunction ("lp12", [this] (int id, double v, double freq, double q)
                        {
                            auto p = getFuncParams<LP12State> (id, sampleRate);
                            return p->process (v, freq, q);
                        });
    parser.addFunction ("notch12", [this] (int id, double v, double freq, double q)
                        {
                            auto p = getFuncParams<Notch12State> (id, sampleRate);
                            return p->process (v, freq, q);
                        });
    parser.addFunction ("bp12", [this] (int id, double v, double freq, double q)
                        {
                            auto p = getFuncParams<BP12State> (id, sampleRate);
                            return p->process (v, freq, q);
                        });
    
    parser.addFunction ("hp24", [this] (int id, double v, double freq, double q)
                        {
                            auto p = getFuncParams<HP24State> (id, sampleRate);
                            return p->process (v, freq, q);
                        });
    parser.addFunction ("lp24", [this] (int id, double v, double freq, double q)
                        {
                            auto p = getFuncParams<LP24State> (id, sampleRate);
                            return p->process (v, freq, q);
                        });
    parser.addFunction ("notch24", [this] (int id, double v, double freq, double q)
                        {
                            auto p = getFuncParams<Notch24State> (id, sampleRate);
                            return p->process (v, freq, q);
                        });
    parser.addFunction ("bp24", [this] (int id, double v, double freq, double q)
                        {
                            auto p = getFuncParams<BP24State> (id, sampleRate);
                            return p->process (v, freq, q);
                        });
}
