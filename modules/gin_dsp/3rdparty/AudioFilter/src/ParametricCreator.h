#pragma once

#include "AudioFilterTypes.h"
#include <string>

namespace AudioFilter
{

//==============================================================================
/*
*   Parametric filter types. These can all be realised using one biquad.
*/
enum FilterType
{
    afPeak,         // Peak filter
    afLoShelf,      // 2nd order low shelf
    afHiShelf,      // 2nd order high shelf
    afLoPass,       // 2nd order low pass
    afHiPass,       // 2nd order high pass
    afLoPass6,      // 1st order low pass with Q-factor; setting Q <= 0 results in a classic 1st order low pass
    afHiPass6,      // 1st order high pass with Q-factor; setting Q <= 0 results in a classic 1st order high pass
    afLoShelf6,     // 1st order low shelf
    afHiShelf6,     // 1st order high shelf
    afAllPass,      // 2nd order allpass
    afAllPass6,     // 1st order allpass
    afBandPass,     // bandpass filter
    afNotch,        // notch (bandstop) filter

    afNumTypes
};

/*
*   A handy function to get a name string for a filter type.
*/
inline std::string getNameForFilterType(FilterType type)
{
    switch (type)
    {
    case afNumTypes:
    default:
        return "";
    case afPeak:
        return "Peak";
    case afLoShelf:
        return "Low Shelf";
    case afHiShelf:
        return "High Shelf";
    case afLoPass:
        return "Low Pass";
    case afHiPass:
        return "High Pass";
    case afLoPass6:
        return "Low Pass 6dB";
    case afHiPass6:
        return "High Pass 6 dB";
    case afLoShelf6:
        return "Low Shelf 6dB";
    case afHiShelf6:
        return "High Shelf 6 dB";
    case afAllPass6:
        return "Allpass 6 dB";
    case afAllPass:
        return "Allpass";
    case afBandPass:
        return "Bandpass";
    case afNotch:
        return "Notch";
    }
}

/*
*   The filter implementation which can either be bilinear transform or matched z transform improved (MZTi). The latter is based on the paper 
*   "Optimizing the Magnitude Response of Matched Z-Transform Filters ('MZTI') for Loudspeaker Equalization" by David W. Guiness and Ojas S. Chauhan from 2007.
*/
enum FilterImplementation
{
    filterMZTi,     // Matched Z Transform, improved
    filterBLT       // Bilinear transform
};

/*
*   The namespace ParametricCreator contains free functions to create a parametric EQ stage.
*/
namespace ParametricCreator
{

void createMZTiStage(BiquadParam& param, double freq, double gain, double Q, FilterType type, double sampleRate);
void createBLTStage(BiquadParam& param, double freq, double gain, double Q, FilterType type, double sampleRate);
void createStage(BiquadParam& param, double freq, double gain, double Q, FilterType type, double sampleRate, FilterImplementation impl);

} //ParametricCreator


/*
*   ParametricFilterParams is a helper class that stores user parameters (frequency, Q-factor, etc.) and only creates a new 
*   set of biquad parameters if the user parameters change.
*/
class ParametricFilterParams
{
public:

    // Sets wether the biquad parameters are created using bilinear transform or improved matched-z transform.
    void setType(FilterImplementation newImpl)
    {        
        impl = newImpl;
    }

    // This sets the biquadParams only if any of the given user parameters is changed. If no user parameter has changed the method returns false;
    bool set(BiquadParam& biquadParams, double newFreq, double newGain, double newQ, FilterType newFilterType, double newSampleRate)
    {
        if (! updateIfNeeded(newFreq, newGain, newQ, newFilterType, newSampleRate))
            return false;

        ParametricCreator::createStage(biquadParams, freq, gain, Q, filterType, sampleRate, impl);
        return true;
    }

private:

    // Checks if a parameter has changed.
    bool updateIfNeeded(double newFreq, double newGain, double newQ, FilterType newFilterType, double newSampleRate)
    {
        if (newFreq == freq && newGain == gain && newQ == Q && newFilterType == filterType && newSampleRate == sampleRate)
            return false;

        freq = newFreq;
        gain = newGain;
        Q = newQ;
        filterType = newFilterType;
        sampleRate = newSampleRate;
        return true;
    }

    double freq = 0;
    double Q = 0;
    double gain = 0;
    double sampleRate = 0;
    FilterType filterType = afPeak;
    FilterImplementation impl = filterMZTi;
};


} // namespace AudioFilter
