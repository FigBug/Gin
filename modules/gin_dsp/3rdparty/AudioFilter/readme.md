# C++ AudioFilter

AudioFilter supplies C++ classes and functions to create and process filters for audio applications. There are no other dependancies beside the STL. Also, a C++11 compatible compiler is needed.

## Overview

In general, to process a filter one needs a set of parameters as well as a processing instance holding the processing state. This separation is useful as in the common case of stereo processing only one parameter set is used for two (left and right) channels.

The library provides "normal" first/second order filters (peak, shelves, high/low passes) and higher order butterworth filters. To avoid cramping at frequency settings near nyquist filters can be created using "MZTi" as described in the paper "Optimizing the Magnitude Response of Matched Z-Transform Filters ('MZTI') for Loudspeaker Equalization" by David W. Guiness and Ojas S. Chauhan.

Processing is done using a state variable filter implementation which allows for lower parameter quantisation as well as the ability to tune filter paramters without audio glitches.

### Features

* 1st order filters (cuts, shelves)
* 2nd order parametric filters (cuts, shelves, peak, allpass, bandpass, notch) 
* arbitrary order butterworth filters (cuts, shelves, bandshelves, bandpass, notch)
* BLT as well as MZTi based parameters, the latter allowing for nyquist-decramping

## Library Structure

The library is split into several files:

* **AudioFilterTypes**: Data structures for biquads and the processing implementation. Also an implementation for a preallocated array to avoid memory allocation when changing filter order.
* **FilterInstance**: Functions for processing a filter as well as a class to easily handle basic filters.
* **ParametricCreator**: Functions for creating first and second order parametric filters. Also a helper class that stores eq parameter (frequency etc.) and can update biquad paramters only if filter paramters have changed.
* **ButterworthCreator**: There are two implementations for butterworth filters available. QBasedButterworth only provides high- and lowpass filters but on the other hand allows to use a MZTi implementation resulting in a better frequency response near nyquist. ButterworthCreator on the other hand is based on Vinnie Falco's work in the dspfilters library and also can be used for bandshelf filters. As it makes use of various intermediate variables for calculating the filter coefficients it is implemented as a class to avoid allocation during setting up a filter.
* **Response** provides functions to get the filters frequency magnitude response.

## Examples

Basic first/second order filter:

```
#include "../../audiofilter/src/ParametricCreator.h"
#include "../../audiofilter/src/FilterInstance.h"

AudioFilter::FilterParams<float> filterParams;
AudioFilter::FilterState<float> filterState;

void setFilter(double freq, double q, double gain, AudioFilter::FilterType type, double sampleRate)
{
    // Temporary biquad params
    AudioFilter::BiquadParam bqParams;
    // Setup biquad params
    AudioFilter::ParametricCreator::createMZTiStage(bqParams, freq, gain, Q, type, sampleRate);
    // Transform into SVF parameters
    filterParams.setFromBiquadParams(bqParams);
}

void process(float* out, const float* in, int numSamples)
{
    AudioFilter::processFilter(filterState, filterParams, out, in, numSamples);
}
```

Butterworth filter:

```
#include "../../audiofilter/src/ButterworthCreator.h"
#include "../../audiofilter/src/FilterInstance.h"

static constexpr int maxOrder = 8;
AudioFilter::BiquadParamCascade bqParams = AudioFilter::BiquadParamCascade(maxOrder);
AudioFilter::FilterParamsCascade<float> filterParams = AudioFilter::FilterParamsCascade<float>(maxOrder);
AudioFilter::FilterStateCascade<float> filterStates = AudioFilter::FilterStateCascade<float>(maxOrder);
AudioFilter::ButterworthCreator bwCreator = AudioFilter::ButterworthCreator(maxOrder);

void setFilter(double freq, double q, double gain, size_t order, double sampleRate)
{
    // create biquad params
    bwCreator.createBandShelf(bqParams, freq, Q, gain, order, sampleRate);
    // Transform into SVF parameters
    filterParams.resize(bqParams.size());
    for (size_t i = 0; i < bqParams.size(); ++i)
        filterParams[i].setFromBiquadParams(bqParams[i]);
    // match state size.
    filterStates.resize(bqParams.size()); // should also clear previously unused states to avoid glitches.
}

void process(float* out, const float* in, int numSamples)
{
    AudioFilter::processFilterCascade(filterStates, filterParams, out, in, numSamples);
}
```

Using `AudioFilter::FilterInstance` which also takes care of matching higher order butterworth state and filter parameter sizes:

```
#include "../../audiofilter/src/ButterworthCreator.h"
#include "../../audiofilter/src/FilterInstance.h"

static constexpr int maxOrder = 8;
AudioFilter::BiquadParamCascade bqParams = AudioFilter::BiquadParamCascade(maxOrder);
AudioFilter::FilterParamsCascade<float> filterParams = AudioFilter::FilterParamsCascade<float>(maxOrder);
AudioFilter::FilterStateCascade<float> filterStates = AudioFilter::FilterStateCascade<float>(maxOrder);
AudioFilter::ButterworthCreator bwCreator = AudioFilter::ButterworthCreator(maxOrder);
AudioFilter::FilterInstance<float> filterInstance = AudioFilter::FilterInstance<float>(1, maxOrder);

void setFilter(double freq, double q, double gain, size_t order, AudioFilter::FilterType type, double sampleRate)
{
    // create biquad params
    bwCreator.createBandShelf(bqParams, freq, Q, gain, order, sampleRate);
    // Transform into SVF parameters
    filterInstance.setParams(bqParams);
}

void process(float* out, const float* in, int numSamples)
{
    // AudioFilter::FilterInstance allows to process multiple channels using the same filter parameters.
    // The channels are passed as channel array, e.g. float* channelData[numChannels] = { float* channel0, float* channel1 ...};
    // Here only one channel is used, thus the reference and const cast.
    filterInstance.processBlock(&out, const_cast<const float**> (&in), numSamples);
}
```

The class `AudioFilter::FilterInstance` can also be used for basic filters, e.g.

```
void setFilter(double freq, double q, double gain, AudioFilter::FilterType type, double sampleRate)
{
    // Temporary biquad params
    AudioFilter::BiquadParam bqParams;
    // Setup biquad params
    AudioFilter::ParametricCreator::createMZTiStage(bqParams, freq, gain, Q, type, sampleRate);
    // Transform into SVF parameters
    filterInstance.setParams(bqParams);
}
```

## License

The code is MIT licensed.
Parts of the code (class AudioFilter::ButterworthCreator) are inspiered by Vinnie Falco's dspfilters (https://github.com/vinniefalco/DSPFilters), although the code has been rewritten to fit into AudioFilter.
