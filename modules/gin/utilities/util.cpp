
Colour goldenRatioColor (int idx)
{
    double h = std::fmod (idx * 0.618033988749895, 1);
    return Colour (float (h), 0.8f, 0.95f, 1.0f);
}

String getMidiMessageType (const MidiMessage& msg)
{
    if (msg.isNoteOn()) return "Note On";
    if (msg.isNoteOff()) return "Note Off";
    if (msg.isSysEx()) return "SysEx";
    if (msg.isProgramChange()) return "Program Change";
    if (msg.isPitchWheel()) return "Pitch Wheel";
    if (msg.isAftertouch()) return "Aftertouch";
    if (msg.isChannelPressure()) return "Channel Pressure";
    if (msg.isController()) return "Controller";
    if (msg.isAllNotesOff()) return "All Notes Off";
    if (msg.isAllSoundOff()) return "All Sound Off";
    if (msg.isMetaEvent()) return "Meta";
    if (msg.isTrackMetaEvent()) return "Track Meta";
    if (msg.isEndOfTrackMetaEvent()) return "End of Track Meta";
    if (msg.isTrackNameEvent()) return "Track Name";
    if (msg.isTextMetaEvent()) return "Text Meta";
    if (msg.isTempoMetaEvent()) return "Tempo Meta";
    if (msg.isTimeSignatureMetaEvent()) return "Time Sig";
    if (msg.isKeySignatureMetaEvent()) return "Key Sig";
    if (msg.isMidiChannelMetaEvent()) return "Midi Channel";
    if (msg.isActiveSense()) return "Active Sense";
    if (msg.isMidiStart()) return "Midi Start";
    if (msg.isMidiStop()) return "Midi Stop";
    if (msg.isMidiClock()) return "Midi Clock";
    if (msg.isSongPositionPointer()) return "Song Pos";
    if (msg.isQuarterFrame()) return "Quarter Frame";
    if (msg.isFullFrame()) return "Full Frame";
    if (msg.isMidiMachineControlMessage()) return "MMC";
    
    return "Unknown";
}

//==============================================================================
class BackgroundCaller : private Thread,
                         private AsyncUpdater
{
public:
    BackgroundCaller (std::function<void (void)> func)
      : Thread ("BackgroundCaller"), function (func)
    {
        startThread();
    }
    
    ~BackgroundCaller()
    {
        stopThread (1000);
    }
    
    void run() override
    {
        function();
        triggerAsyncUpdate();
    }
    
    void handleAsyncUpdate() override
    {
        delete this;
    }
    
    std::function<void (void)> function;
};

void callInBackground (std::function<void (void)> function)
{
    new BackgroundCaller (function);
}


