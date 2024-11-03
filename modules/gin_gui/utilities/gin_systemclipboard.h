/*==============================================================================

 Copyright 2024 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/**
    Handles reading/writing to the system's clipboard.
*/
class SystemClipboard
{
public:
    /** Copies a image onto the clipboard */
    static void copyImageToClipboard (const juce::Image& image);
    
    /** Pastes an image from the clipboard */
    static juce::Image getImageFromClipboard();
};
