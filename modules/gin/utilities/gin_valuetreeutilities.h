/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** Converts a ValueTree to a JSON string representation.

    Serializes the entire ValueTree structure (including all properties and children)
    into JSON format. This is useful for saving, transmitting, or debugging ValueTree data.

    @param v  The ValueTree to convert
    @returns  A JSON string representing the ValueTree
*/
juce::String valueTreeToJSON (const juce::ValueTree& v);

/** Parses a JSON string and creates a ValueTree from it.

    The JSON must have been created by valueTreeToJSON() or follow the same format.
    This reconstructs the entire ValueTree structure from the JSON representation.

    @param jsonText  The JSON string to parse
    @returns         A ValueTree reconstructed from the JSON
*/
juce::ValueTree valueTreeFromJSON (const juce::String& jsonText);

//==============================================================================
/** Listens to ValueTree changes using lambda functions instead of virtual methods.

    This class provides a more convenient way to listen to ValueTree changes by
    setting std::function callbacks rather than overriding virtual methods. You can
    set only the callbacks you're interested in.

    Example usage:
    @code
    LambdaValueTreeListener listener (myValueTree);
    listener.onChanged = [this] { updateUI(); };
    listener.onValueTreePropertyChanged = [this] (juce::ValueTree& v, const juce::Identifier& prop) {
        DBG ("Property changed: " + prop.toString());
    };
    @endcode

    @see AsyncLambdaValueTreeListener
*/
class LambdaValueTreeListener : public juce::ValueTree::Listener
{
public:
    /** Creates a listener for the specified ValueTree.

        @param v_  The ValueTree to listen to
    */
    LambdaValueTreeListener (juce::ValueTree v_)
        : vt (std::move (v_))
    {
        vt.addListener (this);
    }

    /** Called whenever any change occurs to the ValueTree. */
    std::function<void ()> onChanged;

    /** Called when a property value changes. Parameters: (tree, property identifier) */
    std::function<void (juce::ValueTree&, const juce::Identifier&)> onValueTreePropertyChanged;

    /** Called when a child is added. Parameters: (parent tree, child that was added) */
    std::function<void (juce::ValueTree&, juce::ValueTree&)> onValueTreeChildAdded;

    /** Called when a child is removed. Parameters: (parent tree, child that was removed, index) */
    std::function<void (juce::ValueTree&, juce::ValueTree&, int)> onValueTreeChildRemoved;

    /** Called when children are reordered. Parameters: (tree, old index, new index) */
    std::function<void (juce::ValueTree&, int, int)> onValueTreeChildOrderChanged;

    /** Called when the parent changes. Parameters: (tree whose parent changed) */
    std::function<void (juce::ValueTree&)> onValueTreeParentChanged;

    /** Called when the tree is redirected to a different shared state. Parameters: (tree) */
    std::function<void (juce::ValueTree&)> onValueTreeRedirected;

private:
    void valueTreePropertyChanged (juce::ValueTree& v, const juce::Identifier& i) override
    {
        if (onChanged)
            onChanged();

        if (onValueTreePropertyChanged)
            onValueTreePropertyChanged (v, i);
    }

    void valueTreeChildAdded (juce::ValueTree& p, juce::ValueTree& c) override
    {
        if (onChanged)
            onChanged();

        if (onValueTreeChildAdded)
            onValueTreeChildAdded (p, c);
    }

    void valueTreeChildRemoved (juce::ValueTree& p, juce::ValueTree& c, int n) override
    {
        if (onChanged)
            onChanged();

        if (onValueTreeChildRemoved)
            onValueTreeChildRemoved (p, c, n);
    }

    void valueTreeChildOrderChanged (juce::ValueTree& v, int a, int b) override
    {
        if (onChanged)
            onChanged();

        if (onValueTreeChildOrderChanged)
            onValueTreeChildOrderChanged (v, a, b);
    }

    void valueTreeParentChanged (juce::ValueTree& v) override
    {
        if (onChanged)
            onChanged();

        if (onValueTreeParentChanged)
            onValueTreeParentChanged (v);
    }

    void valueTreeRedirected (juce::ValueTree& v) override
    {
        if (onChanged)
            onChanged();

        if (onValueTreeRedirected)
            onValueTreeRedirected (v);
    }

    juce::ValueTree vt;
};

//==============================================================================
/** Asynchronous ValueTree listener using lambda functions.

    Similar to LambdaValueTreeListener, but all callbacks are executed asynchronously
    on the message thread using juce::MessageManager::callAsync(). This makes it safe
    to update UI components from the callbacks without worrying about thread safety.

    The callbacks receive const references to prevent accidental modification of the
    ValueTree from the async callbacks.

    Example usage:
    @code
    AsyncLambdaValueTreeListener listener (myValueTree);
    listener.onValueTreePropertyChanged = [this] (const juce::ValueTree& v, const juce::Identifier& prop) {
        // This runs on the message thread, safe to update UI
        someComponent.repaint();
    };
    @endcode

    @see LambdaValueTreeListener
*/
class AsyncLambdaValueTreeListener : public juce::ValueTree::Listener
{
public:
    /** Creates an async listener for the specified ValueTree.

        @param v_  The ValueTree to listen to
    */
    AsyncLambdaValueTreeListener (juce::ValueTree& v_)
        : vt (v_)
    {
        vt.addListener (this);
    }

    /** Destructor. Removes this listener from the ValueTree. */
    ~AsyncLambdaValueTreeListener() override
    {
        vt.removeListener (this);
    }

    /** Called asynchronously when a property value changes. Parameters: (tree, property identifier) */
    std::function<void (const juce::ValueTree&, const juce::Identifier&)> onValueTreePropertyChanged;

    /** Called asynchronously when a child is added. Parameters: (parent tree, child that was added) */
    std::function<void (const juce::ValueTree&, const juce::ValueTree&)> onValueTreeChildAdded;

    /** Called asynchronously when a child is removed. Parameters: (parent tree, child that was removed, index) */
    std::function<void (const juce::ValueTree&, const juce::ValueTree&, int)> onValueTreeChildRemoved;

    /** Called asynchronously when children are reordered. Parameters: (tree, old index, new index) */
    std::function<void (const juce::ValueTree&, int, int)> onValueTreeChildOrderChanged;

    /** Called asynchronously when the parent changes. Parameters: (tree whose parent changed) */
    std::function<void (const juce::ValueTree&)> onValueTreeParentChanged;

    /** Called asynchronously when the tree is redirected to a different shared state. Parameters: (tree) */
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
