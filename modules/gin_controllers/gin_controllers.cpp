/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#if __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wconversion"
 #pragma clang diagnostic ignored "-Wshadow"
 #pragma clang diagnostic ignored "-Wunused-parameter"
#elif _MSC_VER
 #pragma warning (push)
 #pragma warning (disable: 4100)
 #pragma warning (disable: 4244)
#endif

#include "gin_controllers.h"

#if __clang__
 #pragma clang diagnostic pop
#elif _MSC_VER
 #pragma warning (pop)
#endif

// Hoist platform system headers to global scope before we open `namespace gin`
// — otherwise glibc / Win32 / SDK headers and their free functions (`::open`,
// `XInputGetState`, ...) end up inside the namespace, which the backends
// reference via the global scope. macOS's `GameController/GameController.h`
// is hoisted from gin_controllers.mm instead.
#if JUCE_WINDOWS
 #include <windows.h>
 #include <Xinput.h>
#elif JUCE_LINUX || JUCE_BSD
 #include <fcntl.h>
 #include <unistd.h>
 #include <sys/ioctl.h>
 #include <linux/joystick.h>
 #include <errno.h>
 #include <string.h>
#endif

namespace gin
{

#include "controllers/gin_gamecontrollermanager.cpp"

#if JUCE_WINDOWS
 #include "controllers/gin_gamecontrollerbackend_windows.cpp"
#elif JUCE_MAC
 #include "controllers/gin_gamecontrollerbackend_macos.mm"
#elif JUCE_LINUX || JUCE_BSD
 #include "controllers/gin_gamecontrollerbackend_linux.cpp"
#else
 // No backend on this platform — provide a stub so the module still links.
 class NullGameControllerBackend : public GameControllerManager::Backend
 {
 public:
     void poll (int, GameControllerManager::PollState& s) override { s.connected = false; }
 };

 std::unique_ptr<GameControllerManager::Backend> createGameControllerBackend()
 {
     return std::make_unique<NullGameControllerBackend>();
 }
#endif

} // namespace gin
