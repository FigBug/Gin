/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class AudioMetadataTests : public juce::UnitTest
{
public:
    AudioMetadataTests() : juce::UnitTest ("Audio Metadata", "gin_dsp") {}

    void runTest() override
    {
        testRootFromFilename();
        testBpmFromFilename();
        testWavAcidMetadata();
        testWavSamplerMetadata();
        testEmptyFile();

        // Cleanup temp files
        for (auto& f : tempFiles)
            f.deleteFile();
        tempFiles.clear();
    }

private:
    juce::File createMinimalWav (const juce::String& filename, int numSamples = 44100,
                                 double sampleRate = 44100.0,
                                 const std::unordered_map<juce::String, juce::String>& metadata = {})
    {
        auto tempDir = juce::File::getSpecialLocation (juce::File::tempDirectory);
        auto file = tempDir.getChildFile (filename);
        file.deleteFile();

        juce::WavAudioFormat wavFormat;
        auto options = juce::AudioFormatWriterOptions()
                           .withSampleRate (sampleRate)
                           .withNumChannels (1)
                           .withBitsPerSample (16)
                           .withMetadataValues (metadata);

        std::unique_ptr<juce::OutputStream> stream = std::make_unique<juce::FileOutputStream> (file);
        auto writer = wavFormat.createWriterFor (stream, options);

        if (writer)
        {
            juce::AudioBuffer<float> buffer (1, numSamples);
            buffer.clear();
            writer->writeFromAudioSampleBuffer (buffer, 0, numSamples);
        }

        tempFiles.add (file);
        return file;
    }

    juce::Array<juce::File> tempFiles;

    void testRootFromFilename()
    {
        beginTest ("Root Note From Filename");

        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();

        // Test various note name formats
        struct TestCase { const char* filename; int expectedRoot; };
        TestCase testCases[] = {
            { "bass_C.wav", 12 },
            { "bass_C#.wav", 13 },
            { "bass_Db.wav", 13 },
            { "lead_D.wav", 14 },
            { "lead_D#.wav", 15 },
            { "lead_Eb.wav", 15 },
            { "pad_E.wav", 16 },
            { "pad_F.wav", 17 },
            { "pad_F#.wav", 18 },
            { "pad_Gb.wav", 18 },
            { "synth_G.wav", 19 },
            { "synth_G#.wav", 20 },
            { "synth_Ab.wav", 20 },
            { "synth_A.wav", 21 },
            { "synth_A#.wav", 22 },
            { "synth_Bb.wav", 22 },
            { "synth_B.wav", 23 },
            { "loop_Cm.wav", 12 },        // Minor suffix
            { "loop_Cmin.wav", 12 },      // Min suffix
            { "loop_Cmaj.wav", 12 },      // Maj suffix
            { "bass[C#].wav", 13 },       // Brackets
            { "bass-C#-loop.wav", 13 },   // Dashes
            { "bass C#.wav", 13 },        // Spaces
        };

        for (auto& tc : testCases)
        {
            auto file = createMinimalWav (tc.filename, 4410);
            AudioMetadata metadata (file, formatManager);

            expect (metadata.root.has_value(),
                    juce::String ("Root should be detected for: ") + tc.filename);

            if (metadata.root.has_value())
            {
                // Check pitch class matches (mod 12) since octave may vary
                expectEquals (*metadata.root % 12, tc.expectedRoot % 12,
                              juce::String ("Root pitch class should match for: ") + tc.filename);
            }
        }

        // Test that non-note filenames don't produce a root
        auto file = createMinimalWav ("loop_no_note.wav", 4410);
        AudioMetadata metadata (file, formatManager);
        expect (! metadata.root.has_value(), "Non-note filename should not produce root");
    }

    void testBpmFromFilename()
    {
        beginTest ("BPM From Filename");

        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();

        // Create files with exact beat count to pass tempo validation
        // At 120 bpm, 1 second = 2 beats, so we need sample counts that divide evenly
        // 44100 samples at 44100 Hz = 1 second = 2 beats at 120 bpm

        struct TestCase { const char* filename; float expectedBpm; int numSamples; };
        TestCase testCases[] = {
            { "loop_120bpm.wav", 120.0f, 88200 },     // 2 seconds = 4 beats at 120 bpm
            { "loop_120_bpm.wav", 120.0f, 88200 },
            { "loop_120BPM.wav", 120.0f, 88200 },
            { "drum_090bpm.wav", 90.0f, 117600 },     // 2.67 sec = 4 beats at 90 bpm
            { "bass-140bpm-loop.wav", 140.0f, 75600 }, // ~1.71 sec = 4 beats at 140 bpm
        };

        for (auto& tc : testCases)
        {
            auto file = createMinimalWav (tc.filename, tc.numSamples);
            AudioMetadata metadata (file, formatManager);

            expect (metadata.bpm.has_value(),
                    juce::String ("BPM should be detected for: ") + tc.filename);

            if (metadata.bpm.has_value())
            {
                expectWithinAbsoluteError (*metadata.bpm, tc.expectedBpm, 0.1f,
                                           juce::String ("BPM should match for: ") + tc.filename);
            }
        }
    }

    void testWavAcidMetadata()
    {
        beginTest ("WAV ACID Chunk Metadata");

        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();

        // Create WAV with ACID metadata
        std::unordered_map<juce::String, juce::String> metadata;
        metadata[juce::WavAudioFormat::acidRootSet] = "1";
        metadata[juce::WavAudioFormat::acidRootNote] = "60";  // Middle C
        metadata[juce::WavAudioFormat::acidBeats] = "4";
        metadata[juce::WavAudioFormat::acidNumerator] = "4";
        metadata[juce::WavAudioFormat::acidDenominator] = "4";

        // 4 beats at 120 bpm = 2 seconds = 88200 samples
        auto file = createMinimalWav ("test_acid.wav", 88200, 44100.0, metadata);
        AudioMetadata result (file, formatManager);

        expect (result.root.has_value(), "Root should be read from ACID chunk");
        if (result.root.has_value())
            expectEquals (*result.root, 60, "Root should be 60 (middle C)");

        expect (result.bpm.has_value(), "BPM should be calculated from ACID beats");
        if (result.bpm.has_value())
            expectWithinAbsoluteError (*result.bpm, 120.0f, 0.1f, "BPM should be ~120");

        expect (result.numerator.has_value(), "Numerator should be read from ACID chunk");
        if (result.numerator.has_value())
            expectEquals (*result.numerator, 4, "Numerator should be 4");

        expect (result.denominator.has_value(), "Denominator should be read from ACID chunk");
        if (result.denominator.has_value())
            expectEquals (*result.denominator, 4, "Denominator should be 4");
    }

    void testWavSamplerMetadata()
    {
        beginTest ("WAV Sampler Chunk Metadata");

        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();

        // Create WAV with sampler metadata
        std::unordered_map<juce::String, juce::String> metadata;
        metadata["MidiUnityNote"] = "72";  // C5
        metadata["NumSampleLoops"] = "1";
        metadata["Loop0Start"] = "1000";
        metadata["Loop0End"] = "9999";

        auto file = createMinimalWav ("test_sampler.wav", 44100, 44100.0, metadata);
        AudioMetadata result (file, formatManager);

        expect (result.root.has_value(), "Root should be read from MidiUnityNote");
        if (result.root.has_value())
            expectEquals (*result.root, 72, "Root should be 72 (C5)");

        expect (result.loopStart.has_value(), "Loop start should be read from sampler chunk");
        if (result.loopStart.has_value())
            expectEquals ((int)*result.loopStart, 1000, "Loop start should be 1000");

        expect (result.loopEnd.has_value(), "Loop end should be read from sampler chunk");
        if (result.loopEnd.has_value())
            expectEquals ((int)*result.loopEnd, 10000, "Loop end should be 10000 (Loop0End + 1)");
    }

    void testEmptyFile()
    {
        beginTest ("Empty/Invalid File");

        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();

        // Test with non-existent file
        juce::File nonExistent (juce::File::getSpecialLocation (juce::File::tempDirectory)
                                    .getChildFile ("nonexistent_audio_file_12345.wav"));

        AudioMetadata result (nonExistent, formatManager);

        expect (! result.root.has_value(), "Root should be empty for non-existent file");
        expect (! result.bpm.has_value(), "BPM should be empty for non-existent file");
        expect (! result.numerator.has_value(), "Numerator should be empty for non-existent file");
        expect (! result.denominator.has_value(), "Denominator should be empty for non-existent file");
        expect (! result.loopStart.has_value(), "Loop start should be empty for non-existent file");
        expect (! result.loopEnd.has_value(), "Loop end should be empty for non-existent file");
    }
};

static AudioMetadataTests audioMetadataTests;

#endif
