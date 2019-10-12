/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
juce::var parsePlist (const juce::File& f);

juce::var parsePlist (const juce::String& s);

juce::var parsePlist (const juce::XmlElement& f);

