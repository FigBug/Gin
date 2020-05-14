//==============================================================================
class SynthesiserVoice : public MPESynthesiserVoice
{
public:
    void setFastKill()  { fastKill = true; }
    bool isFastKill()   { return fastKill; }
    
protected:
    bool fastKill = false;
};

//==============================================================================
class Synthesiser : public MPESynthesiser
{
public:
    void setNumVoices (int v)
    {
        numVoices = v;
    }
    
    void noteAdded (MPENote newNote) override
    {
        const ScopedLock sl (voicesLock);
    
        if (auto voice = findFreeVoice (newNote, false))
        {
            int active = getNumActiveVoices();
            if (active >= numVoices)
            {
                if (auto v = dynamic_cast<SynthesiserVoice*> (findVoiceToSteal (newNote)))
                {
                    v->setFastKill();
					stopVoice (v, v->getCurrentlyPlayingNote(), true);
                    stompedNotes.add (v->getCurrentlyPlayingNote());
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

		for (int i = stompedNotes.size(); --i >= 0;)
			if (stompedNotes[i] == finishedNote)
				stompedNotes.remove (i);
    
        for (auto i = voices.size(); --i >= 0;)
        {
            auto voice = voices.getUnchecked (i);
    
            if (voice->isCurrentlyPlayingNote (finishedNote))
            {
                stopVoice (voice, finishedNote, true);

                auto v = dynamic_cast<SynthesiserVoice*> (voice);
                if (v != nullptr && ! v->isFastKill())
                {
                    if (stompedNotes.size() > 0)
                    {
						auto n = stompedNotes.getLast();
                        stompedNotes.removeLast();

						if (auto newVoice = findFreeVoice (n, false))
							startVoice (newVoice, n);
                    }
                }
            }
        }
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
    Array<MPENote> stompedNotes;
    int numVoices = 32;
};
