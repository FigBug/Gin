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

	static IptcMetadata* create(const uint8* data, int sz);

	StringPairArray getAllMetadata() const;

private:
	struct MetadataItem
	{
		int cat;
		int type;
		String data;

		String getName() const;
		String getValue() const;
	};
	OwnedArray<MetadataItem> items;
};
