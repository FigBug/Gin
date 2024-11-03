/*==============================================================================

 Copyright 2024 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================

#if JUCE_MAC
namespace macOS
{
juce::Image nsImageToImage (NSImage* nsImage);

NSImage* imageToNSImage (const juce::Image& image);
}
#endif
