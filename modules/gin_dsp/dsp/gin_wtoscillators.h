/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


//==============================================================================
/** WT Oscillator.
*/
class WTOscillator
{
public:
    WTOscillator() = default;
    
    struct Params
    {
        float leftGain = 1.0;
        float rightGain = 1.0;
    };
    
    void setSampleRate (double sr)  { sampleRate = sr; }
    void noteOn (float p = -1);
    
    void process (float note, float table, const Params& params, AudioSampleBuffer& buffer);
    void process (float noteL, float noteR, float table, const Params& params, AudioSampleBuffer& buffer);

    void processAdding (float note, float table, const Params& params, AudioSampleBuffer& buffer);
    void processAdding (float noteL, float noteR, float table, const Params& params, AudioSampleBuffer& buffer);
    
private:
    OwnedArray<BandLimitedLookupTable> bllt;
    double sampleRate = 44100.0;
    float phaseL = 0.0f, phaseR = 0.0f;
};
