#pragma once

#include "AudioFilterTypes.h"
#include "ParametricCreator.h"
#include <complex>

namespace AudioFilter
{

/*
*   The namespace QBasedButterworth contains free functions to add and create butterworth filters that are based on the Q distribution of these filters.
*   It is possible to use bilinear or improved matched z transform as implementation.
*/
namespace QBasedButterworth
{

void addButterworthPass(BiquadParamCascade& cascade, double freq, bool hipass, size_t order, double sampleRate, FilterImplementation impl);
void addButterworthShelf(BiquadParamCascade& cascade, double freq, double gain, bool hishelf, size_t order, double sampleRate, FilterImplementation impl);
void addButterworthAllpass(BiquadParamCascade& cascade, double freq, size_t order, double sampleRate, FilterImplementation impl);

// A linkwitz reiley filter is simply a cascade of two butterworth filters. This is often used in crossovers as they can be made to add to a flat magnitude response.
void addLinkwitzReiley(BiquadParamCascade& cascade, double freq, bool hipass, size_t order, double sampleRate, FilterImplementation impl);

void createHiLoPass(BiquadParamCascade& params, double freq, bool hipass, size_t order, double sampleRate, FilterImplementation impl);
void createHiLoShelf(BiquadParamCascade& params, double freq, double gain, bool hiShelf, size_t order, double sampleRate, FilterImplementation impl);
void createAllPass(BiquadParamCascade& params, double freq, size_t order, double sampleRate, FilterImplementation impl);

} // namespace QBasedButterworth

/*
*   There are different types of butterworth filters available.
*/
enum ButterworthType
{
    bwLoPass,       // Low pass
    bwHiPass,       // High pass
    bwLowShelf,     // Low shelf
    bwHiShelf,      // High shelf
    bwBandShelf,    // Band shelf (a peak filter with steeper slope)
    bwBandPass,     // Band pass
    bwBandStop,     // Band stop / Notch
    bwAllPass,      // Allpass, flat magnitude response but phase is changed over frequency

    bwNumTypes
};

/*
*   A handy funktion to get a name string for a filter type.
*/
inline std::string getNameForButterworthFilterType(ButterworthType type)
{
    switch (type)
    {
    case bwNumTypes:
    default:
        assert(false);
        return "";
    case bwLoPass:
        return "Low Pass";
    case bwHiPass:
        return "High Pass";
    case bwLowShelf:
        return "Low Shelf";
    case bwHiShelf:
        return "High Shelf";
    case bwBandShelf:
        return "Band Shelf";    
    case bwBandPass:
        return "Bandpass";
    case bwBandStop:
        return "Bandstop";
    case bwAllPass:
        return "Allpass";
    }
}

/*
*   ButterworthCreator is loosely based on butterworth implementaions in DspFilters by Vinnie Falco (https://github.com/vinniefalco/DSPFilters).
*   Other than the Q-based butterworth filters these filters are not available using MZTi.
*/
class ButterworthCreator
{
public:

    // ButterworthCreator shouldn't be copied as it is simply used to create filters.
    ButterworthCreator(const ButterworthCreator&) = delete;
    ButterworthCreator& operator= (const ButterworthCreator&) = delete;

    // The maximum filter order must be given during initialisation to avoid memory allocation at a later time.
    ButterworthCreator(size_t maxFilterOrder);

    // Methods to add different filter types. The parameters will be added to params.
    void addHiLoPass(BiquadParamCascade& params, double freq, bool hipass, size_t order, double sampleRate);
    void addBandPass(BiquadParamCascade& params, double freq, double Q, size_t order, double sampleRate);
    void addBandStop(BiquadParamCascade& params, double freq, double Q, size_t order, double sampleRate);
    void addHiLoShelf(BiquadParamCascade& params, double freq, double gain, bool hiShelf, size_t order, double sampleRate);
    void addBandShelf(BiquadParamCascade& params, double freq, double Q, double gain, size_t order,  double sampleRate);
    void add(BiquadParamCascade& params, ButterworthType type, double freq, double Q, double gain, size_t order, double sampleRate);


    // Methods to create different filter types.
    void createHiLoPass(BiquadParamCascade& params, double freq, bool hipass, size_t order, double sampleRate);
    void createBandPass(BiquadParamCascade& params, double freq, double Q, size_t order, double sampleRate);
    void createBandStop(BiquadParamCascade& params, double freq, double Q, size_t order, double sampleRate);
    void createHiLoShelf(BiquadParamCascade& params, double freq, double gain, bool hiShelf, size_t order, double sampleRate);
    void createBandShelf(BiquadParamCascade& params, double freq, double Q, double gain, size_t order, double sampleRate);
    void create(BiquadParamCascade& params, ButterworthType type, double freq, double Q, double gain, size_t order, double sampleRate);

private:

    // Internal data structures
    struct DualComplex
    {
        DualComplex(const std::complex<double>& c1 = 0., const std::complex<double>& c2 = 0.)
            : x1(c1), x2(c2)
        {
        }
        std::complex<double> x1;
        std::complex<double> x2;
    };

    struct PoleZeroPair
    {
        PoleZeroPair() = default;

        PoleZeroPair(const std::complex<double>& p1, const std::complex<double>& z1, const std::complex<double>& p2, const std::complex<double>& z2)
            :pole1(p1), pole2(p2), zero1(z1), zero2(z2)
        {
        }

        static PoleZeroPair single(const std::complex<double>& pole, const std::complex<double>& zero)
        {
            PoleZeroPair pzp;
            pzp.pole1 = pole;
            pzp.zero1 = zero;
            return pzp;
        }

        static PoleZeroPair conjugated(const std::complex<double>& pole, const std::complex<double>& zero)
        {
            PoleZeroPair pzp;
            pzp.pole1 = pole;
            pzp.zero1 = zero;
            pzp.pole2 = std::conj(pole);
            pzp.zero2 = std::conj(zero);
            return pzp;
        }

        std::complex<double> pole1 = { 0., 0. };
        std::complex<double> pole2 = { 0., 0. };
        std::complex<double> zero1 = { 0., 0. };
        std::complex<double> zero2 = { 0., 0. };

        bool isSinglePole() const
        {
            return pole2 == 0. && zero2 == 0.;
        }
    };

    struct TransformParam
    {
        TransformParam() = default;
        double a = 0;
        double a2 = 0;
        double b = 0;
        double b2 = 0;
        double ab = 0;
    };

    // Analog prototypes
    void createAnalogLowpass(size_t order);
    void createAnalogLowShelf(size_t order, double gainDb);

    // Transformations of analog filters to digital filters.
    void transformToBandpass(size_t order, double minFreq, double maxFreq, double sampleRate);
    void transformToBandstop(size_t order, double minFreq, double maxFreq, double sampleRate);
    void transformToLowpass(size_t order, double freq, double sampleRate);
    void transformToHighpass(size_t order, double freq, double sampleRate);


    // Create biquads from poles/zeros.
    void createBiquads(BiquadParamCascade& params);

    // Transformation helpers.
    static DualComplex bandpassTransform(const std::complex<double>& in, const TransformParam& params);
    static DualComplex bandstopTransform(const std::complex<double>& in, const TransformParam& params);
    static DualComplex lowpassTransform(const std::complex<double>& in, double w);
    static DualComplex highpassTransform(const std::complex<double>& in, double w);

    PreallocatedArray<PoleZeroPair> analogPoleZero;
    PreallocatedArray<PoleZeroPair> digitalPoleZero;
};

/*
*   ButterworthFilterParams is a helper class that stores user parameters (frequency, gain, order etc.) and only creates a new
*   set of biquad parameters if the user parameters change.
*   The filters can be generated using QBasedButterworth as well as ButterworthCreator.
*/
class ButterworthFilterParams
{
public:

    // ButterworthFilterParams shouldn't be copied as it is simply used to create biquad parameters.
    ButterworthFilterParams(const ButterworthFilterParams&) = delete;
    ButterworthFilterParams& operator= (const ButterworthFilterParams&) = delete;

    ButterworthFilterParams(size_t maxOrder)
        : biquadCascade(static_cast<int> (maxOrder)), creator(maxOrder)
    {
    }

    // Sets whether BLT or MZTi is used, although the latter is only available with Q-based filters.
    void setType(FilterImplementation newImpl)
    {
        impl = newImpl;
    }

    // Returns whether Q-based filters are used.
    bool getQBased() const
    {
        return useQBasedButterworth;
    }

    // Sets whether Q-based filters should be used.
    void setQBased(bool qBased)
    {
        useQBasedButterworth = qBased;
    }

    // Sets the filter parameters in cascade, but only if one of the given parameters is changed. This of course only works if cascade won't be changed elsewhere
    // as the class doesn't know about the changes otherwise.
    // Returns true in case something changed which can be used to update the changed parameters in a FilterInstance.
    bool set(BiquadParamCascade& cascade, double newFreq, double newQ, double newGain, size_t newOrder, ButterworthType newType, double newSampleRate)
    {
        if (! updateNeeded(newFreq, newQ, newGain, newOrder, newType, newSampleRate))
            return false;

        switch (newType)
        {
        default:
            assert(false);
            break;
        case bwLoPass:
            if (useQBasedButterworth)
                QBasedButterworth::createHiLoPass(cascade, freq, false, order, sampleRate, impl);
            else
                creator.createHiLoPass(cascade, freq, false, order, sampleRate);
            break;
        case bwHiPass:
            if (useQBasedButterworth)
                QBasedButterworth::createHiLoPass(cascade, freq, true, order, sampleRate, impl);
            else
                creator.createHiLoPass(cascade, freq, true, order, sampleRate);
            break;
        case bwLowShelf:
            if (useQBasedButterworth)
                QBasedButterworth::createHiLoShelf(cascade, freq, gain, false, order, sampleRate, impl);
            else
                creator.createHiLoShelf(cascade, freq, gain, false, order, sampleRate);
            break;
        case bwHiShelf:
            if (useQBasedButterworth)
                QBasedButterworth::createHiLoShelf(cascade, freq, gain, true, order, sampleRate, impl);
            else
                creator.createHiLoShelf(cascade, freq, gain, true, order, sampleRate);
            break;
        case bwBandShelf:
            creator.createBandShelf(cascade, freq, Q, gain, order, sampleRate);
            break;
        case bwBandPass:
            creator.createBandPass(cascade, freq, Q, order, sampleRate);
            break;
        case bwBandStop:
            creator.createBandStop(cascade, freq, Q, order, sampleRate);
            break;            
        case bwAllPass:
            QBasedButterworth::createAllPass(cascade, freq, order, sampleRate, impl);
            break;
        }

        return true;
    }

    // Sets the internal set of parameters.
    void set(double newFreq, double newQ, double newGain, size_t newOrder, ButterworthType newType, double newSampleRate)
    {
        set(biquadCascade, newFreq, newQ, newGain, newOrder, newType, newSampleRate);
    }

    BiquadParamCascade biquadCascade;

private:

    // Checks if any parameter has changed and updates the internal values if needed.
    bool updateNeeded(double newFreq, double newQ, double newGain, size_t newOrder, ButterworthType newType, double newSampleRate)
    {
        if (newFreq == freq && Q == newQ && newGain == gain && newOrder == order && newType == type && newSampleRate == sampleRate)
            return false;

        freq = newFreq;
        Q = newQ;
        gain = newGain;
        order = newOrder;
        type = newType;
        sampleRate = newSampleRate;
        return true;
    }

    ButterworthCreator creator;

    double freq = 0;
    double Q = 0;
    double gain = 0;
    size_t order = 2;
    ButterworthType type;
    double sampleRate;
    bool useQBasedButterworth = false;
    FilterImplementation impl = filterBLT;
};

} // namespace AudioFilter
