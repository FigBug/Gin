/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** Maintains an object hierarchy that mirrors a ValueTree structure.

    This base class allows you to create an object-oriented wrapper around a ValueTree,
    where each ValueTree node is represented by a corresponding object. The object
    hierarchy automatically stays in sync with the ValueTree structure.

    To use this class:
    1. Derive your own classes from ValueTreeObject
    2. Implement a factory function to create appropriate objects for each ValueTree type
    3. Changes to the ValueTree will automatically create/destroy matching objects

    @see getState, getChildren, findParentOfType, findChildrenOfClass
*/
class ValueTreeObject : private juce::ValueTree::Listener
{
public:
    /** Creates a ValueTreeObject wrapping the given ValueTree.

        @param state  The ValueTree this object represents
    */
    ValueTreeObject (const juce::ValueTree& state);

    /** Returns the ValueTree that this object represents.

        @returns  Reference to the wrapped ValueTree
    */
    juce::ValueTree& getState() { return state; }

    /** Factory function for creating ValueTreeObject instances.

        Set this to a function that creates the appropriate ValueTreeObject-derived
        class based on the ValueTree's type identifier.
    */
    static std::function<ValueTreeObject* (const juce::Identifier&, const juce::ValueTree&)> factory;

public:
    /** Returns all child objects.

        @returns  Array of child ValueTreeObject instances
    */
    const juce::OwnedArray<ValueTreeObject>& getChildren() const    { return children; }

    /** Searches up the parent chain for an object of the specified type.

        @tparam TargetClass  The class type to search for
        @returns             Pointer to the first parent of the specified type, or nullptr if not found
    */
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

    /** Finds all direct children of a specific class type.

        @tparam TargetClass  The class type to search for
        @returns             Array of pointers to children of the specified type
    */
    template <class TargetClass>
    juce::Array<TargetClass*> findChildrenOfClass() const
    {
        juce::Array<TargetClass*> res;

        for (auto* c : children)
            if (auto* t = dynamic_cast<TargetClass*> (c))
                res.add (t);

        return res;
    }

    /** Counts the number of direct children of a specific class type.

        @tparam TargetClass  The class type to count
        @returns             The number of children of the specified type
    */
    template <class TargetClass>
    int countChildrenOfClass() const
    {
        int count = 0;

        for (auto* c : children)
            if (auto* t = dynamic_cast<TargetClass*> (c))
                count++;

        return count;
    }

    /** Finds a specific child of a class type by index.

        Returns the nth child of the specified type (where n is the idx parameter).

        @tparam TargetClass  The class type to search for
        @param idx           The index of the child to return (0-based, among children of this type)
        @returns             Pointer to the child at the specified index, or nullptr if not found
    */
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
