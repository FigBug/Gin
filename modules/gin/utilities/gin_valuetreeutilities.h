/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
juce::String valueTreeToJSON (const juce::ValueTree& v);

juce::ValueTree valueTreeFromJSON (const juce::String& jsonText);

//==============================================================================
class LambdaValueTreeListener : public ValueTree::Listener
{
public:
    LambdaValueTreeListener (ValueTree& v_)
        : vt (v_)
    {
        vt.addListener (this);
    }
    
    ~LambdaValueTreeListener() override
    {
        vt.removeListener (this);
    }

    std::function<void (ValueTree&, const Identifier&)> onValueTreePropertyChanged;
    std::function<void (ValueTree&, ValueTree&)> onValueTreeChildAdded;
    std::function<void (ValueTree&, ValueTree&, int)> onValueTreeChildRemoved;
    std::function<void (ValueTree&, int, int)> onValueTreeChildOrderChanged;
    std::function<void (ValueTree&)> onValueTreeParentChanged;
    std::function<void (ValueTree&)> onValueTreeRedirected;
    
private:
    void valueTreePropertyChanged (ValueTree&v , const Identifier& i) override
    {
        if (onValueTreePropertyChanged)
            onValueTreePropertyChanged (v, i);
    }
    
    void valueTreeChildAdded (ValueTree& p, ValueTree& c) override
    {
        if (onValueTreeChildAdded)
            onValueTreeChildAdded (p, c);
    }
    
    void valueTreeChildRemoved (ValueTree& p, ValueTree& c, int n) override
    {
        if (onValueTreeChildRemoved)
            onValueTreeChildRemoved (p, c, n);
        
    }
    
    void valueTreeChildOrderChanged (ValueTree& v, int a, int b) override
    {
        if (onValueTreeChildOrderChanged)
            onValueTreeChildOrderChanged (v, a, b);
    }
    
    void valueTreeParentChanged (ValueTree& v) override
    {
        if (onValueTreeParentChanged)
            onValueTreeParentChanged (v);
    }
    
    void valueTreeRedirected (ValueTree& v) override
    {
        if (onValueTreeRedirected)
            onValueTreeRedirected (v);
    }
    
    ValueTree& vt;
};

//==============================================================================
class AsyncLambdaValueTreeListener : public ValueTree::Listener
{
public:
    AsyncLambdaValueTreeListener (ValueTree& v_)
        : vt (v_)
    {
        vt.addListener (this);
    }
    
    ~AsyncLambdaValueTreeListener() override
    {
        vt.removeListener (this);
    }
    
    std::function<void (const ValueTree&, const Identifier&)> onValueTreePropertyChanged;
    std::function<void (const ValueTree&, const ValueTree&)> onValueTreeChildAdded;
    std::function<void (const ValueTree&, const ValueTree&, int)> onValueTreeChildRemoved;
    std::function<void (const ValueTree&, int, int)> onValueTreeChildOrderChanged;
    std::function<void (const ValueTree&)> onValueTreeParentChanged;
    std::function<void (const ValueTree&)> onValueTreeRedirected;
    
private:
    void valueTreePropertyChanged (ValueTree& v, const Identifier& i) override
    {
        ValueTree vc = v;
        Identifier ic = i;
        WeakReference<AsyncLambdaValueTreeListener> weakThis (this);
        MessageManager::getInstance()->callAsync ([this, weakThis, vc, ic]
                                                  {
                                                      if (weakThis != nullptr && onValueTreePropertyChanged)
                                                          onValueTreePropertyChanged (vc, ic);
                                                  });
    }
    
    void valueTreeChildAdded (ValueTree& p, ValueTree& c) override
    {
        ValueTree pc = p;
        ValueTree cc = c;
        WeakReference<AsyncLambdaValueTreeListener> weakThis (this);
        MessageManager::getInstance()->callAsync ([this, weakThis, pc, cc]
                                                  {
                                                      if (weakThis != nullptr && onValueTreeChildAdded)
                                                          onValueTreeChildAdded (pc, cc);
                                                  });
    }
    
    void valueTreeChildRemoved (ValueTree& p, ValueTree& c, int n) override
    {
        ValueTree pc = p;
        ValueTree cc = c;
        WeakReference<AsyncLambdaValueTreeListener> weakThis (this);
        MessageManager::getInstance()->callAsync ([this, weakThis, pc, cc, n]
                                                  {
                                                      if (weakThis != nullptr && onValueTreeChildRemoved)
                                                          onValueTreeChildRemoved (pc, cc, n);
                                                  });
    }
    
    void valueTreeChildOrderChanged (ValueTree& v, int a, int b) override
    {
        ValueTree vc = v;
        WeakReference<AsyncLambdaValueTreeListener> weakThis (this);
        MessageManager::getInstance()->callAsync ([this, weakThis, vc, a, b]
                                                  {
                                                      if (weakThis != nullptr && onValueTreeChildOrderChanged)
                                                          onValueTreeChildOrderChanged (vc, a, b);
                                                  });
    }
    
    void valueTreeParentChanged (ValueTree& v) override
    {
        ValueTree vc = v;
        WeakReference<AsyncLambdaValueTreeListener> weakThis (this);
        MessageManager::getInstance()->callAsync ([this, weakThis, vc]
                                                  {
                                                      if (weakThis != nullptr && onValueTreeParentChanged)
                                                          onValueTreeParentChanged (vc);
                                                  });
    }
    
    void valueTreeRedirected (ValueTree& v) override
    {
        ValueTree vc = v;
        WeakReference<AsyncLambdaValueTreeListener> weakThis (this);
        MessageManager::getInstance()->callAsync ([this, weakThis, vc]
                                                  {
                                                      if (weakThis != nullptr && onValueTreeRedirected)
                                                          onValueTreeRedirected (vc);
                                                  });
    }
    
    ValueTree& vt;
    
    JUCE_DECLARE_WEAK_REFERENCEABLE (AsyncLambdaValueTreeListener)
};
