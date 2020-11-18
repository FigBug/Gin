/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#ifdef  _WIN32
 #include <Windows.h>
 #include <ctime>
#endif

//==============================================================================

#include "gin_metadata.h"

//==============================================================================


namespace gin
{

#include "metadata/gin_libjpegpng.cpp"
#include "metadata/gin_commentmetadata.cpp"
#include "metadata/gin_exifmetadata.cpp"
#include "metadata/gin_imagemetadata.cpp"
#include "metadata/gin_iptcmetadata.cpp"
#include "metadata/gin_xmpmetadata.cpp"

}
