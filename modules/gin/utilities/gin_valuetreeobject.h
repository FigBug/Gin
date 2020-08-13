/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** Mirrors a ValueTree in Objects
 */
class ValueTreeObject : private juce::ValueTree::Listener
{
public:
    ValueTreeObject (const juce::ValueTree& state);

    juce::ValueTree& getState() { return state; }

    static std::function<ValueTreeObject* (const juce::Identifier&, const juce::ValueTree&)> factory;

public:
    const juce::OwnedArray<ValueTreeObject>& getChildren() const    { return children; }

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
    juce::Array<TargetClass*> findChildrenOfClass() const
    {
        juce::Array<TargetClass*> res;

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
    void valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;
    void valueTreeChildAdded (juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved (juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;
    void valueTreeChildOrderChanged (juce::ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override;
    void valueTreeParentChanged (juce::ValueTree& treeWhoseParentHasChanged) override;
    void valueTreeRedirected (juce::ValueTree& treeWhichHasBeenChanged) override;

private:
    juce::ValueTree state;
    ValueTreeObject* parent = nullptr;

    juce::OwnedArray<ValueTreeObject> children;

    JUCE_LEAK_DETECTOR (ValueTreeObject)
};
