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

    for (auto e : envelopes)
        e->setSampleRate (sampleRate);

    reset();
}

void Dynamics::setNumChannels (int ch)
{
    channels = ch;
    
    while (envelopes.size() < channels)
    {
        auto e = new EnvelopeDetector();
        e->setSampleRate (sampleRate);
        envelopes.add (e);
    }
    while (envelopes.size() > channels)
        envelopes.removeLast();
}

void Dynamics::setParams (float attackS, float releaseS, float threshold_, float ratio_, float kneeWidth_)
{
    for (auto e : envelopes)
        e->setParams (attackS, releaseS, false, EnvelopeDetector::peak, true);

    threshold = threshold_;
    ratio = ratio_;
    kneeWidth = kneeWidth_;
}

void Dynamics::reset()
{
    for (auto e : envelopes)
        e->reset();
}

void Dynamics::process (AudioSampleBuffer& buffer, AudioSampleBuffer* envelopeOut)
{
    inputTracker.trackBuffer (buffer);
    
    int numSamples = buffer.getNumSamples();

    auto input  = buffer.getArrayOfReadPointers();
    auto output = buffer.getArrayOfWritePointers();
    auto env    = envelopeOut != nullptr ? envelopeOut->getArrayOfWritePointers() : nullptr;
    
    float peakReduction = 1.0f;

    for (int i = 0; i < numSamples; i++)
    {
        if (channelsLinked)
        {
            float linked = 0.0f;
            for (int c = 0; c < channels; c++)
            {
                float in = inputGain * input[c][i];

                in = envelopes[c]->process (in);

                linked += Decibels::decibelsToGain (in);
            }
            
            linked /= channels;
            
            if (env != nullptr)
                env[0][i] = linked;
            
            linked = Decibels::gainToDecibels (linked);

            auto gain = Decibels::decibelsToGain (calcCurve (linked) - linked);
            peakReduction = std::min (peakReduction, gain);
            
            for (int c = 0; c < channels; c++)
                output[c][i] = inputGain * gain * input[c][i] * outputGain;
        }
        else
        {
            for (int c = 0; c < channels; c++)
            {
                float in = inputGain * input[c][i];

                in = envelopes[c]->process (in);
                
                if (env != nullptr)
                    env[c][i] = Decibels::decibelsToGain (in);

                auto gain = Decibels::decibelsToGain (calcCurve (in) - in);
                peakReduction = std::min (peakReduction, gain);

                output[c][i] = inputGain * gain * input[c][i] * outputGain;
            }
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

        if (kneeWidth > 0 && dbIn >= (threshold - kneeWidth / 2.0) && dbIn <= threshold + kneeWidth / 2.0)
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
