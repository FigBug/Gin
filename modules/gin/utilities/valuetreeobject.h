/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
class ValueTreeAllListener : public ValueTree::Listener
{
public:
    ~ValueTreeAllListener() {}

    void valueTreePropertyChanged (ValueTree&, const Identifier&) override {}
    void valueTreeChildAdded (ValueTree&, ValueTree&) override {}
    void valueTreeChildRemoved (ValueTree&, ValueTree&, int) override {}
    void valueTreeChildOrderChanged (ValueTree&, int, int) override {}
    void valueTreeParentChanged (ValueTree&) override {}
    void valueTreeRedirected (ValueTree&) override {}
};

//==============================================================================
class LambdaValueTreeListener : public ValueTree::Listener
{
public:
    LambdaValueTreeListener (ValueTree& v_)
        : v (v_)
    {
        v.addListener (this);
    }
    
    ~LambdaValueTreeListener()
    {
        v.removeListener (this);
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
    
    ValueTree& v;
};

//==============================================================================
class AsyncLambdaValueTreeListener : public ValueTree::Listener
{
public:
    AsyncLambdaValueTreeListener (ValueTree& v_)
        : v (v_)
    {
        v.addListener (this);
    }
    
    ~AsyncLambdaValueTreeListener()
    {
        v.removeListener (this);
        
        masterReference.clear();
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
    
    ValueTree& v;
    
    WeakReference<AsyncLambdaValueTreeListener>::Master masterReference;
    friend class WeakReference<AsyncLambdaValueTreeListener>;
};

//==============================================================================
/* Mirrors a ValueTree is Objects */
class ValueTreeObject : private ValueTree::Listener
{
public:
    ValueTreeObject (const ValueTree& state);

    ValueTree& getState() { return state; }

    static std::function<ValueTreeObject* (const Identifier&, const ValueTree&)> factory;

public:
    const OwnedArray<ValueTreeObject>& getChildren() const    { return children; }

    template <class TargetClass>
    TargetClass* findParentOfType() const
    {
        auto* p = parent;
        while (p != nullptr)
        {
            if (auto* res = dynamic_cast<TargetClass*> (parent))
                return res;

            p = p->parent;
        }

        return nullptr;
    }

    template <class TargetClass>
    Array<TargetClass*> findChildrenOfClass() const
    {
        Array<TargetClass*> res;

        for (auto* c : children)
            if (auto* t = dynamic_cast<TargetClass*> (c))
                res.add (t);

        return res;
    }

    template <class TargetClass>
    int countChildrenOfClass() const
    {
        int count = 0;

        for (auto* c : children)
            if (auto* t = dynamic_cast<TargetClass*> (c))
                count++;

        return count;
    }

    template <class TargetClass>
    TargetClass* findChildOfClass (int idx) const
    {
        int count = 0;

        for (auto* c : children)
        {
            if (auto* t = dynamic_cast<TargetClass*> (c))
            {
                if (count == idx)
                    return t;

                count++;
            }
        }

        return nullptr;
    }

private:
    void valueTreePropertyChanged (ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override;
    void valueTreeChildAdded (ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved (ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;
    void valueTreeChildOrderChanged (ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override;
    void valueTreeParentChanged (ValueTree& treeWhoseParentHasChanged) override;
    void valueTreeRedirected (ValueTree& treeWhichHasBeenChanged) override;

private:
    ValueTree state;
    ValueTreeObject* parent = nullptr;

    OwnedArray<ValueTreeObject> children;

    JUCE_LEAK_DETECTOR (ValueTreeObject)
};
