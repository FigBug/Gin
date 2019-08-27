/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.
#include "AppConfig.h"

//==============================================================================

#include "gin_network.h"

#include "network/oauth.cpp"

namespace gin
{
#include "network/http.cpp"
#include "network/securestreamingsocket.cpp"
}

#if _MSC_VER
 #pragma warning (push)
 #pragma warning (disable: 4244)
 #pragma warning (disable: 4100)
 #pragma warning (disable: 4267)
 #pragma warning (disable: 4456)
#elif __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wshadow"
 #pragma clang diagnostic ignored "-Wsign-conversion"
 #pragma clang diagnostic ignored "-Wconversion"
 #pragma clang diagnostic ignored "-Wshadow-field"
#endif

#include "3rdparty/easywsclient/easywsclient.hpp"
#include "3rdparty/easywsclient/easywsclient.cpp"

#if _MSC_VER
 #pragma warning (pop)
#elif __clang__
 #pragma clang diagnostic pop
#endif

namespace gin
{
#include "network/asyncwebsocket.cpp"
}
