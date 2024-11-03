/*==============================================================================

 Copyright 2024 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
#if JUCE_MAC
juce::Image createJuceImage (NSImage* nsImage)
{
    if (nsImage != nil)
    {
        [nsImage lockFocus];
        NSBitmapImageRep* bitmapRep = [[NSBitmapImageRep alloc] initWithFocusedViewRect:NSMakeRect (0, 0, nsImage.size.width, nsImage.size.height)];
        [nsImage unlockFocus];
        
        NSDictionary* dic = [NSDictionary dictionary];
        NSData* data = [bitmapRep representationUsingType:NSPNGFileType properties:dic];
        
        auto res = juce::PNGImageFormat::loadFrom (data.bytes, data.length);
        [bitmapRep release];
        
        return res;
    }
    return {};
}

NSImage* createNSImage (const juce::Image& image)
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


void SystemClipboard::copyImageToClipboard (const juce::Image& image)
{
   #if JUCE_MAC
    JUCE_AUTORELEASEPOOL
    {
        auto nsImage = createNSImage (image);
        
        NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
        [pasteboard clearContents];
        NSArray* copiedObjects = [NSArray arrayWithObject:nsImage];
        [pasteboard writeObjects: copiedObjects];
    }
   #else
    jassertfalse;
   #endif
}

juce::Image SystemClipboard::getImageFromClipboard()
{
   #if JUCE_MAC
    JUCE_AUTORELEASEPOOL
    {
        NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
        NSArray* classArray = [NSArray arrayWithObject:[NSImage class]];
        NSDictionary* options = [NSDictionary dictionary];
        BOOL ok = [pasteboard canReadObjectForClasses:classArray options:options];
        if (ok)
        {
            NSArray* objectsToPaste = [pasteboard readObjectsForClasses:classArray options:options];
            NSImage* image = [objectsToPaste objectAtIndex:0];
            
            return createJuceImage (image);
        }
    }
    return {};
   #else
    jassertfalse;
    return {};
   #endif
}
