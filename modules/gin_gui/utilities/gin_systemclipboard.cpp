/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================

void SystemClipboard::copyImageToClipboard (const juce::Image& image)
{
   #if JUCE_MAC
    JUCE_AUTORELEASEPOOL
    {
        auto nsImage = imageToNSImage (image);
        
        NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
        [pasteboard clearContents];
        NSArray* copiedObjects = [NSArray arrayWithObject:nsImage];
        [pasteboard writeObjects: copiedObjects];
    }
   #else
    juce::ignoreUnused (image);
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
            
            return nsImageToImage (image);
        }
    }
    return {};
   #else
    jassertfalse;
    return {};
   #endif
}
