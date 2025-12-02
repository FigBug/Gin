/*==============================================================================

 Copyright 2024 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================

#if JUCE_MAC
/** macOS-specific image conversion utilities.

    This namespace provides functions for converting between JUCE images
    and native macOS NSImage objects, allowing interoperability with
    macOS-specific APIs.
*/
namespace macOS
{
    /** Converts a macOS NSImage to a JUCE Image.

        Useful when receiving images from macOS APIs (such as pasteboard,
        drag and drop, or system icons) and needing to use them in JUCE.

        @param nsImage  The NSImage to convert
        @returns        A JUCE Image containing the same pixel data
    */
    juce::Image nsImageToImage (NSImage* nsImage);

    /** Converts a JUCE Image to a macOS NSImage.

        Useful when passing images to macOS APIs that require NSImage objects.

        @param image  The JUCE image to convert
        @returns      An NSImage containing the same pixel data
    */
    NSImage* imageToNSImage (const juce::Image& image);
}
#endif

#if JUCE_WINDOWS
/** Windows-specific image conversion utilities.

    This namespace provides functions for converting between JUCE images
    and native Windows HBITMAP handles, allowing interoperability with
    Windows GDI and other Windows-specific APIs.
*/
namespace windows
{
    /** Converts a Windows HBITMAP to a JUCE Image.

        Useful when receiving bitmaps from Windows APIs (such as the clipboard,
        GDI operations, or system icons) and needing to use them in JUCE.

        @param hBitmap  The HBITMAP handle to convert
        @returns        A JUCE Image containing the same pixel data
    */
    juce::Image hBitmapToImage (HBITMAP hBitmap);
}
#endif
