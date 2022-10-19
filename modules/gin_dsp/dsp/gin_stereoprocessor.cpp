/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

StereoProcessor::StereoProcessor()
{
}

//
// width1In      [ 0 - 1 ]
// centerFader1  [ 0 - 1 ]
// pan1          [-1 - 1 ]
// rotationFader [ 0 - 1 ]
// pan2          [-1 - 1 ]
// centerFader2  [ 0 - 1 ]
// width2In      [ 0 - 1 ]
// outVolFader   [ 0 - 1 ]
//
void StereoProcessor::setParameters (float width1In, float centerFader1, float pan1, float rotationFader, float pan2, float centerFader2, float width2In, float outVolFader)
{
    //
    // Width
    //
    width1 = width1In;
    widthCoeff1 = std::max (width1, 1.0f);

    //
    // Centre
    //
    center1 = std::min (centerFader1 * 2, 1.0f);
    side1 = 2.0f - centerFader1 * 2.0f;

    //
    // Pan 1
    //
    pan1Left = (pan1 > 0 ? 1 - pan1 : 1);
    pan1Right = (pan1 < 0 ? 1 + pan1 : 1);

    //
    // Rotation
    //
    rotation = (rotationFader - 0.5f) * 180.0f * 0.017453292f;

    //
    // Pan 2
    //
    pan2Left = (pan2 > 0 ? 1 - pan2 : 1);
    pan2Right = (pan2 < 0 ? 1 + pan2 : 1);

    //
    // Centre
    //
    center2 = std::min (centerFader2 * 2, 1.0f);
    side2 = 2.0f - centerFader2 * 2.0f;

    //
    // Width
    //
    width2 = width2In;
    widthCoeff2 = std::max (width2, 1.0f);

    //
    // Output
    //
    auto dB2Amp = [] (float dB)
    {
        constexpr auto cDBAmp = 0.115524530f;
        return (std::exp (dB * cDBAmp));
    };

    outVol = dB2Amp (sign (outVolFader - 0.5f) * std::pow ((outVolFader - 0.5f) * 2.0f, 2.0f) * 18.0f);
}

void StereoProcessor::process (juce::AudioSampleBuffer& buffer)
{
    const float* in1  = buffer.getReadPointer (0);
    const float* in2  = buffer.getReadPointer (1);
    float* out1 = buffer.getWritePointer (0);
    float* out2 = buffer.getWritePointer (1);

    int sampleFrames = buffer.getNumSamples();

    while (--sampleFrames >= 0)
    {
        // Width 1
        auto mono   = (((*in1) + (*in2)) / 2.0f) * center1;
        auto stereo = ( (*in1) - (*in2)        ) * side1;
        (*out1) = (mono + stereo * width1) / widthCoeff1;
        (*out2) = (mono - stereo * width1) / widthCoeff1;

        // Pan 1
        (*out1) *= pan1Left;
        (*out2) *= pan1Right;

        // Rotate
        if (rotation != 0.0f)
        {
            auto angle = 0.0f;

            if ((*out2) == 0.0f)
            {
                if ((*out1) > 0)
                    angle = juce::MathConstants<float>::pi / 2.0f;
                else
                    angle = juce::MathConstants<float>::pi * 1.5f;
            }
            else if ((*out1) == 0.0f)
            {
                if ((*out2) > 0)
                    angle = 0.0f;
                else
                    angle = juce::MathConstants<float>::pi;
            }
            else
            {
                auto s1 = sign ((*out1));
                auto s2 = sign ((*out2));

                angle = std::atan ((*out1) / (*out2));

                if ((s1 == 1 && s2 == -1) || (s1 == -1 && s2 == -1))
                    angle += juce::MathConstants<float>::pi;

                if ((s1 == -1 && s2 == 1))
                    angle += juce::MathConstants<float>::pi;
            }

            auto radius = std::sqrt (std::pow ((*out1), 2.0f) + std::pow ((*out2), 2.0f));
            angle -= rotation;
            (*out1) = std::sin (angle) * radius;
            (*out2) = std::cos (angle) * radius;
        }

        // Pan 2
        (*out1) *= pan2Left;
        (*out2) *= pan2Right;

        // Width 2
        mono   = (((*out1) + (*out2)) / 2.0f) * center2;
        stereo = ( (*out1) - (*out2)        ) * side2;
        (*out1) = (mono + stereo * width2) / widthCoeff2;
        (*out2) = (mono - stereo * width2) / widthCoeff2;

        *out1 *= outVol;
        *out2 *= outVol;

        out1++;
        out2++;
        in1++;
        in2++;
    }
}
