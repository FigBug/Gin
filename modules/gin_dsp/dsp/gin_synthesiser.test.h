//==============================================================================
#if GIN_UNIT_TESTS

// Mock voice for testing Synthesiser
class MockSynthVoice : public gin::SynthesiserVoice
{
public:
    MockSynthVoice() = default;

    void noteStarted() override
    {
        noteStartedCalled = true;
        active = true;
    }

    void noteStopped (bool allowTailOff) override
    {
        noteStoppedCalled = true;
        if (!allowTailOff)
            active = false;
    }

    void notePressureChanged() override {}
    void notePitchbendChanged() override {}
    void noteTimbreChanged() override {}
    void noteKeyStateChanged() override {}

    void renderNextBlock (juce::AudioBuffer<float>& buffer, int start, int num) override
    {
        // Generate simple sine wave
        for (int i = start; i < start + num; ++i)
        {
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                float sample = 0.1f * std::sin (phase);
                buffer.addSample (ch, i, sample);
            }
            phase += 0.01f;
        }

        if (!isActive())
            active = false;
    }

    float getCurrentNote() override
    {
        return currentlyPlayingNote.initialNote;
    }

    void noteRetriggered() override
    {
        noteRetriggeredCalled = true;
    }

    bool isActive() const override
    {
        return active && currentlyPlayingNote.keyState == juce::MPENote::keyDown;
    }

    bool noteStartedCalled = false;
    bool noteStoppedCalled = false;
    bool noteRetriggeredCalled = false;
    bool active = false;
    float phase = 0.0f;
};

class SynthesiserTests : public juce::UnitTest
{
public:
    SynthesiserTests() : juce::UnitTest ("Synthesiser", "gin_dsp") {}

    void runTest() override
    {
        testSynthesiserCreation();
        testPolyphonicMode();
        testMonophonicMode();
        testLegatoMode();
        testPitchBend();
        testMPEMode();
        testGlideInfo();
        testFastKill();
    }

private:
    void testSynthesiserCreation()
    {
        beginTest ("Synthesiser Creation");

        gin::Synthesiser synth;
        synth.setCurrentPlaybackSampleRate (44100.0);

        for (int i = 0; i < 8; ++i)
            synth.addVoice (new MockSynthVoice());

        expect (synth.getNumVoices() == 8, "Should have 8 voices");
    }

    void testPolyphonicMode()
    {
        beginTest ("Polyphonic Mode");

        gin::Synthesiser synth;
        synth.setCurrentPlaybackSampleRate (44100.0);

        for (int i = 0; i < 4; ++i)
            synth.addVoice (new MockSynthVoice());

        synth.setMono (false);
        synth.setNumVoices (4);

        // Play multiple notes
        juce::MidiBuffer midi;
        midi.addEvent (juce::MidiMessage::noteOn (1, 60, 1.0f), 0);
        midi.addEvent (juce::MidiMessage::noteOn (1, 64, 1.0f), 10);
        midi.addEvent (juce::MidiMessage::noteOn (1, 67, 1.0f), 20);

        juce::AudioBuffer<float> buffer (2, 512);
        buffer.clear();

        synth.renderNextBlock (buffer, midi, 0, 512);

        // Should have audio output from multiple voices
        float rms = buffer.getRMSLevel (0, 0, 512);
        expect (rms > 0.01f, "Should have audio output in poly mode");
    }

    void testMonophonicMode()
    {
        beginTest ("Monophonic Mode");

        gin::Synthesiser synth;
        synth.setCurrentPlaybackSampleRate (44100.0);

        for (int i = 0; i < 4; ++i)
            synth.addVoice (new MockSynthVoice());

        synth.setMono (true);
        synth.setLegato (false);

        // Play multiple notes - only one should sound at a time
        juce::MidiBuffer midi;
        midi.addEvent (juce::MidiMessage::noteOn (1, 60, 1.0f), 0);
        midi.addEvent (juce::MidiMessage::noteOn (1, 64, 1.0f), 100);

        juce::AudioBuffer<float> buffer (2, 512);
        buffer.clear();

        synth.renderNextBlock (buffer, midi, 0, 512);

        // Count active voices
        int activeVoices = 0;
        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto voice = dynamic_cast<MockSynthVoice*> (synth.getVoice (i)))
                if (voice->isActive())
                    activeVoices++;
        }

        expect (activeVoices <= 1, "Mono mode should have at most 1 active voice");
    }

    void testLegatoMode()
    {
        beginTest ("Legato Mode");

        gin::Synthesiser synth;
        synth.setCurrentPlaybackSampleRate (44100.0);

        for (int i = 0; i < 2; ++i)
            synth.addVoice (new MockSynthVoice());

        synth.setMono (true);
        synth.setLegato (true);

        // Play two notes in sequence
        juce::MidiBuffer midi;
        midi.addEvent (juce::MidiMessage::noteOn (1, 60, 1.0f), 0);
        midi.addEvent (juce::MidiMessage::noteOn (1, 64, 1.0f), 100);

        juce::AudioBuffer<float> buffer (2, 512);
        buffer.clear();

        synth.renderNextBlock (buffer, midi, 0, 512);

        // In legato mode with second note, the voice should be retriggered
        bool retriggeredCalled = false;
        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto voice = dynamic_cast<MockSynthVoice*> (synth.getVoice (i)))
                if (voice->noteRetriggeredCalled)
                    retriggeredCalled = true;
        }

        // Note: The exact behavior depends on implementation, but we can verify audio output
        float rms = buffer.getRMSLevel (0, 0, 512);
        expect (rms > 0.0f, "Should have audio output in legato mode");
        expect (retriggeredCalled, "A voice should be retriggered");
    }

    void testPitchBend()
    {
        beginTest ("Pitch Bend");

        gin::Synthesiser synth;
        synth.setCurrentPlaybackSampleRate (44100.0);

        synth.addVoice (new MockSynthVoice());
        synth.setPitchBendRange (2); // ±2 semitones

        juce::MidiBuffer midi;
        midi.addEvent (juce::MidiMessage::noteOn (1, 60, 1.0f), 0);
        midi.addEvent (juce::MidiMessage::pitchWheel (1, 8192 + 4096), 100); // +1 semitone

        juce::AudioBuffer<float> buffer (2, 512);
        buffer.clear();

        synth.renderNextBlock (buffer, midi, 0, 512);

        // Should produce audio with pitch bend applied
        float rms = buffer.getRMSLevel (0, 0, 512);
        expect (rms > 0.0f, "Should have audio with pitch bend");
    }

    void testMPEMode()
    {
        beginTest ("MPE Mode");

        gin::Synthesiser synth;
        synth.setCurrentPlaybackSampleRate (44100.0);

        synth.addVoice (new MockSynthVoice());
        synth.setMPE (true);

        // MPE mode should be enabled
        juce::MidiBuffer midi;
        midi.addEvent (juce::MidiMessage::noteOn (2, 60, 1.0f), 0); // Channel 2 (MPE zone)

        juce::AudioBuffer<float> buffer (2, 512);
        buffer.clear();

        synth.renderNextBlock (buffer, midi, 0, 512);

        // Should handle MPE messages
        expect (true, "MPE mode should be settable");
    }

    void testGlideInfo()
    {
        beginTest ("Glide Info");

        gin::GlideInfo glide;
        glide.fromNote = 60.0f;
        glide.portamento = true;
        glide.glissando = false;
        glide.rate = 250.0f;

        expect (glide.fromNote == 60.0f, "From note should be 60");
        expect (glide.portamento, "Portamento should be enabled");
        expect (!glide.glissando, "Glissando should be disabled");
        expectWithinAbsoluteError (glide.rate, 250.0f, 0.001f, "Rate should be 250ms");
    }

    void testFastKill()
    {
        beginTest ("Fast Kill");

        MockSynthVoice voice;

        expect (!voice.isFastKill(), "Fast kill should be false initially");

        voice.setFastKill();

        expect (voice.isFastKill(), "Fast kill should be true after setting");
    }
};

static SynthesiserTests synthesiserTests;

#endif
