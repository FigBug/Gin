//==============================================================================
#if GIN_UNIT_TESTS

class FilterTests : public juce::UnitTest
{
public:
    FilterTests() : juce::UnitTest ("Filter", "gin_dsp") {}

    void runTest() override
    {
        testFilterCreation();
        testFilterTypes();
        testFilterSlopes();
        testFrequencyResponse();
        testLowpassFilter();
        testHighpassFilter();
        testBandpassFilter();
        testNotchFilter();
        testShelvingFilters();
        testPeakFilter();
        testReset();
    }

private:
    void testFilterCreation()
    {
        beginTest ("Filter Creation");

        gin::Filter filter;
        filter.setSampleRate (44100.0);
        filter.setNumChannels (2);

        expect (true, "Filter should be created successfully");
    }

    void testFilterTypes()
    {
        beginTest ("Filter Types");

        gin::Filter filter;
        filter.setSampleRate (44100.0);
        filter.setNumChannels (1);

        // Test that all filter types can be set
        filter.setType (gin::Filter::none);
        filter.setType (gin::Filter::lowpass);
        filter.setType (gin::Filter::highpass);
        filter.setType (gin::Filter::bandpass);
        filter.setType (gin::Filter::notch);
        filter.setType (gin::Filter::lowshelf);
        filter.setType (gin::Filter::highshelf);
        filter.setType (gin::Filter::peak);
        filter.setType (gin::Filter::allpass);

        expect (true, "All filter types should be settable");
    }

    void testFilterSlopes()
    {
        beginTest ("Filter Slopes");

        gin::Filter filter;
        filter.setSampleRate (44100.0);
        filter.setNumChannels (1);
        filter.setType (gin::Filter::lowpass);
        filter.setParams (1000.0f, 0.707f);

        // Test 12dB/oct slope
        filter.setSlope (gin::Filter::db12);
        float response12dB_high = filter.getResponseMagnitude (10000.0f);

        // Test 24dB/oct slope
        filter.setSlope (gin::Filter::db24);
        float response24dB_high = filter.getResponseMagnitude (10000.0f);

        // 24dB/oct should attenuate more than 12dB/oct at high frequencies
        expect (response24dB_high < response12dB_high, "24dB slope should attenuate more");
    }

    void testFrequencyResponse()
    {
        beginTest ("Frequency Response");

        gin::Filter filter;
        filter.setSampleRate (44100.0);
        filter.setNumChannels (1);
        filter.setType (gin::Filter::lowpass);
        filter.setParams (1000.0f, 0.707f);

        // Lowpass should pass low frequencies and attenuate high frequencies
        float responseLow = filter.getResponseMagnitude (100.0f);
        float responseHigh = filter.getResponseMagnitude (10000.0f);

        expect (responseLow > responseHigh, "Lowpass should attenuate high frequencies more");
        expect (responseLow > 0.5f, "Low frequencies should be mostly passed");
        expect (responseHigh < 0.5f, "High frequencies should be attenuated");
    }

    void testLowpassFilter()
    {
        beginTest ("Lowpass Filter");

        gin::Filter filter;
        filter.setSampleRate (44100.0);
        filter.setNumChannels (1);
        filter.setType (gin::Filter::lowpass);
        filter.setParams (1000.0f, 0.707f);

        // Create test buffer with impulse
        juce::AudioBuffer<float> buffer (1, 512);
        buffer.clear();
        buffer.setSample (0, 0, 1.0f); // Impulse

        filter.process (buffer);

        // After filtering impulse, signal should have non-zero values (impulse response)
        bool hasNonZeroValues = false;
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            if (std::abs (buffer.getSample (0, i)) > 0.001f)
            {
                hasNonZeroValues = true;
                break;
            }
        }

        expect (hasNonZeroValues, "Filter should produce impulse response");
    }

    void testHighpassFilter()
    {
        beginTest ("Highpass Filter");

        gin::Filter filter;
        filter.setSampleRate (44100.0);
        filter.setNumChannels (1);
        filter.setType (gin::Filter::highpass);
        filter.setParams (1000.0f, 0.707f);

        // Highpass should attenuate low frequencies and pass high frequencies
        float responseLow = filter.getResponseMagnitude (100.0f);
        float responseHigh = filter.getResponseMagnitude (10000.0f);

        expect (responseHigh > responseLow, "Highpass should attenuate low frequencies more");
        expect (responseLow < 0.5f, "Low frequencies should be attenuated");
        expect (responseHigh > 0.5f, "High frequencies should be mostly passed");
    }

    void testBandpassFilter()
    {
        beginTest ("Bandpass Filter");

        gin::Filter filter;
        filter.setSampleRate (44100.0);
        filter.setNumChannels (1);
        filter.setType (gin::Filter::bandpass);
        filter.setParams (1000.0f, 2.0f);

        // Bandpass should attenuate both low and high frequencies
        float responseLow = filter.getResponseMagnitude (100.0f);
        float responseCenter = filter.getResponseMagnitude (1000.0f);
        float responseHigh = filter.getResponseMagnitude (10000.0f);

        expect (responseCenter > responseLow, "Center should be louder than low");
        expect (responseCenter > responseHigh, "Center should be louder than high");
    }

    void testNotchFilter()
    {
        beginTest ("Notch Filter");

        gin::Filter filter;
        filter.setSampleRate (44100.0);
        filter.setNumChannels (1);
        filter.setType (gin::Filter::notch);
        filter.setParams (1000.0f, 2.0f);

        // Notch should attenuate center frequency
        float responseLow = filter.getResponseMagnitude (100.0f);
        float responseCenter = filter.getResponseMagnitude (1000.0f);
        float responseHigh = filter.getResponseMagnitude (10000.0f);

        expect (responseCenter < responseLow, "Center should be quieter than low");
        expect (responseCenter < responseHigh, "Center should be quieter than high");
    }

    void testShelvingFilters()
    {
        beginTest ("Shelving Filters");

        gin::Filter lowShelf, highShelf;

        lowShelf.setSampleRate (44100.0);
        lowShelf.setNumChannels (1);
        lowShelf.setType (gin::Filter::lowshelf);
        lowShelf.setParams (500.0f, 0.707f, 2.0f); // +6dB boost (2.0x gain)

        highShelf.setSampleRate (44100.0);
        highShelf.setNumChannels (1);
        highShelf.setType (gin::Filter::highshelf);
        highShelf.setParams (5000.0f, 0.707f, 2.0f); // +6dB boost (2.0x gain)

        // Low shelf should boost low frequencies
        float lowShelfLow = lowShelf.getResponseMagnitude (100.0f);
        float lowShelfHigh = lowShelf.getResponseMagnitude (10000.0f);
        expect (lowShelfLow > lowShelfHigh, "Low shelf should boost low more than high");

        // High shelf should boost high frequencies
        float highShelfLow = highShelf.getResponseMagnitude (100.0f);
        float highShelfHigh = highShelf.getResponseMagnitude (10000.0f);
        expect (highShelfHigh > highShelfLow, "High shelf should boost high more than low");
    }

    void testPeakFilter()
    {
        beginTest ("Peak Filter");

        gin::Filter filter;
        filter.setSampleRate (44100.0);
        filter.setNumChannels (1);
        filter.setType (gin::Filter::peak);
        filter.setParams (1000.0f, 2.0f, 2.0f); // Peak at 1kHz with gain

        float responseLow = filter.getResponseMagnitude (100.0f);
        float responseCenter = filter.getResponseMagnitude (1000.0f);
        float responseHigh = filter.getResponseMagnitude (10000.0f);

        // Peak should boost center frequency
        expect (responseCenter > responseLow, "Peak should boost center more than low");
        expect (responseCenter > responseHigh, "Peak should boost center more than high");
        expect (responseCenter > 1.0f, "Peak with gain should boost above unity");
    }

    void testReset()
    {
        beginTest ("Filter Reset");

        gin::Filter filter;
        filter.setSampleRate (44100.0);
        filter.setNumChannels (1);
        filter.setType (gin::Filter::lowpass);
        filter.setParams (1000.0f, 0.707f);

        // Process some audio to build up filter state
        juce::AudioBuffer<float> buffer (1, 512);
        buffer.clear();
        for (int i = 0; i < 100; ++i)
            buffer.setSample (0, i, 1.0f);

        filter.process (buffer);

        // Reset filter
        filter.reset();

        // Process silence - should get silence out (no residual state)
        buffer.clear();
        filter.process (buffer);

        float maxValue = 0.0f;
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            maxValue = std::max (maxValue, std::abs (buffer.getSample (0, i)));

        expectWithinAbsoluteError (maxValue, 0.0f, 0.001f, "Filter should be silent after reset");
    }
};

static FilterTests filterTests;

#endif
