/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

std::function<ValueTreeObject* (const juce::Identifier&, const juce::ValueTree&)> ValueTreeObject::factory;

ValueTreeObject::ValueTreeObject (const juce::ValueTree& state_)
  : state (state_)
{
    for (auto c : state)
    {
        if (auto* newObj = factory (c.getType(), c))
        {
            newObj->parent = this;
            children.add (newObj);
        }
        else
        {
            jassertfalse; // type missing in factory
        }
    }

    state.addListener (this);
}

void ValueTreeObject::valueTreePropertyChanged (juce::ValueTree& p, const juce::Identifier& i)
{
    ignoreUnused (p, i);
}

void ValueTreeObject::valueTreeChildAdded (juce::ValueTree& p, juce::ValueTree& c)
{
    if (p == state)
    {
        if (auto* newObj = factory (c.getType(), c))
        {
            newObj->parent = this;
            children.insert (p.indexOf (c), newObj);
        }
        else
        {
            jassertfalse; // type missing in factory
        }
    }
}

void ValueTreeObject::valueTreeChildRemoved (juce::ValueTree& p, juce::ValueTree& c, int i)
{
    ignoreUnused (c);

    if (p == state)
        children.remove (i);
}

void ValueTreeObject::valueTreeChildOrderChanged (juce::ValueTree& p, int oldIndex, int newIndex)
{
    if (p == state)
        children.move (oldIndex, newIndex);
}

void ValueTreeObject::valueTreeParentChanged (juce::ValueTree&)
{
}

void ValueTreeObject::valueTreeRedirected (juce::ValueTree&)
{
}
