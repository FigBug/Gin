/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

// System headers (fcntl.h, unistd.h, linux/joystick.h, etc.) are hoisted
// at global scope from gin_controllers.cpp before this file is included
// inside namespace gin — including them here would put `::open`, `::close`,
// `::ioctl` into `gin::` and break the calls below.

namespace
{
    // Standard xpad (Xbox-style) layout. Most popular pads — Xbox, PS via
    // ds4drv/dualsensectl, generic XInput-compatible USB pads — present this
    // ordering through joydev. Vendor-specific quirks would need an SDL-style
    // mapping database, which is outside the scope of this module.
    constexpr int kBtnA      = 0;
    constexpr int kBtnB      = 1;
    constexpr int kBtnX      = 2;
    constexpr int kBtnY      = 3;
    constexpr int kBtnLB     = 4;
    constexpr int kBtnRB     = 5;
    constexpr int kBtnSelect = 6;
    constexpr int kBtnStart  = 7;
    constexpr int kBtnHome   = 8;
    constexpr int kBtnLStick = 9;
    constexpr int kBtnRStick = 10;

    constexpr int kAxisLX = 0;
    constexpr int kAxisLY = 1;
    constexpr int kAxisLT = 2;
    constexpr int kAxisRX = 3;
    constexpr int kAxisRY = 4;
    constexpr int kAxisRT = 5;
    constexpr int kAxisDX = 6;   // dpad as analog axis (-1 / 0 / +1)
    constexpr int kAxisDY = 7;
}

class JoydevBackend : public GameControllerManager::Backend
{
public:
    JoydevBackend()
    {
        devices.resize ((size_t) GameControllerManager::maxControllers);
    }

    ~JoydevBackend() override
    {
        for (auto& d : devices)
            d.close();
    }

    void poll (int port, GameControllerManager::PollState& s) override
    {
        if (port < 0 || port >= (int) devices.size())
        {
            s.connected = false;
            return;
        }

        auto& dev = devices[(size_t) port];

        if (dev.fd < 0)
            dev.tryOpen (port);

        if (dev.fd < 0)
        {
            s.connected = false;
            return;
        }

        // Drain pending events into the cached state.
        for (;;)
        {
            js_event e {};
            const ssize_t r = ::read (dev.fd, &e, sizeof (e));
            if (r != (ssize_t) sizeof (e))
            {
                if (r < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
                    break;

                // ENODEV when device is unplugged mid-stream — drop and let the
                // next poll attempt to reopen.
                dev.close();
                s.connected = false;
                return;
            }

            const bool isInit = (e.type & JS_EVENT_INIT) != 0;
            const auto type   = e.type & ~JS_EVENT_INIT;

            if (type == JS_EVENT_BUTTON && e.number < dev.btnState.size())
                dev.btnState[e.number] = (e.value != 0);
            else if (type == JS_EVENT_AXIS && e.number < dev.axisState.size())
                dev.axisState[e.number] = e.value;

            (void) isInit;
        }

        s.connected = true;
        s.name      = dev.name;

        auto setBtnIfValid = [&] (GameController::Button b, int idx)
        {
            if (idx >= 0 && idx < (int) dev.btnState.size())
                s.buttons[(size_t) b] = dev.btnState[(size_t) idx];
        };

        setBtnIfValid (GameController::Button::faceDown,      kBtnA);
        setBtnIfValid (GameController::Button::faceRight,     kBtnB);
        setBtnIfValid (GameController::Button::faceLeft,      kBtnX);
        setBtnIfValid (GameController::Button::faceUp,        kBtnY);
        setBtnIfValid (GameController::Button::leftShoulder,  kBtnLB);
        setBtnIfValid (GameController::Button::rightShoulder, kBtnRB);
        setBtnIfValid (GameController::Button::select,        kBtnSelect);
        setBtnIfValid (GameController::Button::start,         kBtnStart);
        setBtnIfValid (GameController::Button::home,          kBtnHome);
        setBtnIfValid (GameController::Button::leftStick,     kBtnLStick);
        setBtnIfValid (GameController::Button::rightStick,    kBtnRStick);

        auto axis = [&] (int idx) -> float
        {
            if (idx < 0 || idx >= (int) dev.axisState.size())
                return 0.0f;
            return juce::jlimit (-1.0f, 1.0f, float (dev.axisState[(size_t) idx]) / 32767.0f);
        };

        s.axes[(size_t) GameController::Axis::leftX]  = axis (kAxisLX);
        s.axes[(size_t) GameController::Axis::leftY]  = axis (kAxisLY);
        s.axes[(size_t) GameController::Axis::rightX] = axis (kAxisRX);
        s.axes[(size_t) GameController::Axis::rightY] = axis (kAxisRY);

        // Triggers come through as -1..1 on joydev; remap to 0..1.
        s.axes[(size_t) GameController::Axis::leftTrigger]  = juce::jlimit (0.0f, 1.0f, (axis (kAxisLT) + 1.0f) * 0.5f);
        s.axes[(size_t) GameController::Axis::rightTrigger] = juce::jlimit (0.0f, 1.0f, (axis (kAxisRT) + 1.0f) * 0.5f);

        // Convert dpad axes to digital button state.
        const float dx = axis (kAxisDX);
        const float dy = axis (kAxisDY);
        s.buttons[(size_t) GameController::Button::dpadLeft]  = dx < -0.5f;
        s.buttons[(size_t) GameController::Button::dpadRight] = dx >  0.5f;
        s.buttons[(size_t) GameController::Button::dpadUp]    = dy < -0.5f;
        s.buttons[(size_t) GameController::Button::dpadDown]  = dy >  0.5f;
    }

private:
    struct Device
    {
        int fd = -1;
        juce::String name;
        std::array<bool,   256> btnState  {};
        std::array<int16_t,  64> axisState {};

        void tryOpen (int port)
        {
            char path[64];
            std::snprintf (path, sizeof (path), "/dev/input/js%d", port);

            const int f = ::open (path, O_RDONLY | O_NONBLOCK);
            if (f < 0)
                return;

            char nameBuf[256] = {};
            if (::ioctl (f, JSIOCGNAME (sizeof (nameBuf) - 1), nameBuf) < 0)
                std::snprintf (nameBuf, sizeof (nameBuf), "joydev js%d", port);

            fd   = f;
            name = juce::String::fromUTF8 (nameBuf);
            btnState.fill (false);
            axisState.fill (0);
        }

        void close()
        {
            if (fd >= 0)
                ::close (fd);
            fd = -1;
            name.clear();
            btnState.fill (false);
            axisState.fill (0);
        }
    };

    std::vector<Device> devices;
};

std::unique_ptr<GameControllerManager::Backend> createGameControllerBackend()
{
    return std::make_unique<JoydevBackend>();
}
