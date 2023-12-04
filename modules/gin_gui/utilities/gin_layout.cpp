
static juce::String getParentPath (juce::String in)
{
    auto lastSlash = in.lastIndexOfChar ('/');

    if (lastSlash > 0)
        return in.substring (0, lastSlash);

    if (lastSlash == 0)
        return "/";

    return in;
}

static juce::String addTrailingSlash (const juce::String& path)
{
    return path.endsWithChar ('/') ? path : path + '/';
}

static bool isAbsolutePath (juce::StringRef path)
{
    auto firstChar = *(path.text);

    return firstChar == '/' || firstChar == '~';
}

static juce::String getChildPath (juce::String fullPath, juce::StringRef relativePath)
{
    auto r = relativePath.text;

    if (isAbsolutePath (r))
        return juce::String (r);

    auto path = fullPath;
    auto separatorChar = juce::juce_wchar('/');

    while (*r == '.')
    {
        auto lastPos = r;
        auto secondChar = *++r;

        if (secondChar == '.') // remove "../"
        {
            auto thirdChar = *++r;

            if (thirdChar == separatorChar || thirdChar == 0)
            {
                auto lastSlash = path.lastIndexOfChar (separatorChar);

                if (lastSlash >= 0)
                    path = path.substring (0, lastSlash);

                while (*r == separatorChar) // ignore duplicate slashes
                    ++r;
            }
            else
            {
                r = lastPos;
                break;
            }
        }
        else if (secondChar == separatorChar || secondChar == 0)  // remove "./"
        {
            while (*r == separatorChar) // ignore duplicate slashes
                ++r;
        }
        else
        {
            r = lastPos;
            break;
        }
    }

    path = addTrailingSlash (path);
    path.appendCharPointer (r);
    return path;
}

static juce::StringArray expandTokens (juce::StringArray in)
{
    juce::StringArray out;

    for (const auto& str : in)
    {
        if (str.contains ("[") && str.contains ("..") && str.contains ("]"))
        {
            auto prefix = str.upToFirstOccurrenceOf ("[", false, false);
            auto postfix = str.fromLastOccurrenceOf ("]", false, false);

            auto middle = str.substring (prefix.length() + 1, str.length() - postfix.length() - 1);

            auto n1 = middle.upToFirstOccurrenceOf ("..", false, false).getIntValue();
            auto n2 = middle.fromLastOccurrenceOf ("..", false, false).getIntValue();

            for (int i = n1; i <= n2; i++)
                out.add (prefix + juce::String (i) + postfix);
        }
        else
        {
            out.add (str);
        }
    }

    return out;
}

static juce::String getComponentPath (juce::Component& parent, juce::Component& c)
{
    juce::String path;

    auto p = &c;
    while (p != nullptr)
    {
        if (p->getName().isNotEmpty())
            path = "/" + p->getName() + path;

        if (p == &parent)
            break;

        p = p->getParentComponent();
    }

    return path;
}

Layout::Layout (juce::Component& p) : parent (p)
{
    setupParser();
   #if ! JUCE_IOS
    watcher.addListener (this);
   #endif
}

Layout::~Layout()
{
   #if ! JUCE_IOS
    watcher.removeListener (this);
   #endif
}

void Layout::setupParser()
{
    try
    {
        auto getComp = [this] (const juce::String& cid) -> juce::Component*
        {
            if (auto itr = componentMap.find (cid); itr != componentMap.end())
                return itr->second;

            if (curComponent != nullptr)
            {
                auto cwd = getComponentPath (parent, *curComponent);
                auto fullpath = getParentPath (cwd);
                fullpath = getChildPath (fullpath, cid);

                if (auto itr = componentMap.find (fullpath); itr != componentMap.end())
                    return itr->second;
            }

            jassertfalse;
            return nullptr;
        };

        parser.addFunction ("getX", [getComp] (int, const juce::String& id) { if (auto c = getComp (id))    return c->getX();       return 0; });
        parser.addFunction ("getY", [getComp] (int, const juce::String& id) { if (auto c = getComp (id))    return c->getY();       return 0; });
        parser.addFunction ("getW", [getComp] (int, const juce::String& id) { if (auto c = getComp (id))    return c->getWidth();   return 0; });
        parser.addFunction ("getH", [getComp] (int, const juce::String& id) { if (auto c = getComp (id))    return c->getHeight();  return 0; });
        parser.addFunction ("getR", [getComp] (int, const juce::String& id) { if (auto c = getComp (id))    return c->getRight();   return 0; });
        parser.addFunction ("getB", [getComp] (int, const juce::String& id) { if (auto c = getComp (id))    return c->getBottom();  return 0; });

        parser.addFunction ("prevX", [this] (int) { if (auto c = prevComponent)                             return c->getX();       return 0; });
        parser.addFunction ("prevY", [this] (int) { if (auto c = prevComponent)                             return c->getY();       return 0; });
        parser.addFunction ("prevW", [this] (int) { if (auto c = prevComponent)                             return c->getWidth();   return 0; });
        parser.addFunction ("prevH", [this] (int) { if (auto c = prevComponent)                             return c->getHeight();  return 0; });
        parser.addFunction ("prevR", [this] (int) { if (auto c = prevComponent)                             return c->getRight();   return 0; });
        parser.addFunction ("prevB", [this] (int) { if (auto c = prevComponent)                             return c->getBottom();  return 0; });

        parser.addFunction ("parX", [this] (int) { if (auto c = curComponent->getParentComponent())         return c->getX();       return 0; });
        parser.addFunction ("parY", [this] (int) { if (auto c = curComponent->getParentComponent())         return c->getY();       return 0; });
        parser.addFunction ("parW", [this] (int) { if (auto c = curComponent->getParentComponent())         return c->getWidth();   return 0; });
        parser.addFunction ("parH", [this] (int) { if (auto c = curComponent->getParentComponent())         return c->getHeight();  return 0; });
        parser.addFunction ("parR", [this] (int) { if (auto c = curComponent->getParentComponent())         return c->getRight();   return 0; });
        parser.addFunction ("parB", [this] (int) { if (auto c = curComponent->getParentComponent())         return c->getBottom();  return 0; });
    }
    catch (...)
    {
        jassertfalse;
    }
}

void Layout::setLayout (const juce::String& filename, const juce::File& source)
{
    juce::ignoreUnused (filename, source);
   #if __has_include("BinaryData.h")
    for (int i = 0; i < BinaryData::namedResourceListSize; i++)
    {
        if (filename.equalsIgnoreCase (BinaryData::originalFilenames[i]))
        {
           #if JUCE_DEBUG
            if (source.existsAsFile())
            {
                if (auto rawLayout = source.loadFileAsString(); rawLayout.isNotEmpty() && parseLayout (rawLayout))
                {
                    layoutFile = source;
                   #if ! JUCE_IOS
                    watcher.addFolder (source.getParentDirectory());
                   #endif
                    break;
                }
                else
                {
                    DBG("Unable to load layout file. Is your app sandboxed?");
                }
            }
           #endif
            int sz = 0;
            if (auto data = BinaryData::getNamedResource (BinaryData::namedResourceList[i], sz))
                parseLayout (juce::String (data, size_t (sz)));

            break;
        }
    }
   #else
    jassertfalse; // you have no binary data, can't load layout
   #endif
}

bool Layout::parseLayout (const juce::String& content)
{
    juce::var obj;
    auto err = juce::JSON::parse (content, obj);
    if (! err.wasOk() && ! obj.isObject())
    {
        jassertfalse;
        return false;
    }

    componentMap = findAllComponents();
    doComponent ({}, obj);
    componentMap = {};
                    
    return true;
}

void Layout::doComponent (const juce::String& currentPath, const juce::var& component)
{
    if (component.hasProperty ("id"))
    {
        auto idString = component["id"].toString();
        juce::StringArray ids;

        if (idString.contains (","))
            ids = expandTokens (juce::StringArray::fromTokens (idString, ",", ""));
        else
            ids = expandTokens ({idString});

        auto idIdx = 0;
        for (auto& id : ids)
            prevComponent = setBounds (currentPath, id, idIdx++, component);
    }
    else
    {
        if (component.hasProperty ("children"))
        {
            auto arr = component["children"];
            if (arr.isArray())
                for (auto c : *arr.getArray())
                    doComponent (currentPath, c);
        }
    }
}

juce::Component* Layout::setBounds (const juce::String& currentPath, const juce::String& id, int idIdx, const juce::var& component)
{
    auto path = currentPath + "/" + id;

    auto itr = componentMap.find (path);
    if (itr == componentMap.end())
    {
        DBG("Can't find: " + path);
        jassertfalse;
        return nullptr;
    }

    juce::ScopedValueSetter<juce::Component*> svs (curComponent, itr->second);

    std::optional<int> x;
    std::optional<int> y;
    std::optional<int> r;
    std::optional<int> b;
    std::optional<int> w;
    std::optional<int> h;

    if (component.hasProperty ("x"))    x = parse (component["x"], idIdx);
    if (component.hasProperty ("y"))    y = parse (component["y"], idIdx);
    if (component.hasProperty ("r"))    r = parse (component["r"], idIdx);
    if (component.hasProperty ("b"))    b = parse (component["b"], idIdx);

    if (component.hasProperty ("w"))    w = parse (component["w"], idIdx);
    if (component.hasProperty ("h"))    h = parse (component["h"], idIdx);

    if (r.has_value() && x.has_value()) w = *r - *x;
    if (b.has_value() && y.has_value()) h = *b - *y;
    if (r.has_value() && w.has_value()) x = *r - *w;
    if (b.has_value() && h.has_value()) y = *b - *h;

    if (component.hasProperty ("bounds"))
    {
        if (component["bounds"] == "parent")
        {
            x = 0;
            y = 0;
            w = curComponent->getParentComponent()->getWidth();
            h = curComponent->getParentComponent()->getHeight();
        }
        else if (component["bounds"] == "prev")
        {
            x = prevComponent->getX();
            y = prevComponent->getY();
            w = prevComponent->getWidth();
            h = prevComponent->getHeight();
        }
        else
        {
            jassertfalse;
        }
    }

    if (x.has_value() && y.has_value())
        curComponent->setTopLeftPosition (*x, *y);
    if (w.has_value() && h.has_value())
        curComponent->setSize (*w, *h);

    if (component.hasProperty ("children"))
    {
        auto arr = component["children"];
        if (arr.isArray())
            for (auto c : *arr.getArray())
                doComponent (path, c);
    }

    return curComponent;
}

#if ! JUCE_IOS
void Layout::fileChanged (const juce::File f, gin::FileSystemWatcher::FileSystemEvent)
{
    if (f == layoutFile)
        if (auto str = layoutFile.loadFileAsString(); str.isNotEmpty())
            parseLayout (str);
}
#endif

std::map<juce::String, juce::Component*> Layout::findAllComponents()
{
    std::map<juce::String, juce::Component*> components;

    std::function<void (juce::Component&)> findAll = [&] (juce::Component& p)
    {
        for (auto c : p.getChildren())
        {
            if (c->getName().isNotEmpty())
                components[getComponentPath (parent, *c)] = c;

            findAll (*c);
        }
    };

    components[getComponentPath (parent, parent)] = &parent;
    findAll (parent);

    return components;
}

int Layout::parse (const juce::var& equation, int equationIndex)
{
    try
    {
        if (equation.isString())
        {
            auto equationText = equation.toString().replace ("'", "\"");
            if (equationText.contains (","))
            {
                auto tokens = juce::StringArray::fromTokens (equationText, ",", "");
                equationText = tokens[std::min (equationIndex, tokens.size() - 1)];
            }

            parser.setEquation (equationText);

            return juce::roundToInt (parser.evaluate());
        }

        return equation;
    }
    catch (...)
    {
        jassertfalse;
        return 0;
    }
}
