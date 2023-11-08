/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
juce::String valueTreeToJSON (const juce::ValueTree& v);

juce::ValueTree valueTreeFromJSON (const juce::String& jsonText);

//==============================================================================
/** Listen to value tree changes with std::function<>
*/
class LambdaValueTreeListener : public juce::ValueTree::Listener
{
public:
    LambdaValueTreeListener (juce::ValueTree v_)
        : vt (std::move (v_))
    {
        vt.addListener (this);
    }

    std::function<void (juce::ValueTree&, const juce::Identifier&)> onValueTreePropertyChanged;
    std::function<void (juce::ValueTree&, juce::ValueTree&)> onValueTreeChildAdded;
    std::function<void (juce::ValueTree&, juce::ValueTree&, int)> onValueTreeChildRemoved;
    std::function<void (juce::ValueTree&, int, int)> onValueTreeChildOrderChanged;
    std::function<void (juce::ValueTree&)> onValueTreeParentChanged;
    std::function<void (juce::ValueTree&)> onValueTreeRedirected;

private:
    void valueTreePropertyChanged (juce::ValueTree&v , const juce::Identifier& i) override
    {
        if (onValueTreePropertyChanged)
            onValueTreePropertyChanged (v, i);
    }

    void valueTreeChildAdded (juce::ValueTree& p, juce::ValueTree& c) override
    {
        if (onValueTreeChildAdded)
            onValueTreeChildAdded (p, c);
    }

    void valueTreeChildRemoved (juce::ValueTree& p, juce::ValueTree& c, int n) override
    {
        if (onValueTreeChildRemoved)
            onValueTreeChildRemoved (p, c, n);

    }

    void valueTreeChildOrderChanged (juce::ValueTree& v, int a, int b) override
    {
        if (onValueTreeChildOrderChanged)
            onValueTreeChildOrderChanged (v, a, b);
    }

    void valueTreeParentChanged (juce::ValueTree& v) override
    {
        if (onValueTreeParentChanged)
            onValueTreeParentChanged (v);
    }

    void valueTreeRedirected (juce::ValueTree& v) override
    {
        if (onValueTreeRedirected)
            onValueTreeRedirected (v);
    }

    juce::ValueTree vt;
};

//==============================================================================
/** Listen to value tree changes with std::function<>, but Async
*/
class AsyncLambdaValueTreeListener : public juce::ValueTree::Listener
{
public:
    AsyncLambdaValueTreeListener (juce::ValueTree& v_)
        : vt (v_)
    {
        vt.addListener (this);
    }

    ~AsyncLambdaValueTreeListener() override
    {
        vt.removeListener (this);
    }

    std::function<void (const juce::ValueTree&, const juce::Identifier&)> onValueTreePropertyChanged;
    std::function<void (const juce::ValueTree&, const juce::ValueTree&)> onValueTreeChildAdded;
    std::function<void (const juce::ValueTree&, const juce::ValueTree&, int)> onValueTreeChildRemoved;
    std::function<void (const juce::ValueTree&, int, int)> onValueTreeChildOrderChanged;
    std::function<void (const juce::ValueTree&)> onValueTreeParentChanged;
    std::function<void (const juce::ValueTree&)> onValueTreeRedirected;

private:
    void valueTreePropertyChanged (juce::ValueTree& v, const juce::Identifier& i) override
    {
        juce::ValueTree vc = v;
        juce::Identifier ic = i;
        juce::WeakReference<AsyncLambdaValueTreeListener> weakThis (this);
        juce::MessageManager::getInstance()->callAsync ([this, weakThis, vc, ic]
                                                  {
                                                      if (weakThis != nullptr && onValueTreePropertyChanged)
                                                          onValueTreePropertyChanged (vc, ic);
                                                  });
    }

    void valueTreeChildAdded (juce::ValueTree& p, juce::ValueTree& c) override
    {
        juce::ValueTree pc = p;
        juce::ValueTree cc = c;
        juce::WeakReference<AsyncLambdaValueTreeListener> weakThis (this);
        juce::MessageManager::getInstance()->callAsync ([this, weakThis, pc, cc]
                                                  {
                                                      if (weakThis != nullptr && onValueTreeChildAdded)
                                                          onValueTreeChildAdded (pc, cc);
                                                  });
    }

    void valueTreeChildRemoved (juce::ValueTree& p, juce::ValueTree& c, int n) override
    {
        juce::ValueTree pc = p;
        juce::ValueTree cc = c;
        juce::WeakReference<AsyncLambdaValueTreeListener> weakThis (this);
        juce::MessageManager::getInstance()->callAsync ([this, weakThis, pc, cc, n]
                                                  {
                                                      if (weakThis != nullptr && onValueTreeChildRemoved)
                                                          onValueTreeChildRemoved (pc, cc, n);
                                                  });
    }

    void valueTreeChildOrderChanged (juce::ValueTree& v, int a, int b) override
    {
        juce::ValueTree vc = v;
        juce::WeakReference<AsyncLambdaValueTreeListener> weakThis (this);
        juce::MessageManager::getInstance()->callAsync ([this, weakThis, vc, a, b]
                                                  {
                                                      if (weakThis != nullptr && onValueTreeChildOrderChanged)
                                                          onValueTreeChildOrderChanged (vc, a, b);
                                                  });
    }

    void valueTreeParentChanged (juce::ValueTree& v) override
    {
        juce::ValueTree vc = v;
        juce::WeakReference<AsyncLambdaValueTreeListener> weakThis (this);
        juce::MessageManager::getInstance()->callAsync ([this, weakThis, vc]
                                                  {
                                                      if (weakThis != nullptr && onValueTreeParentChanged)
                                                          onValueTreeParentChanged (vc);
                                                  });
    }

    void valueTreeRedirected (juce::ValueTree& v) override
    {
        juce::ValueTree vc = v;
        juce::WeakReference<AsyncLambdaValueTreeListener> weakThis (this);
        juce::MessageManager::getInstance()->callAsync ([this, weakThis, vc]
                                                  {
                                                      if (weakThis != nullptr && onValueTreeRedirected)
                                                          onValueTreeRedirected (vc);
                                                  });
    }

    juce::ValueTree& vt;

    JUCE_DECLARE_WEAK_REFERENCEABLE (AsyncLambdaValueTreeListener)
};
