/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#ifdef  _WIN32
 #include <Windows.h>
#else
 #include <unistd.h>
#endif

// clang on windows uses msvc name mangling for compatibility. JUCE_WINDOWS isn't
// defined until the JUCE headers below are included, so test the compiler macro here
#ifndef _WIN32
 #include <cxxabi.h>
#endif

#include "gin_remote.h"

namespace gin
{

#include "remote/gin_remoteserver.cpp"
#include "remote/gin_remotecommands.cpp"

}
