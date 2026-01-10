//==============================================================================
#if GIN_UNIT_TESTS

class GraphicEQTests : public juce::UnitTest
{
public:
    GraphicEQTests() : juce::UnitTest ("GraphicEQ", "gin_dsp") {}

    void runTest() override
    {
        testCreation();
        testBandConfigurations();
        testBandFrequencies();
        testPerChannelGain();
        testFlatten();
        testSetAllBandGains();
        testProcessing();
        testFrequencyResponse();
        testMultiChannel();
        testReset();
    }

private:
    void testCreation()
    {
        beginTest ("GraphicEQ Creation");

        gin::GraphicEQ eq5 (gin::GraphicEQ::Bands5);
        gin::GraphicEQ eq7 (gin::GraphicEQ::Bands7);
        gin::GraphicEQ eq10 (gin::GraphicEQ::Bands10);
        gin::GraphicEQ eq15 (gin::GraphicEQ::Bands15);
        gin::GraphicEQ eq31 (gin::GraphicEQ::Bands31);

        expectEquals (eq5.getNumBands(), 5, "Should have 5 bands");
        expectEquals (eq7.getNumBands(), 7, "Should have 7 bands");
        expectEquals (eq10.getNumBands(), 10, "Should have 10 bands");
        expectEquals (eq15.getNumBands(), 15, "Should have 15 bands");
        expectEquals (eq31.getNumBands(), 31, "Should have 31 bands");
    }

    void testBandConfigurations()
    {
        beginTest ("Band Configurations");

        gin::GraphicEQ eq5 (gin::GraphicEQ::Bands5);
        eq5.setNumChannels (1);
        eq5.setSampleRate (44100.0);

        // Check 5-band frequencies
        expectWithinAbsoluteError (eq5.getBandFrequency (0), 60.0f, 0.1f, "Band 0 should be 60Hz");
        expectWithinAbsoluteError (eq5.getBandFrequency (2), 1000.0f, 0.1f, "Band 2 should be 1kHz");
        expectWithinAbsoluteError (eq5.getBandFrequency (4), 16000.0f, 0.1f, "Band 4 should be 16kHz");

        gin::GraphicEQ eq10 (gin::GraphicEQ::Bands10);
        eq10.setNumChannels (1);

        // Check 10-band frequencies
        expectWithinAbsoluteError (eq10.getBandFrequency (0), 31.5f, 0.1f, "Band 0 should be 31.5Hz");
        expectWithinAbsoluteError (eq10.getBandFrequency (5), 1000.0f, 0.1f, "Band 5 should be 1kHz");
        expectWithinAbsoluteError (eq10.getBandFrequency (9), 16000.0f, 0.1f, "Band 9 should be 16kHz");
    }

    void testBandFrequencies()
    {
        beginTest ("Band Frequencies - 31 Band");

        gin::GraphicEQ eq (gin::GraphicEQ::Bands31);
        eq.setNumChannels (1);

        // Check some key frequencies for 31-band
        expectWithinAbsoluteError (eq.getBandFrequency (0), 20.0f, 0.1f, "Band 0 should be 20Hz");
        expectWithinAbsoluteError (eq.getBandFrequency (17), 1000.0f, 0.1f, "Band 17 should be 1kHz");
        expectWithinAbsoluteError (eq.getBandFrequency (30), 20000.0f, 0.1f, "Band 30 should be 20kHz");
    }

    void testPerChannelGain()
    {
        beginTest ("Per-Channel Gain");

        gin::GraphicEQ eq (gin::GraphicEQ::Bands10);
        eq.setNumChannels (2);
        eq.setSampleRate (44100.0);

        // Set different gains per channel
        eq.setBandGain (0, 5, 6.0f);   // Channel 0, 1kHz: +6dB
        eq.setBandGain (1, 5, -3.0f);  // Channel 1, 1kHz: -3dB

        expectWithinAbsoluteError (eq.getBandGain (0, 5), 6.0f, 0.001f, "Channel 0 band 5 should be 6dB");
        expectWithinAbsoluteError (eq.getBandGain (1, 5), -3.0f, 0.001f, "Channel 1 band 5 should be -3dB");

        // Verify channels are independent
        expect (std::abs (eq.getBandGain (0, 5) - eq.getBandGain (1, 5)) > 1.0f, "Channels should have different gains");
    }

    void testFlatten()
    {
        beginTest ("Flatten");

        gin::GraphicEQ eq (gin::GraphicEQ::Bands10);
        eq.setNumChannels (2);
        eq.setSampleRate (44100.0);

        // Set some gains
        eq.setBandGain (0, 0, 6.0f);
        eq.setBandGain (0, 5, -3.0f);
        eq.setBandGain (1, 3, 12.0f);

        // Flatten channel 0 only
        eq.flatten (0);

        // Channel 0 should be flat
        for (int b = 0; b < eq.getNumBands(); ++b)
            expectWithinAbsoluteError (eq.getBandGain (0, b), 0.0f, 0.001f, "Channel 0 bands should be 0dB after flatten");

        // Channel 1 should retain its gain
        expectWithinAbsoluteError (eq.getBandGain (1, 3), 12.0f, 0.001f, "Channel 1 band 3 should still be 12dB");

        // Flatten all
        eq.flattenAll();

        for (int c = 0; c < eq.getNumChannels(); ++c)
            for (int b = 0; b < eq.getNumBands(); ++b)
                expectWithinAbsoluteError (eq.getBandGain (c, b), 0.0f, 0.001f, "All bands should be 0dB after flattenAll");
    }

    void testSetAllBandGains()
    {
        beginTest ("Set All Band Gains");

        gin::GraphicEQ eq (gin::GraphicEQ::Bands10);
        eq.setNumChannels (2);
        eq.setSampleRate (44100.0);

        float gains[10];
        for (int i = 0; i < 10; ++i)
            gains[i] = static_cast<float> (i - 5); // -5 to +4

        eq.setAllBandGains (0, gains);

        for (int i = 0; i < 10; ++i)
            expectWithinAbsoluteError (eq.getBandGain (0, i), gains[i], 0.001f, "Band gain should match");

        // Channel 1 should still be 0
        for (int i = 0; i < 10; ++i)
            expectWithinAbsoluteError (eq.getBandGain (1, i), 0.0f, 0.001f, "Channel 1 should be unchanged");
    }

    void testProcessing()
    {
        beginTest ("Processing");

        gin::GraphicEQ eq (gin::GraphicEQ::Bands10);
        eq.setNumChannels (1);
        eq.setSampleRate (44100.0);

        // Boost all bands by 6dB
        for (int i = 0; i < eq.getNumBands(); ++i)
            eq.setBandGain (0, i, 6.0f);

        juce::AudioBuffer<float> buffer (1, 512);
        buffer.clear();

        // Generate broadband signal
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            buffer.setSample (0, i, 0.3f * std::sin (i * 0.1f));

        float rmsInput = buffer.getRMSLevel (0, 0, buffer.getNumSamples());

        eq.process (buffer);

        float rmsOutput = buffer.getRMSLevel (0, 0, buffer.getNumSamples());

        // With boost, output should be louder
        expect (rmsOutput > rmsInput, "Output should be boosted");
    }

    void testFrequencyResponse()
    {
        beginTest ("Frequency Response");

        gin::GraphicEQ eq (gin::GraphicEQ::Bands10);
        eq.setNumChannels (2);
        eq.setSampleRate (44100.0);

        // Boost 1kHz band (index 5) on channel 0
        eq.setBandGain (0, 5, 6.0f);

        // Channel 0 should have boosted response at 1kHz
        float response0 = eq.getFrequencyResponse (0, 1000.0f);
        expect (response0 > 1.5f, "Channel 0 response at 1kHz should be boosted");

        // Channel 1 should be flat
        float response1 = eq.getFrequencyResponse (1, 1000.0f);
        expectWithinAbsoluteError (response1, 1.0f, 0.1f, "Channel 1 response should be near unity");

        // Test dB version
        float responseDb = eq.getFrequencyResponseDb (0, 1000.0f);
        expect (responseDb > 3.0f, "Channel 0 response in dB should show boost");
    }

    void testMultiChannel()
    {
        beginTest ("Multi-Channel Processing");

        gin::GraphicEQ eq (gin::GraphicEQ::Bands10);
        eq.setNumChannels (4);
        eq.setSampleRate (44100.0);

        // Set different gains per channel at 1kHz band
        eq.setBandGain (0, 5, 6.0f);
        eq.setBandGain (1, 5, -6.0f);
        eq.setBandGain (2, 5, 3.0f);
        eq.setBandGain (3, 5, -3.0f);

        juce::AudioBuffer<float> buffer (4, 512);

        // Add same signal to all channels
        for (int ch = 0; ch < 4; ++ch)
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                buffer.setSample (ch, i, 0.5f * std::sin (i * 0.1f));

        eq.process (buffer);

        // All channels should have signal but different levels
        for (int ch = 0; ch < 4; ++ch)
        {
            bool hasSignal = false;
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                if (std::abs (buffer.getSample (ch, i)) > 0.01f)
                {
                    hasSignal = true;
                    break;
                }
            }
            expect (hasSignal, "Channel " + juce::String (ch) + " should have signal");
        }
    }

    void testReset()
    {
        beginTest ("Reset");

        gin::GraphicEQ eq (gin::GraphicEQ::Bands10);
        eq.setNumChannels (1);
        eq.setSampleRate (44100.0);

        eq.setBandGain (0, 5, 6.0f);

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

        expectWithinAbsoluteError (maxValue, 0.0f, 0.001f, "Should be silent after reset");
    }
};

static GraphicEQTests graphicEQTests;

#endif
