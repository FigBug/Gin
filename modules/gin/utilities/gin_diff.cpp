namespace Diff
{

std::vector<uint8_t> bsDiff (const juce::String& s1, const juce::String& s2)
{
    std::vector<uint8_t> result;

    uint32_t sz = uint32_t (s2.getNumBytesAsUTF8());
    result.insert (result.end(), (uint8_t*)&sz, (uint8_t*)&sz + sizeof(uint32_t));

    bsdiff_stream stream;

    stream.opaque = &result;
    stream.malloc = malloc;
    stream.free   = free;
    stream.write  = [] (struct bsdiff_stream* param, const void* buffer, int size)
    {
        auto& buf = *((std::vector<uint8_t>*)param->opaque);

        buf.insert (buf.end(), (uint8_t*)buffer, (uint8_t*)buffer + size);
        return 0;
    };

    [[maybe_unused]] auto err = bsdiff ((const uint8_t*)s1.toRawUTF8(), int64_t (s1.getNumBytesAsUTF8()), (const uint8_t*)s2.toRawUTF8(), int64_t (s2.getNumBytesAsUTF8()), &stream);
    jassert (err == 0);

    return result;
}

juce::String bsApplyPatch (const juce::String& s, const std::vector<uint8_t>& patch)
{
    uint32_t sz;
    memcpy (&sz, patch.data(), sizeof (sz));

    juce::MemoryBlock mb (sz + 1, true);

    juce::MemoryInputStream input (patch.data() + sizeof (uint32_t), patch.size() - 4, false);

    bspatch_stream stream;
    stream.opaque = &input;
    stream.read   = [] (const struct bspatch_stream* param, void* buffer, int length)
    {
        auto& is = *(juce::MemoryInputStream*)param->opaque;

        if (is.read (buffer, length) == length)
            return 0;

        return -1;
    };

    [[maybe_unused]] auto err = bspatch ((const uint8_t*)s.toRawUTF8(), int64_t (s.getNumBytesAsUTF8()), (uint8_t*)mb.getData(), sz, &stream);
    jassert (err == 0);

    return juce::String::fromUTF8 ((const char*)mb.getData());
}

}
