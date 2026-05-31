/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#include <windows.h>
#include <Xinput.h>

#pragma comment (lib, "xinput.lib")

namespace
{
    constexpr int xinputMaxPorts = XUSER_MAX_COUNT; // 4

    constexpr float deadzone (int v, int dz)
    {
        if (v >  dz) return float (v - dz) / float (32767 - dz);
        if (v < -dz) return float (v + dz) / float (32768 - dz);
        return 0.0f;
    }

    juce::String wideToJuce (const wchar_t* s)
    {
        if (s == nullptr) return {};
        return juce::String (juce::CharPointer_UTF16 ((const juce::juce_wchar*) s));
    }
}

class XInputBackend : public GameControllerManager::Backend
{
public:
    void poll (int port, GameControllerManager::PollState& s) override
    {
        if (port >= xinputMaxPorts)
        {
            s.connected = false;
            return;
        }

        XINPUT_STATE st {};
        const DWORD result = XInputGetState ((DWORD) port, &st);

        if (result != ERROR_SUCCESS)
        {
            s.connected = false;
            return;
        }

        s.connected = true;

        // XInput offers no reliable product-name API short of XInputGetCapabilities +
        // device-instance lookup via setupapi. The Subtype gives us something useful.
        XINPUT_CAPABILITIES caps {};
        if (XInputGetCapabilities ((DWORD) port, 0, &caps) == ERROR_SUCCESS)
        {
            switch (caps.SubType)
            {
                case XINPUT_DEVSUBTYPE_GAMEPAD:       s.name = "XInput Gamepad";        break;
                case XINPUT_DEVSUBTYPE_WHEEL:         s.name = "XInput Wheel";          break;
                case XINPUT_DEVSUBTYPE_ARCADE_STICK:  s.name = "XInput Arcade Stick";   break;
                case XINPUT_DEVSUBTYPE_FLIGHT_STICK:  s.name = "XInput Flight Stick";   break;
                case XINPUT_DEVSUBTYPE_DANCE_PAD:     s.name = "XInput Dance Pad";      break;
                case XINPUT_DEVSUBTYPE_GUITAR:        s.name = "XInput Guitar";         break;
                case XINPUT_DEVSUBTYPE_DRUM_KIT:      s.name = "XInput Drum Kit";       break;
                case XINPUT_DEVSUBTYPE_ARCADE_PAD:    s.name = "XInput Arcade Pad";     break;
                default:                              s.name = "XInput Controller";     break;
            }
        }
        else
        {
            s.name = "XInput Controller";
        }

        const auto& g = st.Gamepad;

        auto button = [&] (GameController::Button b, WORD mask)
        {
            s.buttons[(size_t) b] = (g.wButtons & mask) != 0;
        };

        button (GameController::Button::dpadUp,        XINPUT_GAMEPAD_DPAD_UP);
        button (GameController::Button::dpadDown,      XINPUT_GAMEPAD_DPAD_DOWN);
        button (GameController::Button::dpadLeft,      XINPUT_GAMEPAD_DPAD_LEFT);
        button (GameController::Button::dpadRight,     XINPUT_GAMEPAD_DPAD_RIGHT);

        button (GameController::Button::faceDown,      XINPUT_GAMEPAD_A);
        button (GameController::Button::faceRight,     XINPUT_GAMEPAD_B);
        button (GameController::Button::faceLeft,      XINPUT_GAMEPAD_X);
        button (GameController::Button::faceUp,        XINPUT_GAMEPAD_Y);

        button (GameController::Button::leftShoulder,  XINPUT_GAMEPAD_LEFT_SHOULDER);
        button (GameController::Button::rightShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER);

        button (GameController::Button::select,        XINPUT_GAMEPAD_BACK);
        button (GameController::Button::start,         XINPUT_GAMEPAD_START);

        button (GameController::Button::leftStick,     XINPUT_GAMEPAD_LEFT_THUMB);
        button (GameController::Button::rightStick,    XINPUT_GAMEPAD_RIGHT_THUMB);

        // XInput exposes no public bit for the Guide / home button.
        s.buttons[(size_t) GameController::Button::home] = false;

        s.axes[(size_t) GameController::Axis::leftX]  =  deadzone (g.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        s.axes[(size_t) GameController::Axis::leftY]  = -deadzone (g.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        s.axes[(size_t) GameController::Axis::rightX] =  deadzone (g.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        s.axes[(size_t) GameController::Axis::rightY] = -deadzone (g.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

        s.axes[(size_t) GameController::Axis::leftTrigger]
            = g.bLeftTrigger  < XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? 0.0f : float (g.bLeftTrigger)  / 255.0f;
        s.axes[(size_t) GameController::Axis::rightTrigger]
            = g.bRightTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? 0.0f : float (g.bRightTrigger) / 255.0f;
    }
};

std::unique_ptr<GameControllerManager::Backend> createGameControllerBackend()
{
    return std::make_unique<XInputBackend>();
}
