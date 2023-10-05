#ifndef _USE_MATH_DEFINES
 #define _USE_MATH_DEFINES
#endif
#include <cmath>
#include "ButterworthCreator.h"
#include "Response.h"
#include <assert.h>

namespace AudioFilter
{

namespace QBasedButterworth
{

double getButterworthQ(int index, int order)
{
    return -0.5 / std::cos(juce::MathConstants<double>::pi * (2 * index + order + 1) / (2 * order));
}

void addButterworthPass(BiquadParamCascade& cascade, double freq, bool hipass, size_t order, double sampleRate, FilterImplementation impl)
{
    const size_t numBiquads = (order + 1) / 2;

    const size_t startIdx = cascade.size();
    cascade.resize(startIdx + numBiquads);
    const auto filterType = hipass ? FilterType::afHiPass : FilterType::afLoPass;

    for (size_t i = 0; i < numBiquads; ++i)
    {
        cascade[i].useCompensation = false;
        const double Q = getButterworthQ(static_cast<int> (i), static_cast<int> (order));
        ParametricCreator::createStage(cascade[startIdx + i], freq, 0., Q, filterType, sampleRate, impl);
    }


    if ((order & 1) == 1)
    {
        const auto filterType6 = hipass ? FilterType::afHiPass6 : FilterType::afLoPass6;
        ParametricCreator::createStage(cascade[startIdx + numBiquads - 1], freq, 0., -1, filterType6, sampleRate, impl);
    }
}

void addButterworthShelf(BiquadParamCascade& cascade, double freq, double gain, bool hishelf, size_t order, double sampleRate, FilterImplementation impl)
{
    const size_t numBiquads = (order + 1) / 2;

    const size_t startIdx = cascade.size();
    cascade.resize(startIdx + numBiquads);
    const auto filterType = hishelf ? FilterType::afHiShelf : FilterType::afLoShelf;

    gain = gain / order;

    for (size_t i = 0; i < numBiquads; ++i)
    {
        cascade[i].useCompensation = false;
        const double Q = getButterworthQ(static_cast<int> (i), static_cast<int> (order));
        ParametricCreator::createStage(cascade[startIdx + i], freq, 2*gain, Q, filterType, sampleRate, impl);
    }

    if ((order & 1) == 1)
    {
        // Other than even order shelves a first order shelf isn't symmetric around the set frequency (e.g. gain/2 at f=freq). 
        // The following will shift the shelf response to get this symmetric response.
        const auto filterType6 = hishelf ? FilterType::afHiShelf6 : FilterType::afLoShelf6;
        freq *= std::pow(10., (hishelf ? 1. : -1.) * gain / 40);
        
        // While MZTi filters don't mind a frequency parameter above samplerate/2, BLT filters do, so they need to be capped.
        // As a result, the shelves won't look as nice at high frequencies, so it is recommended to either use MZTi filters or only allow 
        // even orders (12, 24, 36 but not 6, 18, 30 dB/oct).
        if (impl == FilterImplementation::filterBLT)
            freq = std::min(freq, 0.4999 * sampleRate);

        ParametricCreator::createStage(cascade[startIdx + numBiquads - 1], freq, gain, -1, filterType6, sampleRate, impl);
    }
}

void addButterworthAllpass(BiquadParamCascade& cascade, double freq, size_t order, double sampleRate, FilterImplementation impl)
{
    const size_t numBiquads = (order + 1) / 2;

    const size_t startIdx = cascade.size();
    cascade.resize(startIdx + numBiquads);

    for (size_t i = 0; i < numBiquads; ++i)
    {
        const double Q = getButterworthQ(static_cast<int> (i), static_cast<int> (order));
        ParametricCreator::createStage(cascade[startIdx + i], freq, 0., Q, afAllPass, sampleRate, impl);
    }

    if ((order & 1) == 1)
        ParametricCreator::createStage(cascade[startIdx + numBiquads - 1], freq, 0., -1, afAllPass6, sampleRate, impl);
}

void addLinkwitzReiley(BiquadParamCascade& cascade, double freq, bool hipass, size_t order, double sampleRate, FilterImplementation impl)
{
    if (order == 0)
    {
        addButterworthPass(cascade, freq, hipass, 1, sampleRate, impl);
    }
    else
    {
        addButterworthPass(cascade, freq, hipass, order, sampleRate, impl);
        addButterworthPass(cascade, freq, hipass, order, sampleRate, impl);
    }
}

void createHiLoPass(BiquadParamCascade& params, double freq, bool hipass, size_t order, double sampleRate, FilterImplementation impl)
{
    params.resize(0);
    addButterworthPass(params, freq, hipass, order, sampleRate, impl);
}

void createHiLoShelf(BiquadParamCascade& params, double freq, double gain, bool hiShelf, size_t order, double sampleRate, FilterImplementation impl)
{
    params.resize(0);
    addButterworthShelf(params, freq, gain, hiShelf, order, sampleRate, impl);
}

void createAllPass(BiquadParamCascade& params, double freq, size_t order, double sampleRate, FilterImplementation impl)
{
    params.resize(0);
    addButterworthAllpass(params, freq, order, sampleRate, impl);
}

} // namespace QBasedButterworth

//==============================================================================
ButterworthCreator::ButterworthCreator(size_t maxFilterOrder)
    : analogPoleZero(static_cast<int> (maxFilterOrder)), digitalPoleZero(static_cast<int> (maxFilterOrder))
{

}

void ButterworthCreator::addHiLoPass(BiquadParamCascade& params, double freq, bool hipass, size_t order, double sampleRate)
{
    createAnalogLowpass(order);

    if (hipass)
        transformToHighpass(order, freq, sampleRate);
    else
        transformToLowpass(order, freq, sampleRate);

    createBiquads(params);

    if (params.size() > 0)
    {
        const auto cor = 1. / Response::getResponsePoint(params, hipass ? sampleRate/2 : 0., sampleRate);
        params[0].b0 *= cor;
        params[0].b1 *= cor;
        params[0].b2 *= cor;
    }
}

void ButterworthCreator::addBandPass(BiquadParamCascade& params, double freq, double Q, size_t order, double sampleRate)
{
    const auto bandWidthHz = freq / Q;

    createAnalogLowpass(order);
    const auto minFreq = (-bandWidthHz + std::sqrt(bandWidthHz*bandWidthHz + 4 * freq*freq)) / 2;
    const auto maxFreq = minFreq + bandWidthHz;
    transformToBandpass(order, minFreq, maxFreq, sampleRate);
    createBiquads(params);

    if (params.size() > 0)
    {
        const auto gain = 1. / Response::getResponsePoint(params, freq, sampleRate);
        params[0].b0 *= gain;
        params[0].b1 *= gain;
        params[0].b2 *= gain;
    }
}

void ButterworthCreator::addBandStop(BiquadParamCascade& params, double freq, double Q, size_t order, double sampleRate)
{
    const auto bandWidthHz = freq / Q;

    createAnalogLowpass(order);
    const auto minFreq = (-bandWidthHz + std::sqrt(bandWidthHz*bandWidthHz + 4 * freq*freq)) / 2;
    const auto maxFreq = minFreq + bandWidthHz;
    transformToBandstop(order, minFreq, maxFreq, sampleRate);
    createBiquads(params);

    if (params.size() > 0)
    {
        const auto gain = 1. / Response::getResponsePoint(params, freq > sampleRate / 4 ? 0. : sampleRate / 2, sampleRate);
        params[0].b0 *= gain;
        params[0].b1 *= gain;
        params[0].b2 *= gain;
    }
}

void ButterworthCreator::addHiLoShelf(BiquadParamCascade& params, double freq, double gain, bool hiShelf, size_t order, double sampleRate)
{
    createAnalogLowShelf(order, gain);

    if (hiShelf)
        transformToHighpass(order, freq, sampleRate);
    else
        transformToLowpass(order, freq, sampleRate);

    createBiquads(params);

    if (params.size() > 0)
    {
        const auto cor = 1. / Response::getResponsePoint(params, hiShelf ? 0 : sampleRate / 2, sampleRate);
        params[0].b0 *= cor;
        params[0].b1 *= cor;
        params[0].b2 *= cor;
    }
}

void ButterworthCreator::addBandShelf(BiquadParamCascade& params, double freq, double Q, double gain, size_t order, double sampleRate)
{
    const auto bandWidthHz = freq / Q;
    createAnalogLowShelf(order, gain);
    const auto minFreq = (-bandWidthHz + std::sqrt(bandWidthHz*bandWidthHz + 4 * freq*freq)) / 2;
    const auto maxFreq = minFreq + bandWidthHz;
    transformToBandpass(order, minFreq, maxFreq, sampleRate);
    createBiquads(params);

    if (params.size() > 0)
    {
        const auto cor = std::pow(10., 0.05*gain) / Response::getResponsePoint(params, freq, sampleRate);
        params[0].b0 *= cor;
        params[0].b1 *= cor;
        params[0].b2 *= cor;
    }
}

void ButterworthCreator::add(BiquadParamCascade& params, ButterworthType type, double freq, double Q, double gain, size_t order, double sampleRate)
{
    switch (type)
    {
    case bwLoPass:
        addHiLoPass(params, freq, false, order, sampleRate);
        break;
    case bwHiPass:
        addHiLoPass(params, freq, true, order, sampleRate);
        break;
    case bwLowShelf:
        addHiLoShelf(params, freq, gain, false, order, sampleRate);
        break;
    case bwHiShelf:
        addHiLoShelf(params, freq, gain, true, order, sampleRate);
        break;
    case bwBandShelf:
        addBandShelf(params, freq, Q, gain, order, sampleRate);
        break;
    case bwBandPass:
        addBandPass(params, freq, Q, order, sampleRate);
        break;
    case bwBandStop:
        addBandStop(params, freq, Q, order, sampleRate);
        break;
    case bwAllPass:
    default:
        assert(false); // Should be a type suppotered by this class.
        break;
    }
}

void ButterworthCreator::createHiLoPass(BiquadParamCascade& params, double freq, bool hipass, size_t order, double sampleRate)
{
    params.resize(0);
    addHiLoPass(params, freq, order, hipass, sampleRate);
}

void ButterworthCreator::createBandPass(BiquadParamCascade& params, double freq, double Q, size_t order, double sampleRate)
{
    params.resize(0);
    addBandPass(params, freq, Q, order, sampleRate);
}

void ButterworthCreator::createBandStop(BiquadParamCascade& params, double freq, double Q, size_t order, double sampleRate)
{
    params.resize(0);
    addBandStop(params, freq, Q, order, sampleRate);
}

void ButterworthCreator::createHiLoShelf(BiquadParamCascade& params, double freq, double gain, bool hiShelf, size_t order, double sampleRate)
{
    params.resize(0);
    addHiLoShelf(params, freq, gain, hiShelf, order, sampleRate);
}

void ButterworthCreator::createBandShelf(BiquadParamCascade& params, double freq, double Q, double gain, size_t order, double sampleRate)
{
    params.resize(0);
    addBandShelf(params, freq, Q, gain, order, sampleRate);
}

void ButterworthCreator::create(BiquadParamCascade& params, ButterworthType type, double freq, double Q, double gain, size_t order, double sampleRate)
{
    switch (type)
    {
    case bwLoPass:
        createHiLoPass(params, freq, false, order, sampleRate);
        break;
    case bwHiPass:
        createHiLoPass(params, freq, true, order, sampleRate);
        break;
    case bwLowShelf:
        createHiLoShelf(params, freq, gain, false, order, sampleRate);
        break;
    case bwHiShelf:
        createHiLoShelf(params, freq, gain, true, order, sampleRate);
        break;
    case bwBandShelf:
        addBandShelf(params, freq, Q, gain, order, sampleRate);
        break;
    case bwBandPass:
        createBandPass(params, freq, Q, order, sampleRate);
        break;
    case bwBandStop:
        createBandStop(params, freq, Q, order, sampleRate);
        break;
    case bwAllPass:
    default:
        assert(false); // Should be a type suppotered by this class.
        break;
    }
}

void ButterworthCreator::createAnalogLowpass(size_t order)
{
    const auto n2 = 2. * order;
    const auto pairs = order / 2;

    analogPoleZero.resize((order + 1) / 2);

    for (size_t i = 0; i < pairs; ++i)
    {        
        std::complex<double> c = std::polar(1., 0.5 * juce::MathConstants<double>::pi + (2 * i + 1) * juce::MathConstants<double>::pi / n2);
        analogPoleZero[i] = PoleZeroPair::conjugated(c, std::numeric_limits<double>::infinity());
    }

    if (order & 1)
    {
        analogPoleZero[pairs] = PoleZeroPair::single(-1, std::numeric_limits<double>::infinity());
    }
}

void ButterworthCreator::createAnalogLowShelf(size_t order, double gainDb)
{
    const auto n2 = 2. * order;
    const auto g = std::pow(std::pow(10., gainDb / 20), 1. / n2);
    const auto gp = -1. / g;
    const auto gz = -g;

    const auto pairs = static_cast<int> (order / 2);
    analogPoleZero.resize((order + 1) / 2);

    for (int i = 0; i < pairs; ++i)
    {
        const double theta = juce::MathConstants<double>::pi * (0.5 - (2 * i + 1) / n2);
        analogPoleZero[i] = PoleZeroPair::conjugated(std::polar(gp, theta), std::polar(gz, theta));
    }

    if (order & 1)
    {
        analogPoleZero[pairs] = PoleZeroPair::single(gp, gz);
    }
}

void ButterworthCreator::transformToBandpass(size_t order, double minFreq, double maxFreq, double sampleRate)
{
    minFreq = std::max(0., minFreq);
    maxFreq = std::min(0.5 * 0.99 * sampleRate, maxFreq);
    const double wc2 = 2 * juce::MathConstants<double>::pi * minFreq / sampleRate;
    const double wc = 2 * juce::MathConstants<double>::pi * maxFreq / sampleRate;
    
    TransformParam params;

    const double a = std::cos((wc + wc2) * 0.5) / std::cos((wc - wc2) * 0.5);
    params.b = 1 / std::tan((wc - wc2) * 0.5);
    params.a2 = a * a;
    params.b2 = params.b * params.b;
    params.ab = a * params.b;

    const auto numPoles = order;
    const auto pairs = numPoles / 2;

    digitalPoleZero.resize(0);

    for (size_t i = 0; i < pairs; ++i)
    {
        const PoleZeroPair& analog(analogPoleZero[i]);
        const auto pole = bandpassTransform(analog.pole1, params);
        const auto zero = bandpassTransform(analog.zero1, params);
        digitalPoleZero.push_back(PoleZeroPair::conjugated(pole.x1, zero.x1));
        digitalPoleZero.push_back(PoleZeroPair::conjugated(pole.x2, zero.x2));
    }

    if (numPoles & 1)
    {
        const PoleZeroPair& analog(analogPoleZero[pairs]);
        const auto pole = bandpassTransform(analog.pole1, params);
        const auto zero = bandpassTransform(analog.zero1, params);
        digitalPoleZero.push_back(PoleZeroPair(pole.x1, zero.x1, pole.x2, zero.x2));
    }
}

void ButterworthCreator::transformToBandstop(size_t order, double minFreq, double maxFreq, double sampleRate)
{
    minFreq = std::max(0., minFreq);
    maxFreq = std::min(0.5 * 0.99 * sampleRate, maxFreq);
    const double wc2 = 2 * juce::MathConstants<double>::pi * minFreq / sampleRate;
    const double wc = 2 * juce::MathConstants<double>::pi * maxFreq / sampleRate;

    TransformParam params;

    params.a = std::cos((wc + wc2) * 0.5) / std::cos((wc - wc2) * 0.5);
    params.b = std::tan((wc - wc2) * 0.5);
    params.a2 = params.a * params.a;
    params.b2 = params.b * params.b;

    const auto numPoles = order;
    const auto pairs = numPoles / 2;

    digitalPoleZero.resize(0);

    for (size_t i = 0; i < pairs; ++i)
    {
        const PoleZeroPair& analog(analogPoleZero[i]);
        const auto pole = bandstopTransform(analog.pole1, params);
        const auto zero = bandstopTransform(analog.zero1, params);
        digitalPoleZero.push_back(PoleZeroPair::conjugated(pole.x1, zero.x1));
        digitalPoleZero.push_back(PoleZeroPair::conjugated(pole.x2, zero.x2));
    }

    if (numPoles & 1)
    {
        const PoleZeroPair& analog(analogPoleZero[pairs]);
        const auto pole = bandstopTransform(analog.pole1, params);
        const auto zero = bandstopTransform(analog.zero1, params);
        digitalPoleZero.push_back(PoleZeroPair(pole.x1, zero.x1, pole.x2, zero.x2));
    }
}

void ButterworthCreator::transformToLowpass(size_t order, double freq, double sampleRate)
{
    const auto w = std::tan(juce::MathConstants<double>::pi * freq / sampleRate);

    const auto pairs = order / 2;

    digitalPoleZero.resize(0);

    for (size_t i = 0; i < pairs; ++i)
    {
        const PoleZeroPair& analog(analogPoleZero[i]);
        const auto pole = lowpassTransform(analog.pole1, w);
        const auto zero = lowpassTransform(analog.zero1, w);
        digitalPoleZero.push_back(PoleZeroPair::conjugated(pole.x1, zero.x1));
    }

    if (order & 1)
    {
        const PoleZeroPair& analog(analogPoleZero[pairs]);
        const auto pole = lowpassTransform(analog.pole1, w);
        const auto zero = lowpassTransform(analog.zero1, w);
        digitalPoleZero.push_back(PoleZeroPair(pole.x1, zero.x1, pole.x2, zero.x2));
    }
}

void ButterworthCreator::transformToHighpass(size_t order, double freq, double sampleRate)
{
    const auto w = 1. / std::tan(juce::MathConstants<double>::pi * freq / sampleRate);

    const auto numPoles = order;
    const auto pairs = numPoles / 2;
    
    digitalPoleZero.resize(0);

    for (size_t i = 0; i < pairs; ++i)
    {
        const PoleZeroPair& analog(analogPoleZero[i]);
        const auto pole = highpassTransform(analog.pole1, w);
        const auto zero = highpassTransform(analog.zero1, w);
        digitalPoleZero.push_back(PoleZeroPair::conjugated(pole.x1, zero.x1));
    }

    if (numPoles & 1)
    {
        const PoleZeroPair& analog(analogPoleZero[pairs]);
        const auto pole = highpassTransform(analog.pole1, w);
        const auto zero = highpassTransform(analog.zero1, w);
        digitalPoleZero.push_back(PoleZeroPair(pole.x1, zero.x1, pole.x2, zero.x2));
    }
}

void ButterworthCreator::createBiquads(BiquadParamCascade& params)
{
    assert(digitalPoleZero.size() <= params.maxSize());

    const auto startIdx = params.size();
    const auto biquadsToAdd = digitalPoleZero.size();
    params.resize(startIdx + biquadsToAdd);

    for (size_t i = 0; i < biquadsToAdd; ++i)
    {
        params[i].b0 = 1;
        params[i].useCompensation = false;
        {
            std::complex<double> pole1(digitalPoleZero[i].pole1);
            std::complex<double> pole2(digitalPoleZero[i].pole2);
            if (pole1.imag() != 0)
            {
                assert(pole2 == std::conj(pole1));

                params[i].a1 = -2 * pole1.real();
                params[i].a2 = std::norm(pole1);
            }
            else
            {
                assert(pole2.imag() == 0);

                params[i].a1 = -(pole1.real() + pole2.real());
                params[i].a2 = pole1.real() * pole2.real();
            }
        }

        {
            std::complex<double> zero1(digitalPoleZero[i].zero1);
            std::complex<double> zero2(digitalPoleZero[i].zero2);

            if (zero1.imag() != 0)
            {
                assert(zero2 == std::conj(zero1));

                params[i].b1 = -2 * zero1.real();
                params[i].b2 = std::norm(zero1);
            }
            else
            {
                assert(zero2.imag() == 0);

                params[i].b1 = -(zero1.real() + zero2.real());
                params[i].b2 = zero1.real() * zero2.real();
            }
        }
    }
}

ButterworthCreator::DualComplex ButterworthCreator::bandpassTransform(const std::complex<double>& in, const TransformParam& params)
{
    if (in == std::numeric_limits<double>::infinity())
        return DualComplex(-1., 1.);

    const auto c = (1. + in) / (1. - in); 

    std::complex<double> v = 0;
    v = 4 * (params.b2 * (params.a2 - 1) + 1) * c;
    v += 8 * (params.b2 * (params.a2 - 1) - 1);
    v *= c;
    v += 4 * (params.b2 * (params.a2 - 1) + 1);
    v = std::sqrt(v);

    std::complex<double> u = -v;
    u += 2 * params.ab * c;
    u += 2 * params.ab;

    v += 2 * params.ab * c;
    v += 2 * params.ab;

    std::complex<double> d = 0;
    d = 2 * (params.b - 1) * c;
    d += 2 * (1 + params.b);

    DualComplex out(u / d, v / d);
    return out;
}

ButterworthCreator::DualComplex ButterworthCreator::bandstopTransform(const std::complex<double>& in, const TransformParam& params)
{
    const auto c =  in == std::numeric_limits<double>::infinity() ? std::complex<double>(-1., 0.) : (1. + in) / (1. - in);

    std::complex<double> u = 0;
    u = u + 4 * (params.b2 + params.a2 - 1) * c;
    u += 8 * (params.b2 - params.a2 + 1);
    u *= c;
    u += 4 * (params.a2 + params.b2 - 1);
    u = std::sqrt(u);

    std::complex<double> v = - 0.5 * u;
    v += params.a;
    v = v - params.a * c;

    u *= .5;
    u += params.a;
    u = u - params.a * c;

    std::complex<double> d = params.b + 1;
    d = d + (params.b - 1) * c;

    DualComplex out(u / d, v / d);
    return out;
}

ButterworthCreator::DualComplex ButterworthCreator::lowpassTransform(const std::complex<double>& in, double w)
{
    if (in == std::numeric_limits<double>::infinity())
        return std::complex<double>(-1., 0.);

    const auto c = w * in;
    return (1. + c) / (1. - c);
}

ButterworthCreator::DualComplex ButterworthCreator::highpassTransform(const std::complex<double>& in, double w)
{
    if (in == std::numeric_limits<double>::infinity())
        return std::complex<double>(1., 0.);

    const auto c = w * in;
    return - (1. + c) / (1. - c);
}

} // namespace AudioFilter
