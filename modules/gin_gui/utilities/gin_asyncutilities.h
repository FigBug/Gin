/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
    Timer that calls a std::function callback instead of requiring virtual override.

    LambdaTimer provides a more convenient alternative to juce::Timer by accepting
    a lambda or std::function for the timer callback. This eliminates the need to
    create a subclass or override timerCallback() for simple timer operations.

    Key Features:
    - Accepts lambda functions or std::function callbacks
    - Same timing capabilities as juce::Timer
    - No need to create subclass for simple timers
    - Callback can be changed at runtime
    - Cleaner code for inline timer operations

    The onTimer function is called at the specified interval. All juce::Timer
    methods are available (startTimer, stopTimer, isTimerRunning, etc.).

    Usage:
    @code
    // Create with lambda
    LambdaTimer timer([this]() {
        updateDisplay();
    });
    timer.startTimer(100); // Call every 100ms

    // Create and assign later
    LambdaTimer timer;
    timer.onTimer = [this]() {
        checkForUpdates();
    };
    timer.startTimer(1000); // Call every second

    // Stop timer
    timer.stopTimer();
    @endcode

    @see juce::Timer, LambdaAsyncUpdater
*/
class LambdaTimer : public juce::Timer
{
public:
    LambdaTimer (std::function<void()> func = nullptr) : onTimer (func) {}

    std::function<void()> onTimer;

private:
    void timerCallback() override
    {
        if (onTimer)
            onTimer();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LambdaTimer)
};

/**
    Async updater that calls a std::function callback instead of requiring virtual override.

    LambdaAsyncUpdater provides a more convenient alternative to juce::AsyncUpdater by
    accepting a lambda or std::function for the async callback. This is useful for
    safely triggering message-thread operations from other threads without creating
    a full subclass.

    Key Features:
    - Accepts lambda functions or std::function callbacks
    - Thread-safe asynchronous message posting
    - Coalesces multiple trigger calls
    - No need to create subclass for simple async operations
    - Callback can be set at construction

    The onAsyncUpdate function is called on the message thread when triggerAsyncUpdate()
    is called from any thread. Multiple calls are coalesced into a single callback.

    Thread Safety:
    - triggerAsyncUpdate() is thread-safe
    - onAsyncUpdate callback runs on message thread only
    - Safe for cross-thread communication

    Usage:
    @code
    // Create with lambda (runs on message thread)
    LambdaAsyncUpdater updater([this]() {
        // This runs on the message thread
        updateUI();
        repaint();
    });

    // Trigger from audio thread or background thread
    void audioThreadCallback() {
        // Process audio...
        updater.triggerAsyncUpdate(); // Safely posts to message thread
    }

    // Multiple triggers coalesce into one callback
    updater.triggerAsyncUpdate();
    updater.triggerAsyncUpdate(); // Only one callback will occur
    @endcode

    @see juce::AsyncUpdater, LambdaTimer, callOnMainThreadBlocking
*/
class LambdaAsyncUpdater : public juce::AsyncUpdater
{
public:
    LambdaAsyncUpdater (std::function<void()> func) : onAsyncUpdate (func) {}

    std::function<void()> onAsyncUpdate;

private:
    void handleAsyncUpdate () override
    {
        if (onAsyncUpdate)
            onAsyncUpdate();
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LambdaAsyncUpdater)
};

void callOnMainThreadBlocking (std::function<void ()> func);

/**
    Mouse listener that uses std::function callbacks instead of virtual overrides.

    LambdaMouseListener provides a convenient alternative to juce::MouseListener by
    accepting lambda functions for each mouse event type. This is particularly useful
    when adding a mouse listener to a component to monitor its child components,
    avoiding the "double event" problem where the component would receive both its
    own mouse events and the forwarded ones from being its own listener.

    Key Features:
    - Separate lambda callbacks for each mouse event type
    - Avoids double-event issues with self-listening
    - No need to create subclass for mouse monitoring
    - Callbacks can be selectively assigned
    - Runtime callback configuration
    - Cleaner code for mouse event handling

    Available Callbacks:
    - onMouseMove: Mouse moved over component
    - onMouseEnter: Mouse entered component
    - onMouseExit: Mouse left component
    - onMouseDown: Mouse button pressed
    - onMouseDrag: Mouse dragged
    - onMouseUp: Mouse button released
    - onMouseDoubleClick: Double-click occurred
    - onMouseWheelMove: Mouse wheel scrolled
    - onMouseMagnify: Trackpad pinch/zoom gesture

    Only assign callbacks for the events you need. Unassigned callbacks are
    safely ignored.

    Usage:
    @code
    // Monitor child component mouse events
    LambdaMouseListener mouseMonitor;
    mouseMonitor.onMouseDown = [this](const MouseEvent& e) {
        DBG("Mouse clicked at: " + e.position.toString());
    };
    mouseMonitor.onMouseDrag = [this](const MouseEvent& e) {
        handleDrag(e.getDistanceFromDragStart());
    };

    // Add to child component (not to self!)
    childComponent.addMouseListener(&mouseMonitor, true);

    // Multiple event types
    LambdaMouseListener listener;
    listener.onMouseEnter = [](const MouseEvent& e) { showTooltip(); };
    listener.onMouseExit = [](const MouseEvent& e) { hideTooltip(); };
    listener.onMouseWheelMove = [](const MouseEvent& e, const MouseWheelDetails& w) {
        zoom(w.deltaY);
    };
    @endcode

    @see juce::MouseListener, juce::MouseEvent
*/
class LambdaMouseListener : public juce::MouseListener
{
public:
    LambdaMouseListener() = default;

    std::function<void (const juce::MouseEvent&)> onMouseMove;
    std::function<void (const juce::MouseEvent&)> onMouseEnter;
    std::function<void (const juce::MouseEvent&)> onMouseExit;
    std::function<void (const juce::MouseEvent&)> onMouseDown;
    std::function<void (const juce::MouseEvent&)> onMouseDrag;
    std::function<void (const juce::MouseEvent&)> onMouseUp;
    std::function<void (const juce::MouseEvent&)> onMouseDoubleClick;
    std::function<void (const juce::MouseEvent&, const juce::MouseWheelDetails&)> onMouseWheelMove;
    std::function<void (const juce::MouseEvent&, float)> onMouseMagnify;

private:
    void mouseMove (const juce::MouseEvent& e) override                                         { if (onMouseMove)          onMouseMove (e);        }
    void mouseEnter (const juce::MouseEvent& e) override                                        { if (onMouseEnter)         onMouseEnter (e);       }
    void mouseExit (const juce::MouseEvent& e) override                                         { if (onMouseExit)          onMouseExit (e);        }
    void mouseDown (const juce::MouseEvent& e) override                                         { if (onMouseDown)          onMouseDown (e);        }
    void mouseDrag (const juce::MouseEvent& e) override                                         { if (onMouseDrag)          onMouseDrag (e);        }
    void mouseUp (const juce::MouseEvent& e) override                                           { if (onMouseUp)            onMouseUp (e);          }
    void mouseDoubleClick (const juce::MouseEvent& e) override                                  { if (onMouseDoubleClick)   onMouseDoubleClick (e); }
    void mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& w) override  { if (onMouseWheelMove)     onMouseWheelMove (e, w);}
    void mouseMagnify (const juce::MouseEvent& e, float s) override                             { if (onMouseMagnify)       onMouseMagnify (e, s);  }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LambdaMouseListener)
};
