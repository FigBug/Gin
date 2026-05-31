/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

// Platform backend defined in the umbrella .cpp/.mm
std::unique_ptr<GameControllerManager::Backend> createGameControllerBackend();

//==============================================================================
GameControllerManager::GameControllerManager()
    : backend (createGameControllerBackend())
{
    controllers.ensureStorageAllocated (maxControllers);
    for (int i = 0; i < maxControllers; ++i)
    {
        auto* c = new GameController();
        c->index = i;
        controllers.add (c);
    }

    setPollIntervalHz (60);
}

GameControllerManager::~GameControllerManager()
{
    stopTimer();
}

GameController* GameControllerManager::getController (int index) noexcept
{
    return juce::isPositiveAndBelow (index, controllers.size()) ? controllers[index] : nullptr;
}

int GameControllerManager::getNumConnectedControllers() const noexcept
{
    int n = 0;
    for (auto* c : controllers)
        if (c->connected)
            ++n;
    return n;
}

void GameControllerManager::setPollIntervalHz (int hz)
{
    hz = juce::jlimit (10, 240, hz);
    startTimerHz (hz);
}

void GameControllerManager::timerCallback()
{
    PollState s;

    for (int i = 0; i < controllers.size(); ++i)
    {
        auto* c = controllers[i];

        s = {};
        backend->poll (i, s);

        // Promote analog trigger crossings to digital trigger buttons.
        s.buttons[(size_t) GameController::Button::leftTrigger]
            = s.axes[(size_t) GameController::Axis::leftTrigger]  >= triggerThreshold;
        s.buttons[(size_t) GameController::Button::rightTrigger]
            = s.axes[(size_t) GameController::Axis::rightTrigger] >= triggerThreshold;

        if (s.connected != c->connected)
        {
            c->connected = s.connected;
            c->name      = s.name;

            if (s.connected)
            {
                c->buttons = s.buttons;
                c->axes    = s.axes;
                listeners.call ([c] (Listener& l) { l.controllerConnected (*c); });
            }
            else
            {
                // Clear state so a held button doesn't appear pressed after unplug.
                c->buttons.fill (false);
                c->axes.fill (0.0f);
                listeners.call ([c] (Listener& l) { l.controllerDisconnected (*c); });
            }
            continue;
        }

        if (! s.connected)
            continue;

        // Name might change if the device was reconfigured; harmless to copy.
        c->name = s.name;

        for (size_t b = 1; b < c->buttons.size(); ++b)
        {
            if (s.buttons[b] != c->buttons[b])
            {
                c->buttons[b] = s.buttons[b];
                const auto button = (GameController::Button) b;
                if (s.buttons[b])
                    listeners.call ([c, button] (Listener& l) { l.controllerButtonPressed  (*c, button); });
                else
                    listeners.call ([c, button] (Listener& l) { l.controllerButtonReleased (*c, button); });
            }
        }

        for (size_t a = 0; a < c->axes.size(); ++a)
        {
            // Bitwise compare — we want a callback on any change, no epsilon.
            if (std::memcmp (&s.axes[a], &c->axes[a], sizeof (float)) != 0)
            {
                c->axes[a] = s.axes[a];
                const auto axis  = (GameController::Axis) a;
                const auto value = s.axes[a];
                listeners.call ([c, axis, value] (Listener& l) { l.controllerAxisMoved (*c, axis, value); });
            }
        }
    }
}
