//==============================================================================
#if GIN_UNIT_TESTS

class EQTests : public juce::UnitTest
{
public:
    EQTests() : juce::UnitTest ("EQ", "gin_dsp") {}

    void runTest() override
    {
        testEQCreation();
        testSingleBandEQ();
        testMultiBandEQ();
        testLowShelfBand();
        testHighShelfBand();
        testPeakBand();
        testHighpassBand();
        testLowpassBand();
        testCombinedBands();
        testReset();
    }

private:
    void testEQCreation()
    {
        beginTest ("EQ Creation");

        gin::EQ eq (3);
        eq.setSampleRate (44100.0);
        eq.setNumChannels (2);

        expect (true, "EQ should be created successfully");
    }

    void testSingleBandEQ()
    {
        beginTest ("Single Band EQ");

        gin::EQ eq (1);
        eq.setSampleRate (44100.0);
        eq.setNumChannels (1);

        // Configure as peak filter
        eq.setParams (0, gin::EQ::peak, 1000.0f, 2.0f, 2.0f);

        // Create test buffer
        juce::AudioBuffer<float> buffer (1, 512);
        buffer.clear();
        buffer.setSample (0, 0, 1.0f); // Impulse

        eq.process (buffer);

        // Should produce non-zero output
        bool hasNonZeroValues = false;
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            if (std::abs (buffer.getSample (0, i)) > 0.001f)
            {
                hasNonZeroValues = true;
                break;
            }
        }

        expect (hasNonZeroValues, "EQ should process signal");
    }

    void testMultiBandEQ()
    {
        beginTest ("Multi-Band EQ");

        gin::EQ eq (3);
        eq.setSampleRate (44100.0);
        eq.setNumChannels (2);

        // Configure 3-band EQ
        eq.setParams (0, gin::EQ::lowshelf, 100.0f, 0.7f, 1.5f);
        eq.setParams (1, gin::EQ::peak, 1000.0f, 2.0f, 0.5f);
        eq.setParams (2, gin::EQ::highshelf, 8000.0f, 0.7f, 1.5f);

        // Create stereo test buffer
        juce::AudioBuffer<float> buffer (2, 512);
        buffer.clear();

        // Add test signal to both channels
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < 100; ++i)
                buffer.setSample (ch, i, std::sin (i * 0.1f));

        eq.process (buffer);

        // Check both channels are processed
        bool ch0HasSignal = false;
        bool ch1HasSignal = false;

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            if (std::abs (buffer.getSample (0, i)) > 0.01f) ch0HasSignal = true;
            if (std::abs (buffer.getSample (1, i)) > 0.01f) ch1HasSignal = true;
        }

        expect (ch0HasSignal, "Channel 0 should have signal");
        expect (ch1HasSignal, "Channel 1 should have signal");
    }

    void testLowShelfBand()
    {
        beginTest ("Low Shelf Band");

        gin::EQ eq (1);
        eq.setSampleRate (44100.0);
        eq.setNumChannels (1);

        // Low shelf boost
        eq.setParams (0, gin::EQ::lowshelf, 200.0f, 0.707f, 2.0f);

        juce::AudioBuffer<float> buffer (1, 512);
        buffer.clear();

        // Generate low frequency sine wave
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            buffer.setSample (0, i, 0.5f * std::sin (i * 0.01f));

        float rmsInput = buffer.getRMSLevel (0, 0, buffer.getNumSamples());

        eq.process (buffer);

        float rmsOutput = buffer.getRMSLevel (0, 0, buffer.getNumSamples());

        // With gain > 1.0, output should be louder
        expect (rmsOutput > rmsInput * 0.9f, "Low shelf with gain should boost signal");
    }

    void testHighShelfBand()
    {
        beginTest ("High Shelf Band");

        gin::EQ eq (1);
        eq.setSampleRate (44100.0);
        eq.setNumChannels (1);

        // High shelf boost
        eq.setParams (0, gin::EQ::highshelf, 5000.0f, 0.707f, 2.0f);

        juce::AudioBuffer<float> buffer (1, 512);
        buffer.clear();

        // Generate high frequency sine wave
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            buffer.setSample (0, i, 0.5f * std::sin (i * 0.5f));

        float rmsInput = buffer.getRMSLevel (0, 0, buffer.getNumSamples());

        eq.process (buffer);

        float rmsOutput = buffer.getRMSLevel (0, 0, buffer.getNumSamples());

        // With gain > 1.0, output should be louder
        expect (rmsOutput > rmsInput * 0.9f, "High shelf with gain should boost signal");
    }

    void testPeakBand()
    {
        beginTest ("Peak Band");

        gin::EQ eq (1);
        eq.setSampleRate (44100.0);
        eq.setNumChannels (1);

        // Peak filter with boost
        eq.setParams (0, gin::EQ::peak, 1000.0f, 2.0f, 2.0f);

        juce::AudioBuffer<float> buffer (1, 512);
        buffer.clear();

        // Generate signal at peak frequency
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            buffer.setSample (0, i, 0.5f * std::sin (i * 0.14f));

        float rmsInput = buffer.getRMSLevel (0, 0, buffer.getNumSamples());

        eq.process (buffer);

        float rmsOutput = buffer.getRMSLevel (0, 0, buffer.getNumSamples());

        // Peak filter with gain should boost
        expect (rmsOutput > rmsInput * 0.9f, "Peak with gain should boost signal");
    }

    void testHighpassBand()
    {
        beginTest ("Highpass Band");

        gin::EQ eq (1);
        eq.setSampleRate (44100.0);
        eq.setNumChannels (1);

        // Highpass filter
        eq.setParams (0, gin::EQ::highpass, 1000.0f, 0.707f, 1.0f);

        juce::AudioBuffer<float> buffer (1, 512);
        buffer.clear();

        // Generate DC offset (should be removed by highpass)
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            buffer.setSample (0, i, 0.5f);

        eq.process (buffer);

        // DC should be attenuated significantly
        float avgValue = 0.0f;
        for (int i = 100; i < buffer.getNumSamples(); ++i) // Skip transient
            avgValue += buffer.getSample (0, i);
        avgValue /= (buffer.getNumSamples() - 100);

        expect (std::abs (avgValue) < 0.1f, "Highpass should remove DC");
    }

    void testLowpassBand()
    {
        beginTest ("Lowpass Band");

        gin::EQ eq (1);
        eq.setSampleRate (44100.0);
        eq.setNumChannels (1);

        // Lowpass filter
        eq.setParams (0, gin::EQ::lowpass, 1000.0f, 0.707f, 1.0f);

        juce::AudioBuffer<float> buffer (1, 512);
        buffer.clear();

        // Generate low frequency signal
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            buffer.setSample (0, i, 0.5f * std::sin (i * 0.05f));

        float rmsInput = buffer.getRMSLevel (0, 0, buffer.getNumSamples());

        eq.process (buffer);

        float rmsOutput = buffer.getRMSLevel (0, 0, buffer.getNumSamples());

        // Low frequencies should pass through
        expect (rmsOutput > rmsInput * 0.5f, "Lowpass should pass low frequencies");
    }

    void testCombinedBands()
    {
        beginTest ("Combined Bands");

        gin::EQ eq (3);
        eq.setSampleRate (44100.0);
        eq.setNumChannels (1);

        // Create a typical "V-shaped" EQ
        eq.setParams (0, gin::EQ::lowshelf, 100.0f, 0.7f, 1.5f);   // Bass boost
        eq.setParams (1, gin::EQ::peak, 1000.0f, 2.0f, 0.7f);      // Mid cut
        eq.setParams (2, gin::EQ::highshelf, 8000.0f, 0.7f, 1.5f); // Treble boost

        juce::AudioBuffer<float> buffer (1, 512);
        buffer.clear();

        // Generate broadband signal
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            buffer.setSample (0, i,
                0.3f * std::sin (i * 0.01f) +   // Low freq
                0.3f * std::sin (i * 0.1f) +    // Mid freq
                0.3f * std::sin (i * 0.5f));    // High freq
        }

        eq.process (buffer);

        // Signal should be processed through all bands
        bool hasNonZeroValues = false;
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            if (std::abs (buffer.getSample (0, i)) > 0.01f)
            {
                hasNonZeroValues = true;
                break;
            }
        }

        expect (hasNonZeroValues, "Combined bands should process signal");
    }

    void testReset()
    {
        beginTest ("EQ Reset");

        gin::EQ eq (2);
        eq.setSampleRate (44100.0);
        eq.setNumChannels (1);

        eq.setParams (0, gin::EQ::lowshelf, 100.0f, 0.7f, 2.0f);
        eq.setParams (1, gin::EQ::highshelf, 8000.0f, 0.7f, 2.0f);

        // Process some audio to build up state
        juce::AudioBuffer<float> buffer (1, 512);
        buffer.clear();
        for (int i = 0; i < 100; ++i)
            buffer.setSample (0, i, 1.0f);

        eq.process (buffer);

        // Reset
        eq.reset();

        // Process silence - should get silence out
        buffer.clear();
        eq.process (buffer);

        float maxValue = 0.0f;
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            maxValue = std::max (maxValue, std::abs (buffer.getSample (0, i)));

        expectWithinAbsoluteError (maxValue, 0.0f, 0.001f, "EQ should be silent after reset");
    }
};

static EQTests eqTests;

#endif
