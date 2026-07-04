/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Cross-platform game controller manager.

    Create one instance — typically as a long-lived member of your application
    or game state — and register Listeners to receive connect/disconnect,
    button, and axis events on the JUCE message thread.

        class Game : private gin::GameControllerManager::Listener
        {
        public:
            Game() { controllers.addListener (this); }

            void controllerConnected (gin::GameController& c) override
            {
                DBG ("Connected: " << c.getName() << " on port " << c.getIndex());
            }

            void controllerButtonPressed (gin::GameController& c,
                                          gin::GameController::Button b) override
            {
                if (b == gin::GameController::Button::faceDown)
                    jump();
            }

        private:
            gin::GameControllerManager controllers;
        };

    The manager polls platform backends on a timer (default 60 Hz) and diffs
    state against the previous frame to emit listener callbacks. Polling rate
    can be changed with setPollIntervalHz().
*/
class GameControllerManager : private juce::Timer
{
public:
    GameControllerManager();
    ~GameControllerManager() override;

    /** Maximum number of simultaneously-tracked controllers. */
    static constexpr int maxControllers = 8;

    /** Returns the controller at the given port, or nullptr if out of range.

        The returned pointer is stable for the lifetime of the manager. Use
        GameController::isConnected() to check whether anything is plugged in.
    */
    GameController* getController (int index) noexcept;

    /** Returns the number of currently-connected controllers. */
    int getNumConnectedControllers() const noexcept;

    /** Trigger axis threshold above which the trigger is reported as a digital
        button press (Button::leftTrigger / Button::rightTrigger). Default 0.5. */
    void setTriggerThreshold (float threshold) noexcept   { triggerThreshold = threshold; }

    /** Sets the poll rate. Range is clamped to 10..240 Hz. Default 60. */
    void setPollIntervalHz (int hz);

    //==========================================================================
    class Listener
    {
    public:
        virtual ~Listener() = default;

        virtual void controllerConnected      (GameController&) {}
        virtual void controllerDisconnected   (GameController&) {}
        virtual void controllerButtonPressed  (GameController&, GameController::Button) {}
        virtual void controllerButtonReleased (GameController&, GameController::Button) {}
        virtual void controllerAxisMoved      (GameController&, GameController::Axis, float /*value*/) {}
    };

    void addListener    (Listener* l)  { listeners.add (l); }
    void removeListener (Listener* l)  { listeners.remove (l); }

    //==========================================================================
    /** Snapshot of a single controller's state, written by a Backend. */
    struct PollState
    {
        bool         connected = false;
        juce::String name;
        std::array<bool,  (size_t) GameController::Button::numButtons> buttons {};
        std::array<float, (size_t) GameController::Axis::numAxes>      axes    {};
    };

    /** Platform extension point. Don't instantiate directly — one is built
        internally for the current OS. */
    class Backend
    {
    public:
        virtual ~Backend() = default;
        virtual void poll (int port, PollState& out) = 0;
    };

private:
    void timerCallback() override;

    std::unique_ptr<Backend> backend;

    juce::OwnedArray<GameController> controllers;
    juce::ListenerList<Listener>     listeners;

    float triggerThreshold = 0.5f;
};
