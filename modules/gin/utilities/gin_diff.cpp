class Diff::Impl
{
public:
    diff_match_patch<std::string> dmp;
};

Diff::Diff()
{
    impl = std::make_unique<Impl>();
    impl->dmp.Diff_Timeout = 0;
}

Diff::~Diff()
{
}

juce::String Diff::diff (const juce::String& s1, const juce::String& s2)
{
    auto ss1 = s1.toStdString();
    auto ss2 = s2.toStdString();
    
    auto patches = impl->dmp.patch_make (ss1, ss2);
    return impl->dmp.patch_toText (patches);
}

juce::String Diff::applyPatch (const juce::String& s, const juce::String& patchText)
{
    auto patches = impl->dmp.patch_fromText (patchText.toStdString());
    auto [res, status] = impl->dmp.patch_apply (patches, s.toStdString());
    
    for (auto b : status)
    {
        jassert (b);
        if (! b)
            return {};
    }
    
    return res;
}
