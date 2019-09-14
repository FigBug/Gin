/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/* Mirrors a ValueTree in Objects */
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
