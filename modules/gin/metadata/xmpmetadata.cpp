/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

XmpMetadata::XmpMetadata() : ImageMetadata ("Xmp")
{
}

XmpMetadata* XmpMetadata::createFromJpg (const uint8* data, int sz)
{
	MemoryInputStream is (data, size_t (sz), false);

	char header[29];
	is.read (header, 29);
	if (memcmp ("http://ns.adobe.com/xap/1.0/", header, 29) != 0)
		return NULL;

	XmpMetadata* md = new XmpMetadata();

	md->xmp = is.readEntireStreamAsString();

	return md;
}

XmpMetadata* XmpMetadata::createFromPng (const char* data, int sz)
{
	MemoryInputStream is (data, (size_t) sz, false);
	XmpMetadata* md = new XmpMetadata();
	md->xmp = is.readEntireStreamAsString();
	return md;
}

XmpMetadata::~XmpMetadata()
{
}

StringPairArray XmpMetadata::getAllMetadata() const
{
	StringPairArray s;
	s.set ("XMP", xmp);
	return s;
}
