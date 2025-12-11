
static juce::String getComponentID (juce::Component& c)
{
    auto name = c.getProperties()["layoutID"].toString();

    if (name.isNotEmpty())
        return name;

    name = c.getComponentID();

    if (name.isNotEmpty())
        return name;

    return c.getName();
}

ComponentMap::ComponentMap (juce::Component& parent_)
    : parent (parent_)
{
}

void ComponentMap::createMap (const std::function<bool (juce::Component&)>& checkChildren)
{
    clearMap();

    std::function<void (juce::Component&)> findAll = [&] (juce::Component& p)
    {
        for (auto c : p.getChildren())
        {
            auto name = getComponentID (*c);
            jassert (name == name.trim()); // don't start / end you component ids with spaces, you'll get confused why your paths aren't working
            if (name != "")
            {
                auto path = getComponentPath (*c);
                componentMap[path] = c;
            }

            //
            // Check children only on demand
            //
            if (checkChildren == nullptr || checkChildren (*c))
                findAll (*c);
        }
    };

    findAll (parent);
}

void ComponentMap::addComponent (juce::Component& c)
{
    if (auto name = getComponentID (c); name != "")
    {
        auto path = getComponentPath (c);
        componentMap[path] = &c;
    }
}

juce::String ComponentMap::getComponentPath (juce::Component& c)
{
    auto path = "/" + getComponentID (c);

    auto p = c.getParentComponent();
    while (p != nullptr)
    {
        if (p == &parent)
            break;

        if (getComponentID (*p) != "")
            path = "/" + getComponentID (*p) + path;

        p = p->getParentComponent();
    }

    return path.toLowerCase();
}

juce::Component* ComponentMap::findComponent (const juce::String& path)
{
    if (path == "")
        return &parent;

    auto key = path.toLowerCase();

    if (componentMap.contains (key))
        return componentMap[key];

    return nullptr;
}

void ComponentMap::clearMap()
{
    componentMap.clear();
}

juce::StringArray ComponentMap::dump()
{
    juce::StringArray res;

    for (auto& itr : componentMap)
        res.add (itr.first);

    res.sort (true);
    return res;
}
