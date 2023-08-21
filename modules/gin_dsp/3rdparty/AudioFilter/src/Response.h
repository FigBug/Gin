#pragma once

#include "AudioFilterTypes.h"

namespace AudioFilter
{

//==============================================================================
// This namespace contains free functions to create a frequency response from a set of (digital) biquad parameters.
namespace Response
{
/*
* Creates an array with log spaced frequencies.
*/
std::vector<float> createLogFreqs(float startFreq, float endFreq, int numPoints);

/*
* Creates a sin2w2 ( sin^2(w/2) ) array to be used with getResponse().
*/
std::vector<float> createSin2W2(const std::vector<float>& freqs, float sampleRate);

/*
* Initialises a gains vector with ones and resizes if needed.
*/
void initGains(std::vector<float>& gains, size_t size);

/*
* Calculates frequency response according to the comment on <https://www.musicdsp.org/en/latest/Analysis/186-frequency-response-from-biquad-coefficients.html>.
* If the gains vector has a different size than sin2w2, the array will be initialised. For performance reasons the gains vector contains the squared magnitude. 
* Use @convertToDb to convert to decibels.
*/
void getResponse(const BiquadParam& params, std::vector<float>& gains, const std::vector<float>& sin2w2);

/*
* Same as getResponse(const BiquadParam& params...) but for higher order filters.
*/
void getResponse(const BiquadParamCascade& params, std::vector<float>& gains, const std::vector<float>& sin2w2);

// Returns magnitude repsonse for a single point.
double getResponsePoint(const BiquadParam& params, double freq, double sampleRate);

// Returns magnitude repsonse for a single point in a higher order filter.
double getResponsePoint(const BiquadParamCascade& params, double freq, double sampleRate);

/*
* Convert linear squared gains to dezibels. Clips values below -160 dB to avoid -Inf dB.
*/
void convertToDb(std::vector<float>& gains);

/*
* A class to handle the frequency points as well as the sin^2(w/2) data.
*/
class ResponseBase
{
public:

    ResponseBase(int numpoints, float startfreq, float endfreq);
    virtual ~ResponseBase() = default;
    void setSampleRate(float newSampleRate);
    int getNumPoints() const;
    float getStartFreq() const;
    float getEndFreq() const;
    const std::vector<float>& getFreqs() const;
    const std::vector<float>& getSin2w2() const;
    int getBinForFreq(float freq) const;

    void getResponse(const BiquadParam& params, std::vector<float>& gains, float peakFreq) const;
    void getResponse(const BiquadParam& params, std::vector<float>& gains) const;
    void getResponse(const BiquadParamCascade& params, std::vector<float>& gains, float peakFreq) const;
    void getResponse(const BiquadParamCascade& params, std::vector<float>& gains) const;

protected:

    int numPoints;
    float startFreq;
    float endFreq;
    double sampleRate = 0;
    std::vector<float> freqs;
    std::vector<float> sin2w2;
};

}

} // namespace AudioFilter