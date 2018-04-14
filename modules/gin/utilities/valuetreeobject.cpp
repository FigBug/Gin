/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

std::function<ValueTreeObject* (const Identifier&)> ValueTreeObject::factory;

ValueTreeObject::ValueTreeObject (const ValueTree& state_)
  : state (state_)
{
    state.addListener (this);
}

void ValueTreeObject::valueTreePropertyChanged (ValueTree& p, const Identifier& i)
{
    ignoreUnused (p, i);
}

void ValueTreeObject::valueTreeChildAdded (ValueTree& p, ValueTree& c)
{
    if (p == state)
        if (auto* newObj = factory (c.getType()))
            children.insert (p.indexOf (c), newObj);
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
