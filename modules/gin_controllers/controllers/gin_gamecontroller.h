/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Represents a single connected game controller.

    GameController instances are owned by the GameControllerManager and live for
    the life of that manager — a disconnect just flips isConnected() to false so
    pointers held by listeners stay valid until they're reconnected on the same
    port.

    Button and axis layouts are normalised across backends so the same enum
    value means the same physical control on Xbox, PlayStation, MFi, etc.
    Face buttons are named by position (faceDown / faceRight / faceUp / faceLeft)
    rather than letter so the mapping holds regardless of vendor:

        Xbox A = PS Cross    = faceDown
        Xbox B = PS Circle   = faceRight
        Xbox Y = PS Triangle = faceUp
        Xbox X = PS Square   = faceLeft
*/
class GameController
{
public:
    enum class Button
    {
        unknown = 0,

        // D-pad
        dpadUp,
        dpadRight,
        dpadDown,
        dpadLeft,

        // Face buttons
        faceDown,
        faceRight,
        faceUp,
        faceLeft,

        // Shoulders / triggers
        leftShoulder,
        rightShoulder,
        leftTrigger,   // digital threshold of the analog trigger
        rightTrigger,

        // Middle buttons
        select,        // Back / Share / View / -
        home,          // Guide / PS / Xbox button
        start,         // Start / Options / Menu / +

        // Stick clicks
        leftStick,
        rightStick,

        numButtons
    };

    enum class Axis
    {
        leftX = 0,
        leftY,
        rightX,
        rightY,
        leftTrigger,    // 0..1
        rightTrigger,   // 0..1

        numAxes
    };

    /** Port index (0..maxControllers - 1). Stable across disconnect/reconnect. */
    int getIndex() const noexcept            { return index; }

    /** Human-readable product name reported by the platform backend. */
    const juce::String& getName() const noexcept   { return name; }

    /** True between a controllerConnected and a controllerDisconnected. */
    bool isConnected() const noexcept        { return connected; }

    bool isButtonDown (Button b) const noexcept
    {
        const auto i = (size_t) b;
        return i < buttons.size() && buttons[i];
    }

    /** Analog axis value. Sticks: -1..1. Triggers: 0..1. */
    float getAxis (Axis a) const noexcept
    {
        const auto i = (size_t) a;
        return i < axes.size() ? axes[i] : 0.0f;
    }

private:
    friend class GameControllerManager;

    int  index = 0;
    bool connected = false;
    juce::String name;

    std::array<bool,  (size_t) Button::numButtons> buttons {};
    std::array<float, (size_t) Axis::numAxes>      axes    {};
};
