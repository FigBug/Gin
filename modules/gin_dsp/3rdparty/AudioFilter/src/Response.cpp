#ifndef _USE_MATH_DEFINES
 #define _USE_MATH_DEFINES
#endif
#include <cmath>
#include "Response.h"
#include <algorithm>

namespace AudioFilter
{

namespace Response
{

//==============================================================================
std::vector<float> createLogFreqs(float startFreq, float endFreq, int numPoints)
{
    std::vector<float> ret(static_cast<size_t> (numPoints));

    const auto logEndStart = std::log(endFreq / startFreq);

    for (int i = 0; i < numPoints; ++i)
    {
        const auto freq = startFreq * std::exp(logEndStart * i / (numPoints - 1.f));
        ret[static_cast<size_t> (i)] = freq;
    }

    return ret;
}

std::vector<float> createSin2W2(const std::vector<float>& freqs, float sampleRate)
{
    const auto numPoints = freqs.size();
    std::vector<float> ret(numPoints);

    for (size_t i = 0; i < numPoints; ++i)
    {
        const auto sinw2 = sin(static_cast<float> (juce::MathConstants<double>::pi) * freqs[i] / sampleRate);
        ret[i] = sinw2 * sinw2;
    }

    return ret;
}

void initGains(std::vector<float>& gains, size_t size)
{
    if (gains.size() != size)
        gains.resize(size);

    for (auto& g : gains)
        g = 1.f;
}

void getResponse(const BiquadParam& params, std::vector<float>& gains, const std::vector<float>& sin2w2)
{
    const auto  numPoints = sin2w2.size();

    if (gains.size() < numPoints)
        initGains(gains, numPoints);

    const auto b0 = static_cast<float> (params.b0);
    const auto b1 = static_cast<float> (params.b1);
    const auto b2 = static_cast<float> (params.b2);
    const auto a1 = static_cast<float> (params.a1);
    const auto a2 = static_cast<float> (params.a2);

    for (size_t i = 0; i < numPoints; ++i)
    {
        const auto phi = sin2w2[i];
        const auto nom = (b0 + b1 + b2)*(b0 + b1 + b2) - 4 * (b0*b1 + 4 * b0*b2 + b1 * b2)*phi + 16 * b0*b2*phi*phi;
        const auto den = (1.f + a1 + a2)*(1.f + a1 + a2) - 4 * (1.f*a1 + 4 * 1.f*a2 + a1 * a2)*phi + 16 * 1.f*a2*phi*phi;

        gains[i] *= nom / den;
    }

    if (params.useCompensation)
    {
        const auto c0 = static_cast<float> (params.c0);
        const auto c1 = static_cast<float> (params.c1);
        const auto c2 = static_cast<float> (params.c2);

        for (size_t i = 0; i < numPoints; ++i)
        {
            const auto phi = sin2w2[i];
            const auto cor = (c0 + c1 + c2)*(c0 + c1 + c2) - 4 * (c0*c1 + 4 * c0*c2 + c1 * c2)*phi + 16 * c0*c2*phi*phi;

            gains[i] *= cor;
        }
    }
}

void getResponse(const BiquadParamCascade& params, std::vector<float>& gains, const std::vector<float>& sin2w2)
{
    for (auto p : params)
        getResponse(p, gains, sin2w2);
}

double getResponsePoint(const BiquadParam& params, double freq, double sampleRate)
{
    const auto sin2w = sin(juce::MathConstants<double>::pi * freq / sampleRate);
    const auto phi = sin2w * sin2w;

    const auto b0 = params.b0;
    const auto b1 = params.b1;
    const auto b2 = params.b2;
    const auto a1 = params.a1;
    const auto a2 = params.a2;

    auto gain = 1.;

    {
        const auto nom = (b0 + b1 + b2)*(b0 + b1 + b2) - 4 * (b0*b1 + 4 * b0*b2 + b1 * b2)*phi + 16 * b0*b2*phi*phi;
        const auto den = (1.f + a1 + a2)*(1.f + a1 + a2) - 4 * (1.f*a1 + 4 * 1.f*a2 + a1 * a2)*phi + 16 * 1.f*a2*phi*phi;

        gain = nom / den;
    }

    if (params.useCompensation)
    {
        const auto c0 = params.c0;
        const auto c1 = params.c1;
        const auto c2 = params.c2;

        const auto cor = (c0 + c1 + c2)*(c0 + c1 + c2) - 4 * (c0*c1 + 4 * c0*c2 + c1 * c2)*phi + 16 * c0*c2*phi*phi;
        gain *= cor;
    }

    return std::sqrt(gain);
}

double getResponsePoint(const BiquadParamCascade& params, double freq, double sampleRate)
{
    auto gain = 1.;

    for (auto p : params)
        gain *= getResponsePoint(p, freq, sampleRate);

    return gain;
}

void convertToDb(std::vector<float>& gains)
{
    for (auto& g : gains)
        g = 10.f * std::log10(std::max(1e-8f, g));
}

//==============================================================================
ResponseBase::ResponseBase(int numpoints, float startfreq, float endfreq)
    : numPoints(numpoints), startFreq(startfreq), endFreq(endfreq)
{
    freqs = createLogFreqs(startFreq, endFreq, numPoints);
}

void ResponseBase::setSampleRate(float newSampleRate)
{
    sampleRate = newSampleRate;
    sin2w2 = createSin2W2(freqs, static_cast<float> (sampleRate));
}

int ResponseBase::getNumPoints() const
{
    return numPoints;
}

float ResponseBase::getStartFreq() const
{
    return startFreq;
}

float ResponseBase::getEndFreq() const
{
    return endFreq;
}

const std::vector<float>& ResponseBase::getFreqs() const
{
    return freqs;
}

const std::vector<float>& ResponseBase::getSin2w2() const
{
    return sin2w2;
}

int ResponseBase::getBinForFreq(float freq) const
{
    const auto binIndex = (numPoints - 1.f) * std::log(freq / startFreq) / std::log(endFreq / startFreq);

    return static_cast<int> (std::max(0.f, std::min(numPoints - 1.f, binIndex + 0.5f)));
}

void ResponseBase::getResponse(const BiquadParam& params, std::vector<float>& gains, float peakFreq) const
{
    AudioFilter::Response::getResponse(params, gains, sin2w2);

    if (peakFreq > 0)
    {
        const auto bin = getBinForFreq(peakFreq);
        const auto gain = AudioFilter::Response::getResponsePoint(params, peakFreq, sampleRate);
        gains[static_cast<size_t> (bin)] = static_cast<float> (gain*gain);
    }
}

void ResponseBase::getResponse(const BiquadParam& params, std::vector<float>& gains) const
{
    getResponse(params, gains, -1.f);
}

void ResponseBase::getResponse(const BiquadParamCascade& params, std::vector<float>& gains, float peakFreq) const
{
    AudioFilter::Response::getResponse(params, gains, sin2w2);

    if (peakFreq > 0)
    {
        const auto bin = getBinForFreq(peakFreq);
        const auto gain = AudioFilter::Response::getResponsePoint(params, peakFreq, sampleRate);
        gains[static_cast<size_t> (bin)] = static_cast<float> (gain*gain);
    }
}

void ResponseBase::getResponse(const BiquadParamCascade& params, std::vector<float>& gains) const
{
    getResponse(params, gains, -1.f);
}


} // namespace Response

} // namespace AudioFilter
