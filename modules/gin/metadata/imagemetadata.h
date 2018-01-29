/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

#pragma once

class ImageMetadata
{
public:
	ImageMetadata (const String& type);

	virtual ~ImageMetadata();

	String getType() const { return type; }

	virtual StringPairArray getAllMetadata() const = 0;

protected:
	String type;
};

