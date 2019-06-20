/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

CommentMetadata::CommentMetadata() : ImageMetadata ("Comment")
{
}

CommentMetadata* CommentMetadata::create(const uint8* data, int sz)
{
    MemoryInputStream is (data, (size_t) sz, false);

    CommentMetadata* md = new CommentMetadata();

    md->comment = is.readEntireStreamAsString();

    return md;
}

CommentMetadata::~CommentMetadata()
{
}

StringPairArray CommentMetadata::getAllMetadata() const
{
    StringPairArray s;
    s.set ("Comment", comment);
    return s;
}
