/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class OscillatorsTests : public juce::UnitTest
{
public:
    OscillatorsTests() : juce::UnitTest ("Oscillators", "gin_dsp") {}

    void runTest() override
    {
        testStereoOscillatorBasic();
        testStereoOscillatorWaveforms();
        testStereoOscillatorGain();
        testStereoOscillatorPulseWidth();
        testStereoOscillatorFold();
        testStereoOscillatorAsymmetry();
        testVoicedOscillatorSingleVoice();
        testVoicedOscillatorMultipleVoices();
        testVoicedOscillatorDetuning();
        testVoicedOscillatorPanning();
    }

private:
    void testStereoOscillatorBasic()
    {
        beginTest ("Stereo Oscillator - Basic");

        BandLimitedLookupTables bllt;
        StereoOscillator osc (bllt);
        osc.setSampleRate (44100.0);

        // Test noteOn with specific phase
        osc.noteOn (0.25f);

        StereoOscillator::Params params;
        params.wave = Wave::sine;

        juce::AudioBuffer<float> buffer (2, 100);
        osc.process (60.0f, params, buffer);

        // Check that audio was generated
        bool hasNonZero = false;
        for (int c = 0; c < buffer.getNumChannels(); c++)
        {
            for (int s = 0; s < buffer.getNumSamples(); s++)
            {
                if (std::abs (buffer.getSample (c, s)) > 0.001f)
                {
                    hasNonZero = true;
                    break;
                }
            }
        }

        expect (hasNonZero, "Oscillator should generate non-zero output");

        // Check that samples are within valid range
        for (int c = 0; c < buffer.getNumChannels(); c++)
        {
            for (int s = 0; s < buffer.getNumSamples(); s++)
            {
                float sample = buffer.getSample (c, s);
                expect (sample >= -1.0f && sample <= 1.0f,
                       "All samples should be in range [-1, 1]");
            }
        }
    }

    void testStereoOscillatorWaveforms()
    {
        beginTest ("Stereo Oscillator - Waveforms");

        BandLimitedLookupTables bllt;
        StereoOscillator osc (bllt);
        osc.setSampleRate (44100.0);
        osc.noteOn (0.0f);

        StereoOscillator::Params params;
        juce::AudioBuffer<float> buffer (2, 100);

        // Test different waveforms produce different output
        params.wave = Wave::sine;
        osc.noteOn (0.0f);
        osc.process (60.0f, params, buffer);
        float sineSample = buffer.getSample (0, 50);

        params.wave = Wave::sawUp;
        osc.noteOn (0.0f);
        osc.process (60.0f, params, buffer);
        float sawSample = buffer.getSample (0, 50);

        params.wave = Wave::square;
        osc.noteOn (0.0f);
        osc.process (60.0f, params, buffer);
        float squareSample = buffer.getSample (0, 50);

        // Waveforms should produce different values
        expect (std::abs (sineSample - sawSample) > 0.01f,
               "Sine and saw should produce different output");
        expect (std::abs (sineSample - squareSample) > 0.01f,
               "Sine and square should produce different output");

        // Test silence waveform
        params.wave = Wave::silence;
        osc.process (60.0f, params, buffer);
        bool isSilent = true;
        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            if (std::abs (buffer.getSample (0, s)) > 0.001f)
            {
                isSilent = false;
                break;
            }
        }
        expect (isSilent, "Silence waveform should produce no output");
    }

    void testStereoOscillatorGain()
    {
        beginTest ("Stereo Oscillator - Gain");

        BandLimitedLookupTables bllt;
        StereoOscillator osc (bllt);
        osc.setSampleRate (44100.0);
        osc.noteOn (0.0f);

        StereoOscillator::Params params;
        params.wave = Wave::sine;
        juce::AudioBuffer<float> buffer (2, 100);

        // Test left gain
        params.leftGain = 0.5f;
        params.rightGain = 1.0f;
        osc.noteOn (0.0f);
        osc.process (60.0f, params, buffer);

        float avgLeft = 0.0f;
        float avgRight = 0.0f;
        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            avgLeft += std::abs (buffer.getSample (0, s));
            avgRight += std::abs (buffer.getSample (1, s));
        }
        avgLeft /= buffer.getNumSamples();
        avgRight /= buffer.getNumSamples();

        expect (avgRight > avgLeft * 1.5f,
               "Right channel with higher gain should have higher amplitude");

        // Test zero gain
        params.leftGain = 0.0f;
        params.rightGain = 0.0f;
        osc.process (60.0f, params, buffer);

        bool isSilent = true;
        for (int c = 0; c < buffer.getNumChannels(); c++)
        {
            for (int s = 0; s < buffer.getNumSamples(); s++)
            {
                if (std::abs (buffer.getSample (c, s)) > 0.001f)
                {
                    isSilent = false;
                    break;
                }
            }
        }
        expect (isSilent, "Zero gain should produce silence");
    }

    void testStereoOscillatorPulseWidth()
    {
        beginTest ("Stereo Oscillator - Pulse Width");

        BandLimitedLookupTables bllt;
        StereoOscillator osc (bllt);
        osc.setSampleRate (44100.0);

        StereoOscillator::Params params;
        params.wave = Wave::pulse;
        juce::AudioBuffer<float> buffer (2, 1000);

        // Test different pulse widths produce different output
        params.pw = 0.25f;
        osc.noteOn (0.0f);
        osc.process (60.0f, params, buffer);
        float sample25 = buffer.getSample (0, 500);

        params.pw = 0.75f;
        osc.noteOn (0.0f);
        osc.process (60.0f, params, buffer);
        float sample75 = buffer.getSample (0, 500);

        expect (std::abs (sample25 - sample75) > 0.1f,
               "Different pulse widths should produce different output");
    }

    void testStereoOscillatorFold()
    {
        beginTest ("Stereo Oscillator - Wavefolding");

        BandLimitedLookupTables bllt;
        StereoOscillator osc (bllt);
        osc.setSampleRate (44100.0);
        osc.noteOn (0.0f);

        StereoOscillator::Params params;
        params.wave = Wave::sine;
        juce::AudioBuffer<float> buffer (2, 100);

        // Test no folding
        params.fold = 0.0f;
        osc.noteOn (0.0f);
        osc.process (60.0f, params, buffer);
        float unfoldedSample = buffer.getSample (0, 50);

        // Test with folding
        params.fold = 0.5f;
        osc.noteOn (0.0f);
        osc.process (60.0f, params, buffer);
        float foldedSample = buffer.getSample (0, 50);

        expect (std::abs (unfoldedSample - foldedSample) > 0.01f,
               "Wavefolding should alter the output");
    }

    void testStereoOscillatorAsymmetry()
    {
        beginTest ("Stereo Oscillator - Asymmetry");

        BandLimitedLookupTables bllt;
        StereoOscillator osc (bllt);
        osc.setSampleRate (44100.0);
        osc.noteOn (0.0f);

        StereoOscillator::Params params;
        params.wave = Wave::sine;
        juce::AudioBuffer<float> buffer (2, 100);

        // Test no asymmetry
        params.asym = 0.0f;
        osc.noteOn (0.0f);
        osc.process (60.0f, params, buffer);
        float symmetricSample = buffer.getSample (0, 50);

        // Test with asymmetry
        params.asym = 0.5f;
        osc.noteOn (0.0f);
        osc.process (60.0f, params, buffer);
        float asymmetricSample = buffer.getSample (0, 50);

        expect (std::abs (symmetricSample - asymmetricSample) > 0.01f,
               "Asymmetry should alter the output");
    }

    void testVoicedOscillatorSingleVoice()
    {
        beginTest ("Voiced Oscillator - Single Voice");

        BandLimitedLookupTables bllt;
        BLLTVoicedStereoOscillator osc (bllt, 8);
        osc.setSampleRate (44100.0);
        osc.noteOn (0.0f);

        VoicedStereoOscillatorParams params;
        params.voices = 1;
        params.wave = Wave::sine;
        params.gain = 1.0f;

        juce::AudioBuffer<float> buffer (2, 100);
        osc.process (60.0f, params, buffer);

        // Check that audio was generated
        bool hasNonZero = false;
        for (int c = 0; c < buffer.getNumChannels(); c++)
        {
            for (int s = 0; s < buffer.getNumSamples(); s++)
            {
                if (std::abs (buffer.getSample (c, s)) > 0.001f)
                {
                    hasNonZero = true;
                    break;
                }
            }
        }

        expect (hasNonZero, "Single voice should generate output");
    }

    void testVoicedOscillatorMultipleVoices()
    {
        beginTest ("Voiced Oscillator - Multiple Voices");

        BandLimitedLookupTables bllt;
        BLLTVoicedStereoOscillator osc (bllt, 8);
        osc.setSampleRate (44100.0);

        VoicedStereoOscillatorParams params;
        params.wave = Wave::sine;
        params.gain = 1.0f;
        juce::AudioBuffer<float> buffer (2, 100);

        // Test with 4 voices
        params.voices = 4;
        osc.noteOn (0.0f);
        osc.process (60.0f, params, buffer);

        bool hasNonZero = false;
        for (int c = 0; c < buffer.getNumChannels(); c++)
        {
            for (int s = 0; s < buffer.getNumSamples(); s++)
            {
                if (std::abs (buffer.getSample (c, s)) > 0.001f)
                {
                    hasNonZero = true;
                    break;
                }
            }
        }

        expect (hasNonZero, "Multiple voices should generate output");

        // Verify output is within valid range (gain compensation should prevent clipping)
        for (int c = 0; c < buffer.getNumChannels(); c++)
        {
            for (int s = 0; s < buffer.getNumSamples(); s++)
            {
                float sample = buffer.getSample (c, s);
                expect (sample >= -2.0f && sample <= 2.0f,
                       "Multi-voice output should be reasonable");
            }
        }
    }

    void testVoicedOscillatorDetuning()
    {
        beginTest ("Voiced Oscillator - Detuning");

        BandLimitedLookupTables bllt;
        BLLTVoicedStereoOscillator osc (bllt, 8);
        osc.setSampleRate (44100.0);

        VoicedStereoOscillatorParams params;
        params.voices = 3;
        params.wave = Wave::sine;
        params.gain = 1.0f;
        juce::AudioBuffer<float> buffer (2, 1000);

        // Test without detuning
        params.detune = 0.0f;
        osc.noteOn (0.0f);
        osc.process (60.0f, params, buffer);

        // Calculate RMS
        float rmsNoDetune = 0.0f;
        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            float sample = buffer.getSample (0, s);
            rmsNoDetune += sample * sample;
        }
        rmsNoDetune = std::sqrt (rmsNoDetune / buffer.getNumSamples());

        // Test with detuning (voices should interfere, creating some beating)
        params.detune = 0.5f; // 0.5 semitones
        osc.noteOn (0.0f);
        osc.process (60.0f, params, buffer);

        float rmsWithDetune = 0.0f;
        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            float sample = buffer.getSample (0, s);
            rmsWithDetune += sample * sample;
        }
        rmsWithDetune = std::sqrt (rmsWithDetune / buffer.getNumSamples());

        // Both should produce significant output
        expect (rmsNoDetune > 0.1f, "No detune should have output");
        expect (rmsWithDetune > 0.1f, "With detune should have output");
    }

    void testVoicedOscillatorPanning()
    {
        beginTest ("Voiced Oscillator - Panning and Spread");

        BandLimitedLookupTables bllt;
        BLLTVoicedStereoOscillator osc (bllt, 8);
        osc.setSampleRate (44100.0);

        VoicedStereoOscillatorParams params;
        params.voices = 1;
        params.wave = Wave::sine;
        params.gain = 1.0f;
        juce::AudioBuffer<float> buffer (2, 100);

        // Test hard left pan
        params.pan = -1.0f;
        osc.noteOn (0.0f);
        osc.process (60.0f, params, buffer);

        float leftRMS = 0.0f;
        float rightRMS = 0.0f;
        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            leftRMS += buffer.getSample (0, s) * buffer.getSample (0, s);
            rightRMS += buffer.getSample (1, s) * buffer.getSample (1, s);
        }
        leftRMS = std::sqrt (leftRMS / buffer.getNumSamples());
        rightRMS = std::sqrt (rightRMS / buffer.getNumSamples());

        expect (leftRMS > rightRMS * 2.0f,
               "Hard left pan should have more energy on left channel");

        // Test hard right pan
        params.pan = 1.0f;
        osc.noteOn (0.0f);
        osc.process (60.0f, params, buffer);

        leftRMS = 0.0f;
        rightRMS = 0.0f;
        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            leftRMS += buffer.getSample (0, s) * buffer.getSample (0, s);
            rightRMS += buffer.getSample (1, s) * buffer.getSample (1, s);
        }
        leftRMS = std::sqrt (leftRMS / buffer.getNumSamples());
        rightRMS = std::sqrt (rightRMS / buffer.getNumSamples());

        expect (rightRMS > leftRMS * 2.0f,
               "Hard right pan should have more energy on right channel");
    }
};

static OscillatorsTests oscillatorsTests;

#endif
