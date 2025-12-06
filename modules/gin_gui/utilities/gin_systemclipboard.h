/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** Utilities for reading and writing images to the system clipboard.

    This class provides platform-specific clipboard operations for images,
    going beyond JUCE's text-only clipboard support. It handles the native
    image formats for each platform (NSImage on macOS, HBITMAP on Windows, etc.).

    Unlike juce::SystemClipboard which only handles text, this allows you to
    copy and paste images directly to/from the system clipboard for integration
    with other applications.

    @see copyImageToClipboard, getImageFromClipboard
*/
class SystemClipboard
{
public:
    /** Copies an image to the system clipboard.

        The image will be converted to the platform's native format and placed
        on the clipboard, making it available to other applications.

        @param image  The JUCE image to copy to the clipboard
    */
    static void copyImageToClipboard (const juce::Image& image);

    /** Retrieves an image from the system clipboard.

        If the clipboard contains an image in a supported format, it will be
        converted to a JUCE Image and returned.

        @returns  The image from the clipboard, or an invalid Image if the clipboard
                  doesn't contain an image
    */
    static juce::Image getImageFromClipboard();
};
