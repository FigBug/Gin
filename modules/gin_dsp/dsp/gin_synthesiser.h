//==============================================================================
/**
    Configuration for pitch glide (portamento/glissando) between notes.

    GlideInfo defines how a synthesizer voice should transition in pitch from
    one note to another. This is used for implementing portamento (smooth glide)
    and glissando (stepped glide through semitones).

    Fields:
    - fromNote: The starting MIDI note number (-1 if no glide)
    - glissando: If true, glide in semitone steps; if false, smooth glide
    - portamento: If true, enable pitch glide
    - rate: Glide time in milliseconds

    @see SynthesiserVoice, Synthesiser
*/
struct GlideInfo
{
    float fromNote = -1;
    bool glissando = false;
    bool portamento = false;
    float rate = 0.0f;
};

//==============================================================================
/**
    Extended synthesizer voice with glide support and fast-kill capability.

    SynthesiserVoice extends JUCE's MPESynthesiserVoice with additional features
    needed for professional synthesizer implementations:
    - Pitch glide (portamento/glissando)
    - Fast kill for voice stealing (bypass release envelope)
    - Note retriggering support for monophonic/legato playing

    When implementing a voice, override:
    - getCurrentNote(): Return the current MIDI note being played
    - noteRetriggered(): Handle retriggering when in legato/mono mode
    - renderNextBlock(): Generate audio (inherited from MPESynthesiserVoice)

    Fast Kill:
    When a voice needs to be stolen while still in release phase, setFastKill()
    allows immediate termination without waiting for the release envelope to finish.

    Usage:
    @code
    class MyVoice : public SynthesiserVoice
    {
        void renderNextBlock(AudioBuffer<float>& buffer, int start, int num) override
        {
            if (isFastKill())
            {
                // Quick fade out instead of full release
            }
            // ... render audio
        }

        float getCurrentNote() override { return currentNote; }
        void noteRetriggered() override { // Handle legato retrigger }
    };
    @endcode

    @see Synthesiser, GlideInfo
*/
class SynthesiserVoice : public juce::MPESynthesiserVoice
{
public:
    virtual void setFastKill()  { fastKill = true; }
    bool isFastKill()           { return fastKill; }

    virtual void noteRetriggered()  {}
    virtual float getCurrentNote() = 0;

    void setCurrentlyPlayingNote (juce::MPENote note)   { currentlyPlayingNote = note;  }
    void setGlideInfo (const GlideInfo& gi)             { glideInfo = gi;               }

protected:

    GlideInfo glideInfo;
    bool fastKill = false;
};

//==============================================================================
/**
    Advanced MPE-capable synthesizer with mono/poly modes, glide, and voice management.

    Synthesiser extends JUCE's MPESynthesiser with professional features commonly
    needed in modern synthesizers:

    Key Features:
    - Monophonic and polyphonic modes
    - Legato mode (retriggering without envelope restart)
    - Portamento (smooth pitch glide) and glissando (stepped pitch glide)
    - Configurable voice count with intelligent voice stealing
    - Fast-kill voice stealing (immediate voice termination)
    - MPE (MIDI Polyphonic Expression) support
    - CPU usage monitoring
    - Pitchbend range control

    Mono Mode:
    In monophonic mode, only one voice plays at a time. New notes either retrigger
    the envelope (non-legato) or smoothly transition pitch without retriggering (legato).
    Released notes in the note stack will cause the synth to jump back to the most
    recent held note.

    Voice Stealing:
    When voice limit is reached, the oldest voice is stolen. The fast-kill mechanism
    allows immediate voice termination rather than waiting for release envelope,
    preventing audio glitches.

    Glide Modes:
    - Portamento: Smooth exponential pitch glide between notes
    - Glissando: Stepped pitch glide through semitones
    - Rate: Glide time in milliseconds

    Usage:
    @code
    Synthesiser synth;
    synth.addVoice(new MyVoice());
    synth.setSampleRate(44100.0);

    // Configure mono mode with portamento
    synth.setMono(true);
    synth.setLegato(true);
    synth.setPortamento(true);
    synth.setGlideRate(250.0f); // 250ms glide

    // Or poly mode
    synth.setMono(false);
    synth.setNumVoices(8); // Max 8 simultaneous voices

    // MPE mode
    synth.setMPE(true);
    synth.setPitchBendRange(48); // ±48 semitones

    synth.renderNextBlock(audioBuffer, midiBuffer, 0, numSamples);
    @endcode

    @see SynthesiserVoice, GlideInfo, juce::MPESynthesiser
*/
class Synthesiser : public juce::MPESynthesiser
{
public:
    Synthesiser()
    {
        instrument.enableLegacyMode();
        setPitchbendTrackingMode (juce::MPEInstrument::allNotesOnChannel);
    }
    
    void setMono (bool m)           { mono = m;         }
    void setNumVoices (int v)       { numVoices = v;    }
    void setLegato (bool l)         { legato = l;       }
    void setGlissando (bool g)      { glissando = g;    }
    void setPortamento (bool p)     { portamento = p;   }
    void setGlideRate (float r )    { glideRate = r;    }
    
    void turnOffAllVoices (bool allowTailOff) override
    {
        juce::MPESynthesiser::turnOffAllVoices (allowTailOff);
        
        noteStack.clearQuick();
        lastNote = -1;
    }

    void setMPE (bool newMPE)
    {
        if (newMPE != mpe)
        {
            mpe = newMPE;
            if (mpe)
            {
                juce::MPEZoneLayout zones;
                zones.setLowerZone (15);
                instrument.setZoneLayout (zones);
                setPitchbendTrackingMode (juce::MPEInstrument::lastNotePlayedOnChannel);
            }
            else
            {
                enableLegacyMode (pitchbend);
                setPitchbendTrackingMode (juce::MPEInstrument::allNotesOnChannel);
            }
        }
    }

    void handleMidiEvent (const juce::MidiMessage& message) override
    {
        juce::MPESynthesiser::handleMidiEvent (message);

        if (message.isAllNotesOff() || message.isAllSoundOff())
        {
            turnOffAllVoices (true);
        }
        else if (! mpe && message.isPitchWheel())
        {
            for (auto v : voices)
            {
                if (auto sv = dynamic_cast<SynthesiserVoice*> (v))
                {
                    if (sv->isPlayingButReleased())
                    {
                        auto note = sv->getCurrentlyPlayingNote();
                        note.totalPitchbendInSemitones = juce::MPEValue::from14BitInt (message.getPitchWheelValue()).asSignedFloat() * getLegacyModePitchbendRange();
                        sv->setCurrentlyPlayingNote (note);
                    }
                }
            }
        }
    }

    void setPitchBendRange (int newPB)
    {
        if (newPB != pitchbend)
        {
            pitchbend = newPB;
            if (! mpe)
                setLegacyModePitchbendRange (pitchbend);
        }
    }

    void noteAdded (juce::MPENote newNote) override
    {
        const juce::ScopedLock sl (voicesLock);

        if (mono)
            return noteAddedMono (newNote);

        if (auto voice = findFreeVoice (newNote, false))
        {
            int active = getNumActiveVoices();
            if (active >= numVoices)
            {
                if (auto v = dynamic_cast<SynthesiserVoice*> (findVoiceToSteal (newNote)))
                {
                    v->setFastKill();
                    stopVoice (v, v->getCurrentlyPlayingNote(), true);
                }
            }
            startVoice (voice, newNote);
        }
        else
        {
            // this should never happen, extra voices should handle this
            jassertfalse;
        }
    }

    void noteReleased (juce::MPENote finishedNote) override
    {
        const juce::ScopedLock sl (voicesLock);

        if (mono)
            (glissando || portamento) ? noteReleasedMonoGlide (finishedNote) : noteReleasedMono (finishedNote);

        for (auto voice : voices)
            if (voice->isCurrentlyPlayingNote (finishedNote))
                stopVoice (voice, finishedNote, true);
    }

    void noteAddedMono (juce::MPENote newNote)
    {
        noteStack.add (newNote);

        if (auto sv = dynamic_cast<SynthesiserVoice*> (voices[0]))
        {
            if (sv->isActive())
                retriggerVoice (sv, newNote);
            else
                startVoice (sv, newNote);
        }
    }

    void noteReleasedMonoGlide (juce::MPENote finishedNote)
    {
        int noteIdx = noteStack.indexOf (finishedNote);
        if (noteIdx < 0) return;
        bool currentNote = noteIdx == noteStack.size() - 1;
        noteStack.remove (noteIdx);

        auto sv = dynamic_cast<SynthesiserVoice*> (voices[0]);
        if (sv == nullptr) return;

        if (noteStack.isEmpty())
        {
            if (sv->isActive())
                stopVoice (sv, finishedNote, true);
        }
        else if (currentNote)
        {
            if (sv->isActive())
                retriggerVoice (sv, noteStack.getLast());
            else
                startVoice (sv, noteStack.getLast());
        }
    }

    void noteReleasedMono (juce::MPENote finishedNote)
    {
        int noteIdx = noteStack.indexOf (finishedNote);
        if (noteIdx < 0) return;
        bool currentNote = noteIdx == noteStack.size() - 1;
        noteStack.remove (noteIdx);

        auto sv = dynamic_cast<SynthesiserVoice*> (voices[0]);
        if (sv == nullptr) return;

        if (noteStack.isEmpty())
        {
            for (auto voice : voices)
                if (voice->isCurrentlyPlayingNote (finishedNote))
                    stopVoice (voice, finishedNote, true);

            return;
        }
        else if (currentNote)
        {
            if (sv->isActive())
                retriggerVoice (sv, noteStack.getLast());
            else
                startVoice (sv, noteStack.getLast());
        }
    }

    bool isNotePlaying (juce::MPENote& n)
    {
        for (int i = 0; i < instrument.getNumPlayingNotes(); i++)
            if (n == instrument.getNote (i))
                return true;

        return false;
    }

    int getNumActiveVoices()
    {
        int active = 0;
        for (auto v : voices)
            if (auto sv = dynamic_cast<SynthesiserVoice*> (v))
                if (sv->isActive() && ! sv->isFastKill())
                    active++;

        return active;
    }

    juce::Array<SynthesiserVoice*> getActiveVoices()
    {
        juce::Array<SynthesiserVoice*> res;

        for (auto v : voices)
            if (auto sv = dynamic_cast<SynthesiserVoice*> (v))
                if (sv->isActive() && ! sv->isFastKill())
                    res.add ( sv );

        return res;
    }

    int getCpuUsage()
    {
        int cpu = int (timeUsed / timeAvailable * 100);
        timeUsed = 0.0;
        timeAvailable = 0.0;
        return std::min (99, cpu);
    }

    void startBlock()
    {
        blockStartTime = juce::Time::getMillisecondCounterHiRes() / 1000.0;
    }

    void endBlock (int blockSize)
    {
        auto blockEndTime = juce::Time::getMillisecondCounterHiRes() / 1000.0;

        timeUsed        += (blockEndTime - blockStartTime);
        timeAvailable   += blockSize / getSampleRate();
    }

    void retriggerVoice (SynthesiserVoice* v, juce::MPENote note)
    {
        updateGlide (v);
        v->setCurrentlyPlayingNote (note);
        v->noteRetriggered();
    }

    virtual void startVoice (juce::MPESynthesiserVoice* v, juce::MPENote note)
    {
        updateGlide (v);
        MPESynthesiser::startVoice (v, note);
    }

    void updateGlide (juce::MPESynthesiserVoice* v)
    {
        auto voice = dynamic_cast<SynthesiserVoice*> (v);

        if (glissando || portamento)
        {
            if (lastNote < 0)
            {
                voice->setGlideInfo ({});
            }
            else if (legato)
            {
                int num = instrument.getNumPlayingNotes();
                if (num > 1)
                {
                    GlideInfo gi;
                    gi.fromNote     = lastNote;
                    gi.glissando    = glissando;
                    gi.portamento   = portamento;
                    gi.rate         = glideRate;

                    voice->setGlideInfo (gi);
                }
                else
                {
                    voice->setGlideInfo ({});
                }
            }
            else
            {
                GlideInfo gi;
                gi.fromNote     = lastNote;
                gi.glissando    = glissando;
                gi.portamento   = portamento;
                gi.rate         = glideRate;

                voice->setGlideInfo (gi);
            }
        }
        else
        {
            voice->setGlideInfo ({});
        }
    }

    void stopVoiceFastKill (juce::MPESynthesiserVoice* v, juce::MPENote note, bool tailOff)
    {
        if (auto sv = dynamic_cast<SynthesiserVoice*> (v))
            sv->setFastKill();

        stopVoice (v, note, tailOff);
    }

    juce::MPESynthesiserVoice* findVoiceToSteal (juce::MPENote noteToStealVoiceFor = juce::MPENote()) const override
    {
        // This voice-stealing algorithm applies the following heuristics:
        // - Re-use the oldest notes first
        // - Protect the lowest & topmost notes, even if sustained, but not if they've been released.

        // apparently you are trying to render audio without having any voices...
        jassert (voices.size() > 0);

        // These are the voices we want to protect (ie: only steal if unavoidable)
        juce::MPESynthesiserVoice* low = nullptr; // Lowest sounding note, might be sustained, but NOT in release phase
        juce::MPESynthesiserVoice* top = nullptr; // Highest sounding note, might be sustained, but NOT in release phase

        // this is a list of voices we can steal, sorted by how long they've been running
        juce::Array<juce::MPESynthesiserVoice*> usableVoices;
        usableVoices.ensureStorageAllocated (voices.size());

        for (auto voice : voices)
        {
             // We have extra voices for the fast kill. Don't steal one of those
            if (! voice->isActive()) continue;
            // Don't steal voices already fast killed
            if (auto v = (dynamic_cast<SynthesiserVoice*> (voice)))
                if (v->isFastKill()) continue;

            usableVoices.add (voice);

            // NB: Using a functor rather than a lambda here due to scare-stories about
            // compilers generating code containing heap allocations..
            struct Sorter
            {
                bool operator() (const juce::MPESynthesiserVoice* a, const juce::MPESynthesiserVoice* b) const noexcept { return a->noteOnTime < b->noteOnTime; }
            };

            std::sort (usableVoices.begin(), usableVoices.end(), Sorter());

            if (! voice->isPlayingButReleased()) // Don't protect released notes
            {
                auto noteNumber = voice->getCurrentlyPlayingNote().initialNote;

                if (low == nullptr || noteNumber < low->getCurrentlyPlayingNote().initialNote)
                    low = voice;

                if (top == nullptr || noteNumber > top->getCurrentlyPlayingNote().initialNote)
                    top = voice;
            }
        }

        // Eliminate pathological cases (ie: only 1 note playing): we always give precedence to the lowest note(s)
        if (top == low)
            top = nullptr;

        // If we want to re-use the voice to trigger a new note,
        // then The oldest note that's playing the same note number is ideal.
        if (noteToStealVoiceFor.isValid())
            for (auto voice : usableVoices)
                if (voice->getCurrentlyPlayingNote().initialNote == noteToStealVoiceFor.initialNote)
                    return voice;

        // Oldest voice that has been released (no finger on it and not held by sustain pedal)
        for (auto voice : usableVoices)
            if (voice != low && voice != top && voice->isPlayingButReleased())
                return voice;

        // Oldest voice that doesn't have a finger on it:
        for (auto voice : usableVoices)
            if (voice != low && voice != top
                 && voice->getCurrentlyPlayingNote().keyState != juce::MPENote::keyDown
                 && voice->getCurrentlyPlayingNote().keyState != juce::MPENote::keyDownAndSustained)
                return voice;

        // Oldest voice that isn't protected
        for (auto voice : usableVoices)
            if (voice != low && voice != top)
                return voice;

        // We've only got "protected" voices now: lowest note takes priority
        jassert (low != nullptr);

        // Duophonic synth: give priority to the bass note:
        if (top != nullptr)
            return top;

        return low;
    }

    template <typename floatType>
    void renderNextBlock (juce::AudioBuffer<floatType>& outputAudio,
                          const juce::MidiBuffer& inputMidi,
                          int startSample,
                          int numSamples)
    {
        noteOnIndex = -1;
        noteOffIndex = -1;

        slice.clear();
        slice.addEvents (inputMidi, startSample, numSamples, 0);

        int numNotesBefore = instrument.getNumPlayingNotes();

        MPESynthesiser::renderNextBlock (outputAudio, slice, startSample, numSamples);

        if (numNotesBefore == 0 && instrument.getNumPlayingNotes() > 0)
        {
            for (auto itr : inputMidi)
            {
                const auto& msg = itr.getMessage();
                int pos = itr.samplePosition;
                if (msg.isNoteOn() && pos >= startSample && pos < startSample + numSamples)
                {
                    noteOnIndex = pos - startSample;
                    break;
                }
            }
        }
        if (numNotesBefore > 0 && instrument.getNumPlayingNotes() == 0)
        {
            noteOffIndex = 0;

            for (auto itr : inputMidi)
            {
                const auto& msg = itr.getMessage();
                int pos = itr.samplePosition;

                if (msg.isNoteOff() && pos >= startSample && pos < startSample + numSamples)
                {
                    noteOnIndex = pos - startSample;
                }
            }
        }

        if (auto v = getNewestVoice())
            lastNote = v->getCurrentNote();
    }

    SynthesiserVoice* getOldestVoice()
    {
        juce::Array<juce::MPESynthesiserVoice*> activeVoices;

        for (auto voice : voices)
            if (voice->isActive())
                activeVoices.add (voice);

        std::sort (activeVoices.begin(), activeVoices.end(), [] (juce::MPESynthesiserVoice* a, juce::MPESynthesiserVoice* b) { return a->noteOnTime < b->noteOnTime; });

        return dynamic_cast<SynthesiserVoice*> (activeVoices.getFirst());
    }

    SynthesiserVoice* getNewestVoice()
    {
        juce::Array<juce::MPESynthesiserVoice*> activeVoices;

        for (auto voice : voices)
            if (voice->isActive())
                activeVoices.add (voice);

        std::sort (activeVoices.begin(), activeVoices.end(), [] (juce::MPESynthesiserVoice* a, juce::MPESynthesiserVoice* b) { return a->noteOnTime < b->noteOnTime; });

        return dynamic_cast<SynthesiserVoice*> (activeVoices.getLast());
    }

protected:
    int noteOnIndex = -1, noteOffIndex = -1;

private:
    juce::MidiBuffer slice;
    juce::Array<juce::MPENote> noteStack;
    bool mono = false, legato = false, glissando = false, portamento = false;
    float glideRate = 500.0f;
    int numVoices = 32;
    float lastNote = -1;
    int pitchbend = 2;
    bool mpe = false;
    double blockStartTime = 0, timeUsed = 0, timeAvailable = 0;
};
