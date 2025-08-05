/*==============================================================================

 Copyright 2024 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
#if JUCE_MAC
juce::Image nsImageToImage (NSImage* nsImage)
{
    if (nsImage != nil)
    {
        if (CGImageRef cgImage = [nsImage CGImageForProposedRect:nullptr context:nil hints:nil])
        {
            const auto width = CGImageGetWidth (cgImage);
            const auto height = CGImageGetHeight (cgImage);
            const auto cgRect = CGRectMake (0, 0, (CGFloat) width, (CGFloat) height);
            const juce::Image image (juce::Image::ARGB, (int) width, (int) height, true);

            CGContextDrawImage (juce_getImageContext (image), cgRect, cgImage);

            return image;
        }
    }
    return {};
}

NSImage* imageToNSImage (const juce::Image& image)
{
    const auto scaleFactor = 1.0f;

    using ColorSpacePtr     = juce::CFUniquePtr<CGColorSpaceRef>;
    using ImagePtr          = juce::CFUniquePtr<CGImageRef>;

    NSImage* im = [[NSImage alloc] init];
    auto requiredSize = NSMakeSize (image.getWidth() / scaleFactor, image.getHeight() / scaleFactor);

    [im setSize: requiredSize];
    ColorSpacePtr colourSpace { CGColorSpaceCreateWithName (kCGColorSpaceSRGB) };
    ImagePtr imageRef { juce::juce_createCoreGraphicsImage (image, colourSpace.get()) };

    NSBitmapImageRep* imageRep = [[NSBitmapImageRep alloc] initWithCGImage: imageRef.get()];
    [imageRep setSize: requiredSize];
    [im addRepresentation: imageRep];
    [imageRep release];
    return im;
}
#endif

#if JUCE_WINDOWS
namespace windows
{
juce::Image hBitmapToImage (void* hBitmap_)
{
    HBITMAP hBitmap = (HBITMAP)hBitmap_;

    if (hBitmap == nullptr)
        return {};

    BITMAP bmp;
    if (! GetObject (hBitmap, sizeof(BITMAP), &bmp))
        return {};

    const int width  = bmp.bmWidth;
    const int height = bmp.bmHeight;

    // Create a compatible DC and select the bitmap into it
    HDC hdcScreen = GetDC (nullptr);
    HDC hdcMem = CreateCompatibleDC (hdcScreen);
    HBITMAP oldBitmap = (HBITMAP)SelectObject (hdcMem, hBitmap);

    // Create a JUCE image to hold the bitmap data
    juce::Image juceImage = juce::Image (juce::Image::PixelFormat::ARGB, width, height, false);

    // Prepare a BITMAPINFO to retrieve the pixels
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Negative to indicate top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Allocate a buffer for the pixels
    juce::HeapBlock<uint8_t> pixelData;
    pixelData.allocate (width * height * 4, true); // 4 bytes per pixel (ARGB)

    // Get the bitmap pixels
    if (GetDIBits (hdcMem, hBitmap, 0, height, pixelData.getData(), &bmi, DIB_RGB_COLORS))
    {
        juce::Image::BitmapData destData (juceImage, juce::Image::BitmapData::writeOnly);

        for (int y = 0; y < height; ++y)
        {
            const uint8_t* src = pixelData.getData() + y * width * 4;
            uint8_t* dest = destData.getLinePointer(y);

            for (int x = 0; x < width; ++x)
            {
                uint8_t r = *src++;
                uint8_t g = *src++;
                uint8_t b = *src++;
                uint8_t a = *src++;

                dest[0] = r;
                dest[1] = g;
                dest[2] = b;
                dest[3] = a;

                dest += 4;
            }
        }
    }

    // Clean up
    SelectObject (hdcMem, oldBitmap);
    DeleteDC (hdcMem);
    ReleaseDC (nullptr, hdcScreen);

    return juceImage;
}
}
#endif