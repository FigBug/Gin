#include "dynamics.h"

//================================================================================
const float DIGITAL_TC = -2.0f; // log(1%)
const float ANALOG_TC = -0.43533393574791066201247090699309f; // (log(36.7%)

//================================================================================
void EnvelopeDetector::reset()
{
    envelope = 0.0;
}

void EnvelopeDetector::setParams (float attackS_, float releaseS_, bool analogTC_, Mode detect_, bool logDetector_)
{
    analogTC = analogTC_;
    mode = detect_;
    logDetector = logDetector_;

    setAttackTime (attackS_);
    setReleaseTime (releaseS_);
}

void EnvelopeDetector::setAttackTime (float attackS)
{
    if (analogTC)
        attackTime = (float) std::exp (ANALOG_TC / (attackS * sampleRate));
    else
        attackTime = (float) std::exp (DIGITAL_TC / (attackS * sampleRate));
}

void EnvelopeDetector::setReleaseTime (float releaseS)
{
    if (analogTC)
        releaseTime = (float) std::exp (ANALOG_TC / (releaseS * sampleRate));
    else
        releaseTime = (float) std::exp (DIGITAL_TC / (releaseS * sampleRate));
}

float EnvelopeDetector::process (float input)
{
    switch (mode)
    {
        case peak:
            input = std::fabs (input);
            break;
        case ms:
            input = std::fabs (input) * std::fabs (input);
            break;
        case rms:
            input = std::pow (std::fabs (input) * std::fabs (input), 0.5f);
            break;
    }

    if (input > envelope)
        envelope = attackTime * (envelope - input) + input;
    else
        envelope = releaseTime * (envelope - input) + input;

    envelope = jlimit (0.0f, 1.0f, envelope);

    if (logDetector)
    {
        if (envelope == 0.0f)
            return -100.0f;

        return Decibels::gainToDecibels (envelope);
    }

    return envelope;
}

//================================================================================
void Dynamics::setSampleRate (double sampleRate_)
{
    sampleRate = sampleRate_;

    leftEnvelope.setSampleRate (sampleRate);
    rightEnvelope.setSampleRate (sampleRate);

    reset();
}

void Dynamics::setMode (Type t)
{
    type = t;
}

void Dynamics::setParams (float attackS, float releaseS, float threshhold_, float ratio_, float kneeWidth_)
{
    leftEnvelope.setParams (attackS, releaseS, false, EnvelopeDetector::peak, true);
    rightEnvelope.setParams (attackS, releaseS, false, EnvelopeDetector::peak, true);

    threshold = threshhold_;
    ratio = ratio_;
    kneeWidth = kneeWidth_;
}

void Dynamics::reset()
{
    leftEnvelope.reset();
    rightEnvelope.reset();
}

void Dynamics::process (AudioSampleBuffer& buffer)
{
    int numSamples = buffer.getNumSamples();

    ScratchBuffer inputBuffer (buffer);

    auto input  = inputBuffer.getArrayOfReadPointers();
    auto output = buffer.getArrayOfWritePointers();

    for (int i = 0; i < numSamples; i++)
    {
        float l = inputGain * input[0][i];
        float r = inputGain * input[1][i];

        l = leftEnvelope.process (l);
        r = rightEnvelope.process (r);

        if (stereoLink)
        {
            float linked = 0.5f * (Decibels::decibelsToGain (l) + Decibels::decibelsToGain (r));
            linked = Decibels::gainToDecibels (linked);

            auto gain = calcGain (linked);

            output[0][i] = gain * input[0][i] * outputGain;
            output[1][i] = gain * input[1][i] * outputGain;
        }
        else
        {
            auto lGain = calcGain (l);
            auto rGain = calcGain (r);

            output[0][i] = lGain * input[0][i] * outputGain;
            output[1][i] = rGain * input[1][i] * outputGain;
        }
    }
}

float Dynamics::calcGain (float v)
{
    float slope;
    
    if (type == compressor || type == limiter)
    {
        slope = (type == limiter) ? 1 : 1.0f - 1.0f / ratio;

        if (kneeWidth > 0 && v > (threshold - kneeWidth / 2.0) && v < threshold + kneeWidth / 2.0)
        {
            double x[2];
            double y[2];
            x[0] = threshold - kneeWidth / 2.0;
            x[1] = std::min (0.0, threshold + kneeWidth / 2.0);
            y[0] = 0;
            y[1] = slope;

            slope = float (Lagrange::interpolate (x, y, 2, double(v)));
        }
    }
    else
    {
        slope = (type == gate) ? -1 : (1.0f / ratio - 1.0f);

        if (kneeWidth > 0 && v > (threshold - kneeWidth / 2.0) && v < threshold + kneeWidth / 2.0)
        {
            double x[2];
            double y[2];
            x[0] = threshold - kneeWidth / 2.0;
            x[1] = std::min (0.0, threshold + kneeWidth / 2.0);
            y[0] = slope;
            y[1] = 0;

            slope = float (Lagrange::interpolate (x, y, 2, double(v)));
        }
    }
    
    float yG = std::min (0.0f, slope * (threshold - v));
    return std::pow (10.0f, yG / 20.0f);
}
