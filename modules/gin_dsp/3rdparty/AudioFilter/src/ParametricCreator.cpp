#ifndef _USE_MATH_DEFINES
 #define _USE_MATH_DEFINES
#endif
#include <cmath>
#include "ParametricCreator.h"

namespace AudioFilter
{

namespace ParametricCreator
{

//==============================================================================
void getAnalog(BiquadParam& biquad, double Q, double gain, FilterType filterType)
{
    const auto A = std::pow(10., 0.025 * gain);

    switch (filterType)
    {
    case afLoPass6:
        biquad.b0 = 1;
        biquad.b1 = Q > 0 ? 1 : 0;
        biquad.b2 = 0;
        biquad.a1 = Q > 0 ? 1 / (std::sqrt(0.5) * Q) : 1;
        biquad.a2 = Q > 0 ? 1 : 0;
        break;
    case afHiPass6:
        biquad.b0 = 0;
        biquad.b1 = 1;
        biquad.b2 = Q > 0 ? 1 : 0;
        biquad.a1 = Q > 0 ? 1 / (std::sqrt(0.5) * Q) : 1;
        biquad.a2 = Q > 0 ? 1 : 0;
        break;
    case afLoShelf6:
    {
        const auto g = A * A - 1;
        biquad.b0 = 1 + g;
        biquad.b1 = 1;
        biquad.b2 = 0;
        biquad.a1 = 1;
        biquad.a2 = 0;
        break;
    }
    case afHiShelf6:
    {
        const auto g = A * A - 1;
        biquad.b0 = 1;
        biquad.b1 = 1 + g;
        biquad.b2 = 0;
        biquad.a1 = 1;
        biquad.a2 = 0;
        break;
    }
    case afAllPass6:
        biquad.b0 = 1;
        biquad.b1 = -1;
        biquad.b2 = 0;
        biquad.a1 = 1;
        biquad.a2 = 0;
        break;
    case afLoPass:
        biquad.b0 = 1;
        biquad.b1 = 0;
        biquad.b2 = 0;
        biquad.a1 = 1 / Q;
        biquad.a2 = 1;
        break;
    case afHiPass:
        biquad.b0 = 0;
        biquad.b1 = 0;
        biquad.b2 = 1;
        biquad.a1 = 1 / Q;
        biquad.a2 = 1;
        break;
    case afPeak:
        biquad.b0 = 1;
        biquad.b1 = A / Q;
        biquad.b2 = 1;
        biquad.a1 = 1 / (A*Q);
        biquad.a2 = 1;
        break;
    case afLoShelf:
        biquad.b0 = A * A;
        biquad.b1 = A * std::sqrt(A) / Q;
        biquad.b2 = A;
        biquad.a1 = std::sqrt(A) / Q;
        biquad.a2 = A;
        break;
    case afHiShelf:
    {
        const auto a0 = A;
        biquad.b0 = A / a0;
        biquad.b1 = (A * std::sqrt(A) / Q) / a0;
        biquad.b2 = (A * A) / a0;
        biquad.a1 = (std::sqrt(A) / Q) / a0;
        biquad.a2 = 1 / a0;
        break;
    }
    case afAllPass:
        biquad.b0 = 1;
        biquad.b1 = -1 / Q;
        biquad.b2 = 1;
        biquad.a1 = 1 / Q;
        biquad.a2 = 1;
        break;
    case afBandPass:
        biquad.b0 = 0;
        biquad.b1 = 1 / Q;
        biquad.b2 = 0;
        biquad.a1 = 1 / Q;
        biquad.a2 = 1;
        break;
    case afNotch:
        biquad.b0 = 1;
        biquad.b1 = 0;
        biquad.b2 = 1;
        biquad.a1 = 1 / Q;
        biquad.a2 = 1;
        break;
    default:
        biquad.b0 = 1;
        biquad.b1 = 0;
        biquad.b2 = 0;
        biquad.a1 = 0;
        biquad.a2 = 0;
        break;
    }
}

void getMztTransform(BiquadParam& analog, BiquadParam& digital, double freq, double sampleRate)
{
    // Matched Z-transform
    if (analog.b2 == 0)
    {
        if (analog.b1 == 0)
        {
            digital.b0 = 1;
            digital.b1 = 0;
            digital.b2 = 0;
        }
        else
        {
            const auto za0 = analog.b0 / analog.b1;
            digital.b0 = 1;
            digital.b1 = -std::exp(-za0 * 2 * juce::MathConstants<double>::pi*freq / sampleRate);
            digital.b2 = 0;
        }
    }
    else
    {
        const auto b11 = analog.b1*analog.b1;
        const auto b02 = 4 * analog.b0*analog.b2;

        if (b11 >= b02)
        {
            const auto za0 = (analog.b1 + std::sqrt(b11 - b02)) / (2 * analog.b2);
            const auto za1 = (analog.b1 - std::sqrt(b11 - b02)) / (2 * analog.b2);
            const auto zd0 = std::exp(-za0 * 2 * juce::MathConstants<double>::pi*freq / sampleRate);
            const auto zd1 = std::exp(-za1 * 2 * juce::MathConstants<double>::pi*freq / sampleRate);
            digital.b0 = 1;
            digital.b1 = -zd0 - zd1;
            digital.b2 = zd0 * zd1;
        }
        else
        {
            const auto zaRe = analog.b1 / (2 * analog.b2);
            const auto zaIm = std::sqrt(b02 - b11) / (2 * analog.b2);
            const auto dRad = std::exp(-zaRe * 2 * juce::MathConstants<double>::pi*freq / sampleRate);
            const auto zdRe = dRad * std::cos(-zaIm * 2 * juce::MathConstants<double>::pi*freq / sampleRate);
            const auto zdIm = dRad * std::sin(-zaIm * 2 * juce::MathConstants<double>::pi*freq / sampleRate);
            digital.b0 = 1;
            digital.b1 = -2.f * zdRe;
            digital.b2 = zdRe * zdRe + zdIm * zdIm;
        }
    }

    if (analog.a2 == 0)
    {
        if (analog.a1 == 0)
        {
            digital.a1 = 0;
            digital.a2 = 0;
        }
        else
        {
            const auto pa0 = 1 / analog.a1;
            digital.a1 = -std::exp(-pa0 * 2 * juce::MathConstants<double>::pi*freq / sampleRate);
            digital.a2 = 0;
        }
    }
    else
    {
        const auto a11 = analog.a1*analog.a1;
        const auto a02 = 4 * analog.a2;

        if (a11 >= a02)
        {
            const auto pa0 = (analog.a1 + std::sqrt(a11 - a02)) / (2 * analog.a2);
            const auto pa1 = (analog.a1 - std::sqrt(a11 - a02)) / (2 * analog.a2);
            const auto pd0 = std::exp(-pa0 * 2 * juce::MathConstants<double>::pi*freq / sampleRate);
            const auto pd1 = std::exp(-pa1 * 2 * juce::MathConstants<double>::pi*freq / sampleRate);
            digital.a1 = -pd0 - pd1;
            digital.a2 = pd0 * pd1;
        }
        else
        {
            const auto zaRe = analog.a1 / (2 * analog.a2);
            const auto zaIm = std::sqrt(a02 - a11) / (2 * analog.a2);
            const auto dRad = std::exp(-zaRe * 2 * juce::MathConstants<double>::pi*freq / sampleRate);
            const auto zdRe = dRad * std::cos(-zaIm * 2 * juce::MathConstants<double>::pi*freq / sampleRate);
            const auto zdIm = dRad * std::sin(-zaIm * 2 * juce::MathConstants<double>::pi*freq / sampleRate);
            digital.a1 = -2.f * zdRe;
            digital.a2 = zdRe * zdRe + zdIm * zdIm;
        }
    }
}

void getCorrection(BiquadParam& digital, const BiquadParam& analog, double freq, double sampleRate)
{
    double freqs[3];
    freqs[0] = sampleRate / 44100.;
    freqs[1] = sampleRate * 1 / 6.;
    freqs[2] = sampleRate * 2 / 6.;

    double H[3];

    for (int i = 0; i < 3; ++i)
    {
        const auto phi0 = std::sin(juce::MathConstants<double>::pi*freqs[i] / sampleRate);
        const auto phi = phi0 * phi0;
        const auto s = freqs[i] / freq;
        const auto a0 = 1.;

        double Hd, Ha;
        {
            const auto b0 = digital.b0;
            const auto b1 = digital.b1;
            const auto b2 = digital.b2;
            const auto a1 = digital.a1;
            const auto a2 = digital.a2;

            const auto Hdz2 = (b0 + b1 + b2)*(b0 + b1 + b2) - 4 * (b0*b1 + 4 * b0*b2 + b1 * b2)*phi + 16 * b0*b2*phi*phi;
            const auto Hdp2 = (a0 + a1 + a2)*(a0 + a1 + a2) - 4 * (a0*a1 + 4 * a0*a2 + a1 * a2)*phi + 16 * a0*a2*phi*phi;

            Hd = Hdz2 / Hdp2;
        }

        {
            const auto realZero = analog.b0 - analog.b2 * s * s;
            const auto imagZero = s * analog.b1;
            const auto realPole = a0 - analog.a2 * s * s;
            const auto imagPole = s * analog.a1;

            const auto Haz2 = realZero * realZero + imagZero * imagZero;
            const auto Hap2 = realPole * realPole + imagPole * imagPole;
            Ha = Haz2 / Hap2;
        }

        H[i] = std::sqrt(Ha / Hd);
    }

    const auto c1 = 0.5*(H[0] - std::sqrt(H[0] * H[0] - 2 * H[1] * H[1] + 2 * H[2] * H[2]));
    const auto c2 = (3 * (H[0] - c1) - std::sqrt(-3 * H[0] * H[0] + 12 * H[1] * H[1] - 6 * H[0] * c1 - 3 * c1*c1)) / 6;
    const auto c0 = H[0] - c1 - c2;
    digital.c0 = c0;
    digital.c1 = c1;
    digital.c2 = c2;
}

void createMZTiStage(BiquadParam& param, double freq, double gain, double Q, FilterType type, double sampleRate)
{
    BiquadParam analogBq;
    getAnalog(analogBq, Q, gain, type);
    getMztTransform(analogBq, param, freq, sampleRate);
    getCorrection(param, analogBq, freq, sampleRate);
    if (type == afAllPass6)
    {
        param.c0 *= -1;
        param.c1 *= -1;
        param.c2 *= -1;
    }
    param.useCompensation = true;
}

void createBLTStage(BiquadParam& param, double freq, double gain, double Q, FilterType type, double sampleRate)
{
    if (type == afLoPass6 || type == afHiPass6)
        Q *= std::sqrt(0.5f);

    const auto w0 = 2 * juce::MathConstants<double>::pi*freq / sampleRate;
    const auto A = std::pow(10., 0.025 * gain);
    const auto alpha = std::sin(w0) / (2 * Q);

    auto a0 = 1.;

    switch (type)
    {
    case afLoPass:
        a0 = 1 + alpha;
        param.b0 = ((1 - cos(w0)) / 2) / a0;
        param.b1 = (1 - cos(w0)) / a0;
        param.b2 = ((1 - cos(w0)) / 2) / a0;
        param.a1 = (-2 * cos(w0)) / a0;
        param.a2 = (1 - alpha) / a0;
        break;
    case afHiPass:
        a0 = 1 + alpha;
        param.b0 = ((1 + cos(w0)) / 2) / a0;
        param.b1 = -(1 + cos(w0)) / a0;
        param.b2 = ((1 + cos(w0)) / 2) / a0;
        param.a1 = (-2 * cos(w0)) / a0;
        param.a2 = (1 - alpha) / a0;
        break;
    case afLoPass6:
        if (Q > 0)
        {
            a0 = 1 + alpha;
            param.b0 = ((1 - cos(w0)) / 2 + Q * alpha) / a0;
            param.b1 = (1 - cos(w0)) / a0;
            param.b2 = ((1 - cos(w0)) / 2 - Q * alpha) / a0;
            param.a1 = (-2 * cos(w0)) / a0;
            param.a2 = (1 - alpha) / a0;
        }
        else
        {
            const auto c = (std::tan(0.5*w0) - 1) / (std::tan(0.5*w0) + 1);
            param.b0 = 0.5 * (c + 1);
            param.b1 = param.b0;
            param.b2 = 0;
            param.a1 = c;
            param.a2 = 0;
        }
        break;
    case afHiPass6:
        if (Q > 0)
        {
            a0 = 1 + alpha;
            param.b0 = ((1 + cos(w0)) / 2 + Q * alpha) / a0;
            param.b1 = -(1 + cos(w0)) / a0;
            param.b2 = ((1 + cos(w0)) / 2 - Q * alpha) / a0;
            param.a1 = (-2 * cos(w0)) / a0;
            param.a2 = (1 - alpha) / a0;
        }
        else
        {
            const auto c = (std::tan(0.5*w0) - 1) / (std::tan(0.5*w0) + 1);
            param.b0 = 0.5 * (1 - c);
            param.b1 = 0.5 * (c - 1);
            param.b2 = 0;
            param.a1 = c;
            param.a2 = 0;
        }
        break;
    case afLoShelf6:
        {
            const auto c = (std::tan(0.5*w0) - 1) / (std::tan(0.5*w0) + 1);            
            const auto g = 0.5 * (A * A - 1);
            param.b0 = 1 + g * (c + 1);
            param.b1 = c + g * (c + 1);
            param.b2 = 0;
            param.a1 = c;
            param.a2 = 0;
        }
        break;
    case afHiShelf6:
        {
            const auto c = (std::tan(0.5*w0) - 1) / (std::tan(0.5*w0) + 1);
            const auto g = 0.5 * (A * A - 1);
            param.b0 = 1 + g * (1 - c);
            param.b1 = c + g * (c - 1);
            param.b2 = 0;
            param.a1 = c;
            param.a2 = 0;
        }
        break;
    case afBandPass:
        a0 = 1 + alpha;
        param.b0 = alpha / a0;
        param.b1 = 0;
        param.b2 = (-alpha) / a0;
        param.a1 = (-2 * cos(w0)) / a0;
        param.a2 = (1 - alpha) / a0;
        break;
    case afNotch:
        a0 = 1 + alpha;
        param.b0 = 1 / a0;
        param.b1 = (-2 * cos(w0)) / a0;
        param.b2 = (1) / a0;
        param.a1 = (-2 * cos(w0)) / a0;
        param.a2 = (1 - alpha) / a0;
        break;
    case afAllPass6:
    {
        const auto c = (std::tan(0.5 * w0) - 1) / (std::tan(0.5 * w0) + 1);
        param.b0 = c;
        param.b1 = 1;
        param.b2 = 0;
        param.a1 = c;
        param.a2 = 0;
        break;
    }
    case afAllPass:
        a0 = 1 + alpha;
        param.b0 = (1 - alpha) / a0;
        param.b1 = (-2 * cos(w0)) / a0;
        param.b2 = (1 + alpha) / a0;
        param.a1 = (-2 * cos(w0)) / a0;
        param.a2 = (1 - alpha) / a0;
        break;
    case afPeak:
        a0 = 1 + alpha / A;
        param.b0 = (1 + alpha * A) / a0;
        param.b1 = (-2 * cos(w0)) / a0;
        param.b2 = (1 - alpha * A) / a0;
        param.a1 = (-2 * cos(w0)) / a0;
        param.a2 = (1 - alpha / A) / a0;
        break;
    case afLoShelf:
        a0 = (A + 1) + (A - 1)*cos(w0) + 2 * sqrt(A)*alpha;
        param.b0 = (A * ((A + 1) - (A - 1)*cos(w0) + 2 * sqrt(A)*alpha)) / a0;
        param.b1 = (2 * A*((A - 1) - (A + 1)*cos(w0))) / a0;
        param.b2 = (A * ((A + 1) - (A - 1)*cos(w0) - 2 * sqrt(A)*alpha)) / a0;
        param.a1 = (-2 * ((A - 1) + (A + 1)*cos(w0))) / a0;
        param.a2 = ((A + 1) + (A - 1)*cos(w0) - 2 * sqrt(A)*alpha) / a0;
        break;
    case afHiShelf:
        a0 = (A + 1) - (A - 1)*cos(w0) + 2 * sqrt(A)*alpha;
        param.b0 = (A * ((A + 1) + (A - 1)*cos(w0) + 2 * sqrt(A)*alpha)) / a0;
        param.b1 = (-2 * A*((A - 1) + (A + 1)*cos(w0))) / a0;
        param.b2 = (A * ((A + 1) + (A - 1)*cos(w0) - 2 * sqrt(A)*alpha)) / a0;
        param.a1 = (2 * ((A - 1) - (A + 1)*cos(w0))) / a0;
        param.a2 = ((A + 1) - (A - 1)*cos(w0) - 2 * sqrt(A)*alpha) / a0;
    default:
        break;
    }
    
    param.useCompensation = false;
}

void createStage(BiquadParam& param, double freq, double gain, double Q, FilterType type, double sampleRate, FilterImplementation impl)
{
    switch (impl)
    {
    default:
        assert(false);
    case filterBLT:
        createBLTStage(param, freq, gain, Q, type, sampleRate);
        break;
    case filterMZTi:
        createMZTiStage(param, freq, gain, Q, type, sampleRate);
        break;
    }
}

} // namespace ParametricCreator

} // namespace AudioFilter
