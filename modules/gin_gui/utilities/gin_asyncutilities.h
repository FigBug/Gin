/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
 Like juce::Timer, but it calls a std::function<>
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
 Like juce::AsyncUpdater, but it calls a std::function<>
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
};

void callOnMainThreadBlocking (std::function<void ()> func);

/**
 If you add a mouse listener to yourself to get nested mouse messages, then you get
 them all twice, so use a lambda mouse listener instead
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
