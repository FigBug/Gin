/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

class IptcMetadata : public ImageMetadata
{
public:
    IptcMetadata();
    virtual ~IptcMetadata();

    static IptcMetadata* create(const juce::uint8* data, int sz);

    juce::StringPairArray getAllMetadata() const;

private:
    struct MetadataItem
    {
        int cat;
        int type;
        juce::String data;

        juce::String getName() const;
        juce::String getValue() const;
    };
    juce::OwnedArray<MetadataItem> items;
};
