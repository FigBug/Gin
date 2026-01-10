/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================

#include "gin_standaloneplugin.h"

#include <juce_audio_plugin_client/detail/juce_CreatePluginFilter.h>

namespace gin
{
#include "standaloneplugin/gin_audioprocessorplayer.cpp"
#include "standaloneplugin/gin_sidebarcomponent.cpp"
#include "standaloneplugin/gin_maincontentcomponent.cpp"
#include "standaloneplugin/gin_standalonepluginholder.cpp"
#include "standaloneplugin/gin_standaloneapp.cpp"
#include "standaloneplugin/gin_standalonefilterwindow.cpp"
}

juce::JUCEApplicationBase* juce_CreateApplication()
{
    return new gin::StandaloneFilterApp();
}
