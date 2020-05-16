//==============================================================================
/** Info on how the voice should glide
*/
struct GlideInfo
{
    int fromNote = 0;
    bool glissando = false;
    bool portamento = false;
    float rate = 0.0f;
};

//==============================================================================
/** A voice with glide info and fast kill support
*/
class SynthesiserVoice : public MPESynthesiserVoice
{
public:
    void setFastKill()  { fastKill = true; }
    bool isFastKill()   { return fastKill; }

    virtual void noteRetriggered()  {}

    void setCurrentlyPlayingNote (MPENote note) { currentlyPlayingNote = note;  }
    void setGlideInfo (const GlideInfo& gi)     { glideInfo = gi;               }
protected:

    GlideInfo glideInfo;
    bool fastKill = false;
};

//==============================================================================
/** MPESynthesiser with better fast kill, mono and glide support
*/
class Synthesiser : public MPESynthesiser
{
public:
    Synthesiser()
    {
        instrument->enableLegacyMode();
        setPitchbendTrackingMode (MPEInstrument::allNotesOnChannel);
    }

    void setMono (bool m)           { mono = m;         }
    void setNumVoices (int v)       { numVoices = v;    }
    void setLegato (bool l)         { legato = l;       }
    void setGlissando (bool g)      { glissando = g;    }
    void setPortamento (bool p)     { portamento = p;   }
    void setGlideRate (float r )    { glideRate = r;    }

    void setMPE (bool newMPE)
    {
        if (newMPE != mpe)
        {
            mpe = newMPE;
            if (mpe)
            {
                MPEZoneLayout zones;
                zones.setLowerZone (15);
                instrument->setZoneLayout (zones);
                setPitchbendTrackingMode (MPEInstrument::lastNotePlayedOnChannel);
            }
            else
            {
                instrument->enableLegacyMode();
                setPitchbendTrackingMode (MPEInstrument::allNotesOnChannel);
            }
        }
    }

    void noteAdded (MPENote newNote) override
    {
        const ScopedLock sl (voicesLock);

        if (mono)
            return (glissando || portamento) ? noteAddedMonoGlide (newNote) : noteAddedMono (newNote);;

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

    void noteReleased (MPENote finishedNote) override
    {
        const ScopedLock sl (voicesLock);

        if (mono)
            return (glissando || portamento) ? noteReleasedMonoGlide (finishedNote) : noteReleasedMono (finishedNote);

        for (auto voice : voices)
            if (voice->isCurrentlyPlayingNote (finishedNote))
                stopVoice (voice, finishedNote, true);
    }

    void noteAddedMonoGlide (MPENote newNote)
    {
        noteStack.add (newNote);

        if (noteStack.size() == 1)
        {
            if (auto voice = voices[0])
                startVoice (voice, newNote);
        }
        else
        {
            if (auto sv = dynamic_cast<SynthesiserVoice*> (voices[0]))
            {
                if (sv->isActive())
                    retriggerVoice (sv, newNote);
                else
                    startVoice (sv, newNote);
            }
        }
    }

    void noteReleasedMonoGlide (MPENote finishedNote)
    {
        int noteIdx = noteStack.indexOf (finishedNote);
        jassert (noteIdx >= 0);
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

    void noteAddedMono (MPENote newNote)
    {
        for (auto voice : voices)
            if (voice->isActive())
                stopVoiceFastKill (voice, voice->getCurrentlyPlayingNote(), true);

        noteStack.add (newNote);

        if (auto voice = findFreeVoice (newNote, false))
            startVoice (voice, newNote);
    }

    void noteReleasedMono (MPENote finishedNote)
    {
        int noteIdx = noteStack.indexOf (finishedNote);
        jassert (noteIdx >= 0);
        bool currentNote = noteIdx == noteStack.size() - 1;
        noteStack.remove (noteIdx);

        if (noteStack.isEmpty())
        {
            for (auto voice : voices)
                if (voice->isCurrentlyPlayingNote (finishedNote))
                    stopVoice (voice, finishedNote, true);

            return;
        }

        if (currentNote)
            for (auto voice : voices)
                if (voice->isActive())
                    if (finishedNote == voice->getCurrentlyPlayingNote())
                        stopVoiceFastKill (voice, finishedNote, true);

        if (noteStack.size() > 0)
            if (auto voice = findFreeVoice (noteStack.getLast(), false))
                startVoice (voice, noteStack.getLast());
    }

    bool isNotePlaying (MPENote& n)
    {
        for (int i = 0; i < instrument->getNumPlayingNotes(); i++)
            if (n == instrument->getNote (i))
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

    void retriggerVoice (SynthesiserVoice* v, MPENote note)
    {
        updateGlide (v, note);
        v->setCurrentlyPlayingNote (note);
        v->noteRetriggered();
    }

    void startVoice (MPESynthesiserVoice* v, MPENote note)
    {
        updateGlide (v, note);
        MPESynthesiser::startVoice (v, note);
    }

    void updateGlide (MPESynthesiserVoice* v, MPENote note)
    {
        auto voice = dynamic_cast<SynthesiserVoice*> (v);

        if (glissando || portamento)
        {
            if (legato)
            {
                int num = instrument->getNumPlayingNotes();
                if (num > 1)
                {
                    GlideInfo gi;
                    gi.fromNote = instrument->getNote (num - 2).initialNote;
                    gi.glissando = glissando;
                    gi.portamento = portamento;
                    gi.rate = glideRate;

                    voice->setGlideInfo (gi);
                }
                else
                {
                    voice->setGlideInfo ({});
                }
            }
            else
            {
                int num = instrument->getNumPlayingNotes();
                if (num > 1)
                    lastNote = instrument->getNote (num - 2).initialNote;

                GlideInfo gi;
                gi.fromNote = lastNote;
                gi.glissando = glissando;
                gi.portamento = portamento;
                gi.rate = glideRate;

                voice->setGlideInfo (gi);

                lastNote = note.initialNote;
            }
        }
        else
        {
            voice->setGlideInfo ({});
        }
    }

    void stopVoiceFastKill (MPESynthesiserVoice* v, MPENote note, bool tailOff)
    {
        if (auto sv = dynamic_cast<SynthesiserVoice*> (v))
            sv->setFastKill();

        stopVoice (v, note, tailOff);
    }

    MPESynthesiserVoice* findVoiceToSteal (MPENote noteToStealVoiceFor = MPENote()) const override
    {
        // This voice-stealing algorithm applies the following heuristics:
        // - Re-use the oldest notes first
        // - Protect the lowest & topmost notes, even if sustained, but not if they've been released.

        // apparently you are trying to render audio without having any voices...
        jassert (voices.size() > 0);

        // These are the voices we want to protect (ie: only steal if unavoidable)
        MPESynthesiserVoice* low = nullptr; // Lowest sounding note, might be sustained, but NOT in release phase
        MPESynthesiserVoice* top = nullptr; // Highest sounding note, might be sustained, but NOT in release phase

        // this is a list of voices we can steal, sorted by how long they've been running
        Array<MPESynthesiserVoice*> usableVoices;
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
                bool operator() (const MPESynthesiserVoice* a, const MPESynthesiserVoice* b) const noexcept { return a->noteOnTime < b->noteOnTime; }
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
                 && voice->getCurrentlyPlayingNote().keyState != MPENote::keyDown
                 && voice->getCurrentlyPlayingNote().keyState != MPENote::keyDownAndSustained)
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
    void renderNextBlock (AudioBuffer<floatType>& outputAudio,
                          const MidiBuffer& inputMidi,
                          int startSample,
                          int numSamples)
    {
        slice.clear();
        slice.addEvents (inputMidi, startSample, numSamples, 0);

        MPESynthesiser::renderNextBlock (outputAudio, slice, startSample, numSamples);
    }

private:
    MidiBuffer slice;
    Array<MPENote> noteStack;
    bool mono = false, legato = false, glissando = false, portamento = false;
    float glideRate = 500.0f;
    int numVoices = 32;
    int lastNote = -1;
    bool mpe = false;
};
