/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

std::function<ValueTreeObject* (const Identifier&, const ValueTree&)> ValueTreeObject::factory;

ValueTreeObject::ValueTreeObject (const ValueTree& state_)
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

void ValueTreeObject::valueTreePropertyChanged (ValueTree& p, const Identifier& i)
{
    ignoreUnused (p, i);
}

void ValueTreeObject::valueTreeChildAdded (ValueTree& p, ValueTree& c)
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

void ValueTreeObject::valueTreeChildRemoved (ValueTree& p, ValueTree& c, int i)
{
    ignoreUnused (c);

    if (p == state)
        children.remove (i);
}

void ValueTreeObject::valueTreeChildOrderChanged (ValueTree& p, int oldIndex, int newIndex)
{
    if (p == state)
        children.move (oldIndex, newIndex);
}

void ValueTreeObject::valueTreeParentChanged (ValueTree&)
{
}

void ValueTreeObject::valueTreeRedirected (ValueTree&)
{
}
