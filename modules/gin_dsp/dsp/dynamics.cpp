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

void Dynamics::setParams (float attackS, float releaseS, float threshold_, float ratio_, float kneeWidth_)
{
    leftEnvelope.setParams (attackS, releaseS, false, EnvelopeDetector::peak, true);
    rightEnvelope.setParams (attackS, releaseS, false, EnvelopeDetector::peak, true);

    threshold = threshold_;
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
    inputTracker.trackBuffer (buffer);
    
    int numSamples = buffer.getNumSamples();

    auto input  = buffer.getArrayOfReadPointers();
    auto output = buffer.getArrayOfWritePointers();
    
    float peakReduction = 1.0f;

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

            auto gain = Decibels::decibelsToGain (calcCurve (linked) - linked);
            
            peakReduction = std::min (peakReduction, gain);

            output[0][i] = gain * input[0][i] * outputGain;
            output[1][i] = gain * input[1][i] * outputGain;
        }
        else
        {
            auto lGain = Decibels::decibelsToGain (calcCurve (l) - l);
            auto rGain = Decibels::decibelsToGain (calcCurve (r) - r);
            
            peakReduction = std::min (peakReduction, (1.0f - (lGain + rGain) / 2.0f));

            output[0][i] = lGain * input[0][i] * outputGain;
            output[1][i] = rGain * input[1][i] * outputGain;
        }
    }
    
    reductionTracker.trackSample (peakReduction);
    outputTracker.trackBuffer (buffer);
}

float Dynamics::calcCurve (float dbIn)
{
    if (type == compressor)
    {
        float dbOut = dbIn;

        if (kneeWidth > 0 && dbIn > (threshold - kneeWidth / 2.0) && dbIn < threshold + kneeWidth / 2.0)
            dbOut = dbIn + ((1.0f / ratio - 1.0f) * std::pow (dbIn - threshold + kneeWidth / 2.0f, 2.0f) / (2.0f * kneeWidth));
        else if (dbIn > threshold + kneeWidth / 2.0)
            dbOut = threshold + (dbIn - threshold) / ratio;

        return dbOut;
    }
    else if (type == limiter)
    {
        float dbOut = dbIn;

        if (kneeWidth > 0 && dbIn > (threshold - kneeWidth / 2.0) && dbIn < threshold + kneeWidth / 2.0)
            dbOut = dbIn + (1.0f * std::pow (dbIn - threshold + kneeWidth / 2.0f, 2.0f) / (2.0f * kneeWidth));
        else if (dbIn > threshold + kneeWidth / 2.0)
            dbOut = threshold;

        return dbOut;
    }
    else
    {
        // todo
        jassertfalse;
        return dbIn;
    }
}
