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
