#if defined (__clang__)
 #pragma clang diagnostic ignored "-Wimplicit-int-conversion"
 #pragma clang diagnostic ignored "-Wsign-conversion"
 #if __has_warning("-Wc++-keyword")
  #pragma clang diagnostic ignored "-Wc++-keyword"
 #endif
#endif

#include "3rdparty/bsdiff/bsdiff.c"
#include "3rdparty/bsdiff/bspatch.c"
