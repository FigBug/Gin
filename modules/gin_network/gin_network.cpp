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
