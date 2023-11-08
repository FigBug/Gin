/*
MIT License

Copyright (c) 2023 Mike Jarmy

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
*/

#pragma once

//------------------------------------------------------------------------------
// PlateReverb is an implementation of the classic plate reverb algorithm
// described by Jon Dattorro.
//
// Dattorro, J. 1997. "Effect Design Part 1: Reverberators and Other Filters."
// Journal of the Audio Engineering Society, Vol. 45, No. 9
//
// https://ccrma.stanford.edu/~dattorro/EffectDesignPart1.pdf
//
// Parameters:
//
//    mix:        Dry/wet mix.
//    predelay:   Delay before reverb.
//    lowpass:    Apply a lowpass filter before reverb.
//    decay:      How quickly the reverb decays.
//    size:       The size of our imaginary plate.
//    damping:    How much high frequencies are filtered during reverb.
//
//------------------------------------------------------------------------------

//==============================================================================
/** Plate reverb from Dattorro's paper.
 */
template <class F, class I> class PlateReverb
{
  public:

    static constexpr F kMaxPredelay = F (0.1); // seconds
    static constexpr F kMaxSize = 2.0;

    PlateReverb() {}
    ~PlateReverb() {}

    // Set the sample rate.  Note that we are re-mallocing all of the various
    // delay lines here.
    void setSampleRate (F sampleRate_)
    {
        sampleRate = sampleRate_;

        // Ratio of our sample rate to the sample rate that is used in
        // Dattorro's paper.
        F r = F (sampleRate / 29761.0);

        // Predelay
        predelayLine.reset (new DelayLine ((I)std::ceil (sampleRate * kMaxPredelay)));

        // Lowpass filters
        lowpass.setSampleRate (sampleRate);
        leftTank.damping.setSampleRate (sampleRate);
        rightTank.damping.setSampleRate (sampleRate);

        // Diffusers
        diffusers[0].reset (new DelayAllpass ((I)std::ceil (142 * r), 0.75));
        diffusers[1].reset (new DelayAllpass ((I)std::ceil (107 * r), 0.75));
        diffusers[2].reset (new DelayAllpass ((I)std::ceil (379 * r), 0.625));
        diffusers[3].reset (new DelayAllpass ((I)std::ceil (277 * r), 0.625));

        // Tanks
        F maxModDepth = F (8.0 * kMaxSize * r);
        leftTank.resetDelayLines (
                                  (I)std::ceil (kMaxSize * 672 * r), (F)-0.7, // apf1
                                  maxModDepth,
                                  (I)std::ceil (kMaxSize * 4453 * r),      // del1
                                  (I)std::ceil (kMaxSize * 1800 * r), 0.5, // apf2
                                  (I)std::ceil (kMaxSize * 3720 * r)       // del2
        );
        rightTank.resetDelayLines (
                                   (I)std::ceil (kMaxSize * 908 * r), (F)-0.7, // apf1
                                   maxModDepth,
                                   (I)std::ceil (kMaxSize * 4217 * r),      // del1
                                   (I)std::ceil (kMaxSize * 2656 * r), 0.5, // apf2
                                   (I)std::ceil (kMaxSize * 3163 * r)       // del2
        );

        leftTank.lfo.setSampleRate (sampleRate);
        rightTank.lfo.setSampleRate (sampleRate);
        leftTank.lfo.setFrequency (1.0);
        rightTank.lfo.setFrequency ((F)0.95);

        // Tap points
        baseLeftTaps = {
            266 * r,  // rightTank.del1
            2974 * r, // rightTank.del1
            1913 * r, // rightTank.apf2
            1996 * r, // rightTank.del2
            1990 * r, // leftTank.del1
            187 * r,  // leftTank.apf2
            1066 * r, // leftTank.del2
        };
        baseRightTaps = {
            353 * r,  // leftTank.del1
            3627 * r, // leftTank.del1
            1228 * r, // leftTank.apf2
            2673 * r, // leftTank.del2
            2111 * r, // rightTank.del1
            335 * r,  // rightTank.apf2
            121 * r,  // rightTank.del2
        };
    }

    // Dry/wet mix.
    void setMix (F m /* [0, 1] */)
    {
        mix = clamp (m, 0.0, 1.0);
    }

    // Delay before reverb.
    void setPredelay (F pd /* in seconds, [0, 0.1] */)
    {
        predelay = clamp(pd, 0.0, kMaxPredelay) * sampleRate;
    }

    // Apply a lowpass filter before reverb.
    void setLowpass (F cutoff /* Hz */)
    {
        cutoff = clamp (cutoff, 16.0, 20000.0);
        lowpass.setCutoff (cutoff);
    }

    // How quickly the reverb decays.
    void setDecay (F dr /* [0, 1) */)
    {
        decayRate = clamp (dr, 0.0, (F)0.9999999);
        leftTank.setDecay (decayRate);
        rightTank.setDecay (decayRate);
    }

    // The size of our imaginary plate.
    //
    // The size parameter scales the delay time for all of the delay lines and
    // APFs in each tank, and for all of the tap points.
    //
    // Note that there is no size parameter in Dattorro's paper; it is an
    // extension to the original algorithm.
    void setSize (F sz /* [0, 2] */)
    {
        F sizeRatio = clamp(sz, 0.0, kMaxSize) / kMaxSize;

        // Scale the tank delays and APFs in each tank
        leftTank.setSizeRatio(sizeRatio);
        rightTank.setSizeRatio(sizeRatio);

        // Scale the taps
        for (I i = 0; i < kNumTaps; i++)
        {
            leftTaps[size_t (i)] = baseLeftTaps[size_t (i)] * sizeRatio;
            rightTaps[size_t (i)] = baseRightTaps[size_t (i)] * sizeRatio;
        }
    }

    // How much high frequencies are filtered during reverb.
    void setDamping (F cutoff /* Hz */)
    {
        cutoff = clamp (cutoff, 16.0, 20000.0);

        leftTank.damping.setCutoff (cutoff);
        rightTank.damping.setCutoff (cutoff);
    }

    // Process a stereo pair of samples.
    void process (F dryLeft, F dryRight, F* leftOut, F* rightOut)
    {
        // Note that this is "synthetic stereo".  We produce a stereo pair
        // of output samples based on the summed input.
        F sum = dryLeft + dryRight;

        // Predelay
        sum = predelayLine->tapAndPush (predelay, sum);

        // Input lowpass
        sum = lowpass.process (sum);

        // Diffusers
        sum = diffusers[0]->process (sum, (F)diffusers[0]->getSize());
        sum = diffusers[1]->process (sum, (F)diffusers[1]->getSize());
        sum = diffusers[2]->process (sum, (F)diffusers[2]->getSize());
        sum = diffusers[3]->process (sum, (F)diffusers[3]->getSize());

        // Tanks
        F leftIn = sum + rightTank.out * decayRate;
        F rightIn = sum + leftTank.out * decayRate;
        leftTank.process(leftIn);
        rightTank.process(rightIn);

        // Tap for output
        F wetLeft = rightTank.del1->tap (leftTaps[0])   //  266
                    + rightTank.del1->tap (leftTaps[1]) // 2974
                    - rightTank.apf2->tap (leftTaps[2]) // 1913
                    + rightTank.del2->tap (leftTaps[3]) // 1996
                    - leftTank.del1->tap (leftTaps[4])  // 1990
                    - leftTank.apf2->tap (leftTaps[5])  //  187
                    - leftTank.del2->tap (leftTaps[6]); // 1066

        F wetRight = leftTank.del1->tap (rightTaps[0])     //  353
                     + leftTank.del1->tap (rightTaps[1])   // 3627
                     - leftTank.apf2->tap (rightTaps[2])   // 1228
                     + leftTank.del2->tap (rightTaps[3])   // 2673
                     - rightTank.del1->tap (rightTaps[4])  // 2111
                     - rightTank.apf2->tap (rightTaps[5])  //  335
                     - rightTank.del2->tap (rightTaps[6]); //  121

        // Mix
        *leftOut = dryLeft * (1 - mix) + wetLeft * mix;
        *rightOut = dryRight * (1 - mix) + wetRight * mix;
    }

    void process (F* l, F* r, I num)
    {
        for (auto i = 0; i < num; i++)
            process (l[i], r[i], &l[i], &r[i]);
    }

    void reset()
    {
        if (predelayLine)
            predelayLine->reset();

        lowpass.reset();

        for (auto& d : diffusers)
            if (d)
                d->reset();

        leftTank.reset();
        rightTank.reset();
    }

  private:

    //--------------------------------------------------------------
    // OnePoleFilter
    //--------------------------------------------------------------

    class OnePoleFilter
    {
    public:
        OnePoleFilter() {}
        ~OnePoleFilter() {}

        void setSampleRate (F sampleRate_)
        {
            sampleRate = sampleRate_;
            recalc();
        }

        void setCutoff (F cutoff_ /* Hz */)
        {
            cutoff = cutoff_;
            recalc();
        }

        F process(F x)
        {
            z = x * a + z * b;
            return z;
        }

        void reset()
        {
            a = 0;
            b = 0;
            z = 0;
        }

    private:
        F sampleRate = 1;
        F cutoff = 0;

        F a = 0;
        F b = 0;
        F z = 0;

        void recalc()
        {
            b = std::exp (-2 * juce::MathConstants<F>::pi * cutoff / sampleRate);
            a = 1 - b;
        }
    };

    //--------------------------------------------------------------
    // DelayLine
    //--------------------------------------------------------------

    class DelayLine
    {
    public:
        DelayLine (I size_)
            : size (size_)
        {

            // For speed, create a bigger buffer than we really need.
            I bufferSize = ceilPowerOfTwo (size);
            buffer.reset (new F[size_t (bufferSize)]);
            std::memset (buffer.get(), 0, size_t (bufferSize) * sizeof(F));

            mask = bufferSize - 1;

            writeIdx = 0;
        }

        ~DelayLine() {}

        inline void push(F val)
        {
            buffer[size_t (writeIdx++)] = val;
            writeIdx &= mask;
        }

        inline F tap(F delay /* samples */)
        {
            // We always want to be able to properly handle any delay value that
            // gets passed in here, without going past the original size.
            assert(delay <= size);

            I d = (I)delay;
            F frac = 1 - (delay - d);

            I readIdx = (writeIdx - 1) - d;
            F a = buffer[size_t ((readIdx - 1) & mask)];
            F b = buffer[size_t (readIdx & mask)];

            return a + (b - a) * frac;
        }

        // This does read-before-write.
        inline F tapAndPush (F delay, F val)
        {
            F out = tap (delay);
            push (val);
            return out;
        }

        void reset()
        {
            std::memset (buffer.get(), 0, size_t (ceilPowerOfTwo (size)) * sizeof (F));
            writeIdx = 0;
        }

        inline I getSize() { return size; }

    private:
        const I size;

        std::unique_ptr<F[]> buffer;
        I mask;

        I writeIdx;

        static I ceilPowerOfTwo (I n)
        {
            return (I)std::pow (2, std::ceil (std::log(n) / std::log (2)));
        }
    };

    //------------------------------------------
    // DelayAllpass
    //------------------------------------------

    class DelayAllpass
    {
    public:
        DelayAllpass (I size_, F gain_) : delayLine(size_), gain(gain_) {}

        ~DelayAllpass() {}

        inline F process (F x, F delay)
        {
            F wd = delayLine.tap(delay);
            F w = x + gain * wd;
            F y = -gain * w + wd;
            delayLine.push(w);
            return y;
        }

        inline void setGain (F gain_) { gain = gain_; }

        inline F tap (F delay) { return delayLine.tap(delay); }

        inline I getSize() { return delayLine.getSize(); }

        void reset() { delayLine.reset(); }

      private:

        DelayLine delayLine;
        F gain;
    };

    //--------------------------------------------------------------
    // Lfo
    //--------------------------------------------------------------

    class Lfo
    {
    public:
        Lfo() {}
        ~Lfo() {}

        void setSampleRate (F sampleRate_)
        {
            sampleRate = sampleRate_;
            recalc();
        }

        void setFrequency (F freq_)
        {
            freq = freq_;
            recalc();
        }

        inline F process()
        {
            F out = -FastMath<F>::fastSin(phase);

            phase += phaseInc;
            if (phase > juce::MathConstants<F>::pi)
                phase = -juce::MathConstants<F>::pi;

            return out;
        }

        void reset()
        {
            phase = 0;
        }

    private:
        F sampleRate = 1;
        F freq = 0;

        F phaseInc = 0;
        F phase = F (-juce::MathConstants<F>::pi);

        void recalc()
        {
            phaseInc = freq / sampleRate;
            phaseInc *= 2 * juce::MathConstants<F>::pi;
        }
    };

    //------------------------------------------
    // Tank
    //------------------------------------------

    class Tank
    {
    public:

        Tank() {}
        ~Tank() {}

        void resetDelayLines (
            I apf1Size_, F apf1Gain_, // First APF
            F maxModDepth_,
            I delay1Size_,            // First delay
            I apf2Size_, F apf2Gain_, // Second APF
            I delay2Size_             // Second delay
        ) {
            apf1Size = apf1Size_;
            maxModDepth = maxModDepth_;
            F maxApf1Size = apf1Size + maxModDepth + 1;
            apf1.reset(new DelayAllpass (I (maxApf1Size), apf1Gain_));

            del1.reset(new DelayLine(delay1Size_));
            apf2.reset(new DelayAllpass(apf2Size_, apf2Gain_));
            del2.reset(new DelayLine(delay2Size_));

            // We've changed the various delay line sizes and associated values,
            // so update the sizeRatio values too.
            recalcSizeRatio();
        }

        void setDecay (F decayRate_)
        {
            decayRate = decayRate_;
            apf2->setGain (clamp (F (decayRate + 0.15), 0.25, 0.5));
        }

        void setSizeRatio (F sizeRatio_)
        {
            sizeRatio = sizeRatio_;
            recalcSizeRatio();
        }

        void process (F val)
        {
            // APF1: "Controls density of tail."
            val = apf1->process(val, apf1Delay + lfo.process() * modDepth);
            val = del1->tapAndPush(del1Delay, val);

            val = damping.process(val);
            val *= decayRate;

            // APF2: "Decorrelates tank signals."
            val = apf2->process(val, apf2Delay);
            val = del2->tapAndPush(del2Delay, val);

            out = val;
        }

        void reset()
        {
            if (apf1) apf1->reset();
            if (apf2) apf2->reset();
            if (del1) del1->reset();
            if (del2) del2->reset();
            damping.reset();
            lfo.reset();
        }

        F out = 0.0;

        std::unique_ptr<DelayAllpass> apf1 = nullptr;
        std::unique_ptr<DelayAllpass> apf2 = nullptr;
        std::unique_ptr<DelayLine> del1 = nullptr;
        std::unique_ptr<DelayLine> del2 = nullptr;
        OnePoleFilter damping;
        Lfo lfo;

    private:
        I apf1Size = 0;
        F maxModDepth = 0;
        F modDepth = 0;

        F apf1Delay = 0;
        F apf2Delay = 0;
        F del1Delay = 0;
        F del2Delay = 0;

        F decayRate = 0;
        F sizeRatio = 0;

        void recalcSizeRatio() {

            apf1Delay = apf1Size * sizeRatio;
            modDepth = maxModDepth * sizeRatio;

            apf2Delay = apf2->getSize() * sizeRatio;
            del1Delay = del1->getSize() * sizeRatio;
            del2Delay = del2->getSize() * sizeRatio;
        }
    };

    //--------------------------------------------------------------
    //--------------------------------------------------------------
    //--------------------------------------------------------------

    F sampleRate = 1.0;

    F mix = 0.0;
    F predelay = 0.0;
    F decayRate = 0.0;

    std::unique_ptr<DelayLine> predelayLine = nullptr;
    OnePoleFilter lowpass;
    std::array<std::unique_ptr<DelayAllpass>, 4> diffusers = {
        nullptr, nullptr, nullptr, nullptr};

    Tank leftTank;
    Tank rightTank;

    static const I kNumTaps = 7;
    std::array<F, kNumTaps> baseLeftTaps = {};
    std::array<F, kNumTaps> baseRightTaps = {};
    std::array<F, kNumTaps> leftTaps = {};
    std::array<F, kNumTaps> rightTaps = {};

    static inline F clamp (F val, F low, F high)
    {
        return std::min (std::max (val, low), high);
    }
};
