/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

// Note: <GameController/GameController.h> is imported at global scope from
// gin_controllers.mm before this file is included inside namespace gin.

class GameControllerMacBackend : public GameControllerManager::Backend
{
public:
    GameControllerMacBackend()
    {
        // GameController.framework discovers controllers asynchronously — calling
        // startWirelessControllerDiscoveryWithCompletionHandler kicks off the BT
        // scan so wireless pads show up shortly after the app starts.
        [GCController startWirelessControllerDiscoveryWithCompletionHandler: ^{}];
    }

    ~GameControllerMacBackend() override
    {
        [GCController stopWirelessControllerDiscovery];

        for (id ptr : pinned)
            if (ptr != nil)
                CFRelease ((__bridge CFTypeRef) ptr);
        pinned.clear();
    }

    void poll (int port, GameControllerManager::PollState& s) override
    {
        @autoreleasepool
        {
            updatePortMap();

            if (port >= (int) pinned.size())
            {
                s.connected = false;
                return;
            }

            GCController* controller = (__bridge GCController*) pinned[(size_t) port];
            if (controller == nil)
            {
                s.connected = false;
                return;
            }

            s.connected = true;
            s.name = controller.vendorName != nil
                       ? juce::String::fromUTF8 ([controller.vendorName UTF8String])
                       : juce::String ("Game Controller");

            GCExtendedGamepad* g = controller.extendedGamepad;
            if (g == nil)
                return; // Connected but not an extended gamepad — leave inputs at default.

            auto setBtn = [&] (GameController::Button b, GCControllerButtonInput* input)
            {
                s.buttons[(size_t) b] = (input != nil) && input.isPressed;
            };

            setBtn (GameController::Button::faceDown,      g.buttonA);
            setBtn (GameController::Button::faceRight,     g.buttonB);
            setBtn (GameController::Button::faceLeft,      g.buttonX);
            setBtn (GameController::Button::faceUp,        g.buttonY);

            setBtn (GameController::Button::leftShoulder,  g.leftShoulder);
            setBtn (GameController::Button::rightShoulder, g.rightShoulder);

            setBtn (GameController::Button::dpadUp,        g.dpad.up);
            setBtn (GameController::Button::dpadDown,      g.dpad.down);
            setBtn (GameController::Button::dpadLeft,      g.dpad.left);
            setBtn (GameController::Button::dpadRight,     g.dpad.right);

            // Stick clicks were added in macOS 10.14.1.
            if (@available (macOS 10.14.1, *))
            {
                setBtn (GameController::Button::leftStick,  g.leftThumbstickButton);
                setBtn (GameController::Button::rightStick, g.rightThumbstickButton);
            }

            // Menu/start, options/select were added at different points.
            setBtn (GameController::Button::start, g.buttonMenu);

            if (@available (macOS 10.15, *))
                setBtn (GameController::Button::select, g.buttonOptions);

            if (@available (macOS 11.0, *))
                setBtn (GameController::Button::home, g.buttonHome);

            s.axes[(size_t) GameController::Axis::leftX]  =  g.leftThumbstick.xAxis.value;
            s.axes[(size_t) GameController::Axis::leftY]  = -g.leftThumbstick.yAxis.value;
            s.axes[(size_t) GameController::Axis::rightX] =  g.rightThumbstick.xAxis.value;
            s.axes[(size_t) GameController::Axis::rightY] = -g.rightThumbstick.yAxis.value;

            s.axes[(size_t) GameController::Axis::leftTrigger]  = g.leftTrigger.value;
            s.axes[(size_t) GameController::Axis::rightTrigger] = g.rightTrigger.value;
        }
    }

private:
    void updatePortMap()
    {
        NSArray<GCController*>* current = [GCController controllers];

        const size_t maxPorts = (size_t) GameControllerManager::maxControllers;
        pinned.resize (maxPorts, nil);

        // Drop pins that are no longer in [GCController controllers].
        for (size_t i = 0; i < pinned.size(); ++i)
        {
            if (pinned[i] == nil)
                continue;

            bool stillPresent = false;
            for (GCController* c in current)
                if ((__bridge id) c == pinned[i])
                    { stillPresent = true; break; }

            if (! stillPresent)
            {
                CFRelease ((__bridge CFTypeRef) pinned[i]);
                pinned[i] = nil;
            }
        }

        // Add new controllers to the first free port.
        for (GCController* c in current)
        {
            bool alreadyPinned = false;
            for (id p : pinned)
                if ((__bridge id) c == p)
                    { alreadyPinned = true; break; }

            if (alreadyPinned)
                continue;

            for (size_t i = 0; i < pinned.size(); ++i)
            {
                if (pinned[i] == nil)
                {
                    pinned[i] = c;
                    CFRetain ((__bridge CFTypeRef) c);
                    break;
                }
            }
        }
    }

    std::vector<id> pinned;
};

std::unique_ptr<GameControllerManager::Backend> createGameControllerBackend()
{
    return std::make_unique<GameControllerMacBackend>();
}
