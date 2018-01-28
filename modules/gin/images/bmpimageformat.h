/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
 Support for reading and writing Windows Bitmap files. Supports uncompressed
 8, 24 and 32 bit images. Always writes 32 bit images. That should be enough
 to cover 99.9% of BMP files. Does not support 1, 4, 16 bit colour images or
 images with RLE compression.
 */
class BMPImageFormat : public ImageFileFormat
{
public:
    String getFormatName() override;

    bool canUnderstand (InputStream& input) override;

    bool usesFileExtension (const File& possibleFile) override;

    Image decodeImage (InputStream& input) override;

    bool writeImageToStream (const Image& sourceImage, OutputStream& destStream) override;
};
