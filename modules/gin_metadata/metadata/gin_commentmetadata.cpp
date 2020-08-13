/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

CommentMetadata::CommentMetadata() : ImageMetadata ("Comment")
{
}

CommentMetadata* CommentMetadata::create (const juce::uint8* data, int sz)
{
    juce::MemoryInputStream is (data, (size_t) sz, false);

    CommentMetadata* md = new CommentMetadata();

    md->comment = is.readEntireStreamAsString();

    return md;
}

CommentMetadata::~CommentMetadata()
{
}

juce::StringPairArray CommentMetadata::getAllMetadata() const
{
    juce::StringPairArray s;
    s.set ("Comment", comment);
    return s;
}
