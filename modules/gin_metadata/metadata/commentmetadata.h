/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

class CommentMetadata : public ImageMetadata
{
public:
    CommentMetadata();
    virtual ~CommentMetadata();

    static CommentMetadata* create(const uint8* data, int sz);

    StringPairArray getAllMetadata() const;

private:
    String comment;
};
