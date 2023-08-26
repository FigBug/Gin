/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2020 - Roland Rabien.

 MIT License

 Copyright (c) 2018 Chris Johnson

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 ==============================================================================
 */

#pragma once

//==============================================================================
/** Distortion based on AirWindows plugins
*/
class AirWindowsDistortion
{
public:
    AirWindowsDistortion ()
    {
        reset();
    }

    void setSampleRate (double sr)
    {
        sampleRate = sr;
    }

    void reset()
    {
        A = 0.2f;
        B = 0.0f;
        C = 1.0f;
        D = 1.0f;
        iirSampleAL = 0.0f;
        iirSampleBL = 0.0f;
        iirSampleAR = 0.0f;
        iirSampleBR = 0.0f;
        fpFlip = true;
        fpNShapeL = 0.0f;
        fpNShapeR = 0.0f;
    }

    void setParams (float density, float highpass, float output, float mix)
    {
        A = density;
        B = highpass;
        C = output;
        D = mix;
    }
    
    void process (juce::AudioSampleBuffer& buffer)
    {
        float* l = buffer.getWritePointer (0);
        float* r = buffer.getWritePointer (1);

        int sampleFrames = buffer.getNumSamples();

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= sampleRate;
        double density = (A*5.0)-1.0;
        double iirAmount = pow(B,3)/overallscale;
        double output = C;
        double wet = D;
        double dry = 1.0-wet;
        double bridgerectifier;
        double out = fabs(density);
        density = density * fabs(density);
        double count;

        long double inputSampleL;
        long double inputSampleR;
        long double drySampleL;
        long double drySampleR;

        while (--sampleFrames >= 0)
        {
            inputSampleL = *l;
            inputSampleR = *r;
            if (inputSampleL<1.2e-38 && -inputSampleL<1.2e-38) {
                static int noisesource = 0;
                //this declares a variable before anything else is compiled. It won't keep assigning
                //it to 0 for every sample, it's as if the declaration doesn't exist in this context,
                //but it lets me add this denormalization fix in a single place rather than updating
                //it in three different locations. The variable isn't thread-safe but this is only
                //a random seed and we can share it with whatever.
                noisesource = noisesource % 1700021; noisesource++;
                int residue = noisesource * noisesource;
                residue = residue % 170003; residue *= residue;
                residue = residue % 17011; residue *= residue;
                residue = residue % 1709; residue *= residue;
                residue = residue % 173; residue *= residue;
                residue = residue % 17;
                double applyresidue = residue;
                applyresidue *= 0.00000001;
                applyresidue *= 0.00000001;
                inputSampleL = applyresidue;
            }
            if (inputSampleR<1.2e-38 && -inputSampleR<1.2e-38) {
                static int noisesource = 0;
                noisesource = noisesource % 1700021; noisesource++;
                int residue = noisesource * noisesource;
                residue = residue % 170003; residue *= residue;
                residue = residue % 17011; residue *= residue;
                residue = residue % 1709; residue *= residue;
                residue = residue % 173; residue *= residue;
                residue = residue % 17;
                double applyresidue = residue;
                applyresidue *= 0.00000001;
                applyresidue *= 0.00000001;
                inputSampleR = applyresidue;
                //this denormalization routine produces a white noise at -300 dB which the noise
                //shaping will interact with to produce a bipolar output, but the noise is actually
                //all positive. That should stop any variables from going denormal, and the routine
                //only kicks in if digital black is input. As a final touch, if you save to 24-bit
                //the silence will return to being digital black again.
            }
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;

            if (fpFlip)
            {
                iirSampleAL = double ((iirSampleAL * (1.0 - iirAmount)) + (inputSampleL * iirAmount));
                inputSampleL -= iirSampleAL;
                iirSampleAR = double ((iirSampleAR * (1.0 - iirAmount)) + (inputSampleR * iirAmount));
                inputSampleR -= iirSampleAR;
            }
            else
            {
                iirSampleBL = double ((iirSampleBL * (1.0 - iirAmount)) + (inputSampleL * iirAmount));
                inputSampleL -= iirSampleBL;
                iirSampleBR = double ((iirSampleBR * (1.0 - iirAmount)) + (inputSampleR * iirAmount));
                inputSampleR -= iirSampleBR;
            }
            //highpass section
            fpFlip = !fpFlip;

            count = density;
            while (count > 1.0)
            {
                bridgerectifier = double (fabs(inputSampleL)*1.57079633);
                if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
                //max value for sine function
                bridgerectifier = sin(bridgerectifier);
                if (inputSampleL > 0.0) inputSampleL = bridgerectifier;
                else inputSampleL = -bridgerectifier;

                bridgerectifier = double (fabs(inputSampleR)*1.57079633);
                if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
                //max value for sine function
                bridgerectifier = sin(bridgerectifier);
                if (inputSampleR > 0.0) inputSampleR = bridgerectifier;
                else inputSampleR = -bridgerectifier;

                count = count - 1.0;
            }
            //we have now accounted for any really high density settings.

            while (out > 1.0) out = out - 1.0;

            bridgerectifier = double (fabs(inputSampleL)*1.57079633);
            if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
            //max value for sine function
            if (density > 0) bridgerectifier = sin(bridgerectifier);
            else bridgerectifier = 1-cos(bridgerectifier);
            //produce either boosted or starved version
            if (inputSampleL > 0) inputSampleL = (inputSampleL*(1-out))+(bridgerectifier*out);
            else inputSampleL = (inputSampleL*(1-out))-(bridgerectifier*out);
            //blend according to density control

            bridgerectifier = double (fabs(inputSampleR)*1.57079633);
            if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
            //max value for sine function
            if (density > 0) bridgerectifier = sin(bridgerectifier);
            else bridgerectifier = 1-cos(bridgerectifier);
            //produce either boosted or starved version
            if (inputSampleR > 0) inputSampleR = (inputSampleR*(1.0-out))+(bridgerectifier*out);
            else inputSampleR = (inputSampleR*(1.0-out))-(bridgerectifier*out);
            //blend according to density control

            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
            }
            if (wet < 1.0) {
                inputSampleL = (drySampleL * dry)+(inputSampleL * wet);
                inputSampleR = (drySampleR * dry)+(inputSampleR * wet);
            }
            //nice little output stage template: if we have another scale of floating point
            //number, we really don't want to meaninglessly multiply that by 1.0.

            //stereo 32 bit dither, made small and tidy.
            int expon; frexpf((float)inputSampleL, &expon);
            long double dither = (rand()/(RAND_MAX*7.737125245533627e+25))*pow(2,expon+62);
            inputSampleL += (dither-fpNShapeL); fpNShapeL = dither;
            frexpf((float)inputSampleR, &expon);
            dither = (rand()/(RAND_MAX*7.737125245533627e+25))*pow(2,expon+62);
            inputSampleR += (dither-fpNShapeR); fpNShapeR = dither;
            //end 32 bit dither

            *l = float (inputSampleL);
            *r = float (inputSampleR);

            l++;
            r++;
        }
    }

private:
    double sampleRate = 44100.0;
    long double fpNShapeL, fpNShapeR;
    double iirSampleAL, iirSampleBL, iirSampleAR, iirSampleBR;
    bool fpFlip;
    float A, B, C, D;
};
