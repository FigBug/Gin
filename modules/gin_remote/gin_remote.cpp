/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#ifdef  _WIN32
 #include <Windows.h>
#else
 #include <unistd.h>
#endif

// clang on windows uses msvc name mangling for compatibility
#if !JUCE_WINDOWS
 #include <cxxabi.h>
#endif

#include "gin_remote.h"

namespace gin
{

#include "remote/gin_remoteserver.cpp"
#include "remote/gin_remotecommands.cpp"

}
