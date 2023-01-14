/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wzero-as-null-pointer-constant")

extern "C"
{
#include "../3rdparty/webp/webp/decode.h"
#include "../3rdparty/webp/webp/encode.h"
}

JUCE_END_IGNORE_WARNINGS_GCC_LIKE

juce::String WEBPImageFormat::getFormatName()
{
    return "WebP";
}

bool WEBPImageFormat::canUnderstand (juce::InputStream& input)
{
    juce::MemoryBlock mb;
    input.readIntoMemoryBlock (mb, 12);

    if (mb.getSize() < 12)
        return false;

    auto riff = ::std::memcmp (mb.begin(), "RIFF", 4) == 0;
    auto webp = ::std::memcmp (mb.begin() + 8, "WEBP", 4) == 0;

    return riff && webp;
}

bool WEBPImageFormat::usesFileExtension (const juce::File& possibleFile)
{
    return possibleFile.hasFileExtension ("webp");
}

juce::Image WEBPImageFormat::decodeImage (juce::InputStream& input)
{
    juce::MemoryBlock mb;
    input.readIntoMemoryBlock (mb);

    int w = 0, h = 0;
    if (! WebPGetInfo ((uint8_t*)mb.begin(), mb.getSize(), &w, &h))
        return {};

    WebPBitstreamFeatures features;
    ::std::memset (&features, 0, sizeof (features));
    if (WebPGetFeatures ((uint8_t*)mb.begin(), mb.getSize(), &features) != VP8_STATUS_OK)
        return {};

    juce::Image img (features.has_alpha ? juce::Image::ARGB : juce::Image::RGB, w, h, true);

    juce::Image::BitmapData data (img, juce::Image::BitmapData::readWrite);

    if (data.pixelFormat == juce::Image::ARGB)
        WebPDecodeBGRAInto ((uint8_t*)mb.begin(), mb.getSize(), data.data, data.size, data.lineStride);
    else
        WebPDecodeBGRInto ((uint8_t*)mb.begin(), mb.getSize(), data.data, data.size, data.lineStride);

    return img;
}

bool WEBPImageFormat::writeImageToStream (const juce::Image& img, juce::OutputStream& dst)
{
    juce::Image::BitmapData data (img, juce::Image::BitmapData::readOnly);
    if (data.pixelFormat == juce::Image::ARGB)
    {
        if (loseless)
        {
            uint8_t* output = nullptr;
            auto size = WebPEncodeLosslessBGRA (data.data, img.getWidth(), img.getHeight(), data.lineStride, &output);
            if (size > 0 && output != nullptr)
            {
                dst.write (output, size);
                WebPFree (output);
                return true;
            }
        }
        else
        {
            uint8_t* output = nullptr;
            auto size = WebPEncodeBGRA (data.data, img.getWidth(), img.getHeight(), data.lineStride, quality, &output);
            if (size > 0 && output != nullptr)
            {
                dst.write (output, size);
                WebPFree (output);
                return true;
            }
        }
    }
    else if (data.pixelFormat == juce::Image::RGB)
    {
        if (loseless)
        {
            uint8_t* output = nullptr;
            auto size = WebPEncodeLosslessBGR (data.data, img.getWidth(), img.getHeight(), data.lineStride, &output);
            if (size > 0 && output != nullptr)
            {
                dst.write (output, size);
                WebPFree (output);
                return true;
            }
        }
        else
        {
            uint8_t* output = nullptr;
            auto size = WebPEncodeBGR (data.data, img.getWidth(), img.getHeight(), data.lineStride, quality, &output);
            if (size > 0 && output != nullptr)
            {
                dst.write (output, size);
                WebPFree (output);
                return true;
            }
        }
    }

    jassertfalse;
    return false;
}
