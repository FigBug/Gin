#pragma once

//==============================================================================
/**
    Extracts musical metadata from audio files.

    AudioMetadata reads loop, tempo, time signature, and root note information
    from WAV and AIFF files. It supports:

    - **WAV files**: ACID chunk (root note, BPM, time signature) and
      sampler chunk (loop points, root note via MidiUnityNote)
    - **AIFF files**: Apple Loops basc chunk (root note, BPM, time signature)
      and INST chunk loop points via cue markers

    For files without embedded metadata, the class attempts to deduce:
    - Root note from the filename (e.g., "bass_C#.wav" -> C#)
    - BPM from the filename (e.g., "loop_120bpm.wav" -> 120) or cue labels

    All metadata fields are optional and will only be set if the corresponding
    data is found in the file or can be deduced.

    Usage:
    @code
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    AudioMetadata metadata (myAudioFile, formatManager);

    if (metadata.bpm.has_value())
        DBG ("BPM: " << *metadata.bpm);

    if (metadata.loopStart.has_value() && metadata.loopEnd.has_value())
        DBG ("Loop: " << *metadata.loopStart << " to " << *metadata.loopEnd);
    @endcode

    @see juce::AudioFormatReader, juce::WavAudioFormat, juce::AiffAudioFormat
*/
class AudioMetadata
{
public:
    /** Creates an AudioMetadata object by reading metadata from an audio file.

        @param f    The audio file to read metadata from
        @param mgr  An AudioFormatManager with registered formats (call registerBasicFormats() first)
    */
    AudioMetadata (const juce::File& f, juce::AudioFormatManager& mgr);

    /** The root note (MIDI note number 0-127).
        Read from ACID chunk (WAV), sampler chunk MidiUnityNote (WAV),
        Apple Loops basc chunk (AIFF), or deduced from filename.
    */
    std::optional<int> root;

    /** The tempo in beats per minute.
        Calculated from ACID chunk beat count (WAV) or Apple Loops
        beat count (AIFF), or deduced from filename/cue labels.
    */
    std::optional<float> bpm;

    /** Time signature numerator (beats per bar).
        For example, 4 in a 4/4 time signature.
        Read from ACID chunk (WAV) or Apple Loops basc chunk (AIFF).
    */
    std::optional<int> numerator;

    /** Time signature denominator (beat unit).
        For example, 4 in a 4/4 time signature.
        Read from ACID chunk (WAV) or Apple Loops basc chunk (AIFF).
    */
    std::optional<int> denominator;

    /** Loop start point in samples.
        Read from WAV sampler chunk (Loop0Start) or AIFF INST chunk
        via cue point markers.
    */
    std::optional<int64_t> loopStart;

    /** Loop end point in samples (exclusive).
        Read from WAV sampler chunk (Loop0End + 1) or AIFF INST chunk
        via cue point markers. Only set if within file length.
    */
    std::optional<int64_t> loopEnd;
};
