

class ParserCache : public juce::DeletedAtShutdown
{
public:
    ~ParserCache() override
    {
        clearSingletonInstance();
    }

    Parser* findParser (const juce::String& equation)
    {
        auto itr = parserCache.find (equation);
        if (itr != parserCache.end())
            return itr->second.get();

        auto p = new Parser (equation);

        parserCache[equation] = std::unique_ptr<Parser> (p);

        return p;
    }

juce_DeclareSingleton (ParserCache, false)

private:
    std::unordered_map<juce::String, std::unique_ptr<Parser>> parserCache;
};

juce_ImplementSingleton (ParserCache)

static juce::String getParentPath (juce::String in)
{
    const auto lastSlash = in.lastIndexOfChar ('/');

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
    const auto firstChar = *(path.text);

    return firstChar == '/' || firstChar == '~';
}

static juce::String getChildPath (juce::String fullPath, juce::StringRef relativePath)
{
    auto r = relativePath.text;

    if (isAbsolutePath (r))
        return juce::String (r);

    auto path = fullPath;
    auto separatorChar = juce::juce_wchar ('/');

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
        else if (secondChar == separatorChar || secondChar == 0) // remove "./"
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

            for (auto i = n1; i <= n2; ++i)
                out.add (prefix + juce::String (i) + postfix);
        }
        else
        {
            out.add (str);
        }
    }

    return out;
}

void ConstantsStack::set (const juce::String& name, double value)
{
    constants.push_back ({ name, value });
}

std::optional<double> ConstantsStack::get (const juce::String& name)
{
    for (auto i = int (constants.size()); --i >= 0;)
        if (constants[i].first == name)
            return constants[i].second;

    return {};
}

Parser::Parser (const juce::String equation)
{
    try
    {
        parser.addFunction ("getX", [this] (int, const juce::String& id)
                            { if (auto c = getComp (id)) return c->getX ();                      return 0; });
        parser.addFunction ("getY", [this] (int, const juce::String& id)
                            { if (auto c = getComp (id)) return c->getY ();                      return 0; });
        parser.addFunction ("getW", [this] (int, const juce::String& id)
                            { if (auto c = getComp (id)) return c->getWidth ();                  return 0; });
        parser.addFunction ("getH", [this] (int, const juce::String& id)
                            { if (auto c = getComp (id)) return c->getHeight ();                 return 0; });
        parser.addFunction ("getR", [this] (int, const juce::String& id)
                            { if (auto c = getComp (id)) return c->getRight ();                  return 0; });
        parser.addFunction ("getB", [this] (int, const juce::String& id)
                            { if (auto c = getComp (id)) return c->getBottom ();                 return 0; });
        parser.addFunction ("getCX", [this] (int, const juce::String& id)
                            { if (auto c = getComp (id)) return c->getBounds().getCentreX ();    return 0; });
        parser.addFunction ("getCY", [this] (int, const juce::String& id)
                            { if (auto c = getComp (id)) return c->getBounds().getCentreY ();    return 0; });

        parser.defineNameChars ("0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.");
    }
    catch (...)
    {
        DBG ("Failed to set up parser");
    }

    parser.setEquation (equation);

    parser.clearVariables();
    parser.setVarFactory ([&] (const char* name) -> double*
    {
        jassert (constants != nullptr);

        auto val = constants->get (name);
        if (val.has_value () && varStackPtr < std::size (varStack) - 1)
        {
            usedConstants.push_back (name);
            varStack[ varStackPtr ] = *val;
            return &varStack[ varStackPtr++ ];
        }

        jassertfalse;
        return nullptr;
    });
}

double Parser::evaluate()
{
    for (auto idx = 0; const auto& c : usedConstants)
    {
        auto val = constants->get (c);
        if (val.has_value())
            varStack[idx] = *val;

        idx++;
    }

    return parser.evaluate();
}

juce::Component* Parser::getComp (const juce::String& cid)
{
    if (auto foundComp = compMap->findComponent (cid))
        return foundComp;

    if (curComponent != nullptr)
    {
        auto cwd = compMap->getComponentPath (*curComponent);
        auto fullpath = getParentPath (cwd);
        fullpath = getChildPath (fullpath, cid);

        if (auto foundComp = compMap->findComponent (fullpath))
            return foundComp;
    }

    DBG ("Unknown component: " + cid);
    jassertfalse;
    return nullptr;
}

LayoutSupport::LayoutSupport (juce::Component& parent_, std::function<std::pair<juce::Component*, bool> (const juce::String&)> factory_)
    : parent (parent_), compMap (parent_), componentFactory (factory_)
{
#if JUCE_MAC
    constants.set ("mac", 1.0);
    constants.set ("win", 0.0);
    constants.set ("linux", 0.0);
#elif JUCE_WINDOWS
    constants.set ("mac", 0.0);
    constants.set ("win", 1.0);
    constants.set ("linux", 0.0);
#elif JUCE_LINUX
    constants.set ("mac", 0.0);
    constants.set ("win", 0.0);
    constants.set ("linux", 1.0);
#endif

    parent.getProperties().set ("usesLayoutSupport", true);

    if (refCount == 0)
    {
        watcher = std::make_unique<FileSystemWatcher>();
        watcher->coalesceEvents (100);
    }

    refCount++;

    watcher->addListener (this);
}

LayoutSupport::~LayoutSupport()
{
    watcher->removeListener (this);

    refCount--;
    if (refCount == 0)
        watcher = nullptr;
}

bool LayoutSupport::isLayoutSet()
{
    return layoutSet;
}

void LayoutSupport::loadConstants (juce::var& j)
{
    //
    // Find all the base level constants
    //
    if (j.hasProperty ("constants"))
    {
        for (auto& c : getAllProperties (j["constants"]))
        {
            for (auto& itm : getPropertySet (c))
            {
                auto val = itm.value;

                if (val.isObject())
                {
                    for (auto& it : getPropertySet (val))
                        constants.set ((itm.key + "." + it.key).toStdString(), parse (it.value, 0));
                }
                else
                {
                    constants.set (itm.key.toStdString(), parse (val, 0));
                }
            }
        }
    }
}

void LayoutSupport::loadMacros (juce::var& j)
{
    //
    // Find all the base level constants
    //
    if (j.hasProperty ("macros"))
    {
        for (auto& c : getAllProperties (j["macros"]))
        {
            auto id = c["id"].toString();
            macros[id] = c;
        }
    }
}

juce::String LayoutSupport::findContent ([[maybe_unused]] const juce::String& name)
{
#if __has_include("BinaryData.h")
    for (auto i = 0; i < BinaryData::namedResourceListSize; ++i)
    {
        if (name == juce::String (BinaryData::originalFilenames[i]))
        {
            auto sz = 0;
            auto raw = BinaryData::getNamedResource (BinaryData::namedResourceList[i], sz);

            return juce::String (raw);
        }
    }
#endif

    DBG (name + " not found");

    jassertfalse;
    return juce::String();
}

void LayoutSupport::setLayout (const juce::Array<juce::File>& files)
{
    juce::Array<JsonFile> contents;

    for (auto& f : files)
    {
        layoutFiles.add (f);
        contents.add ({ f.getFileName(), f.loadFileAsString() });

        watcher->addFolder (f.getParentDirectory());
    }

    setLayoutInternal (contents);
}

void LayoutSupport::setLayout (const juce::StringArray& resourceNames)
{
    juce::Array<JsonFile> contents;

    for (auto f : resourceNames)
        contents.add ({ f, findContent (f) });

    setLayoutInternal (contents);
}

void LayoutSupport::setLayout (const juce::String& rawJson)
{
    juce::Array<JsonFile> files;
    files.add ({ "rawData", rawJson });

    setLayoutInternal (files);
}

void LayoutSupport::setLayoutInternal (const juce::Array<JsonFile>& files)
{
    layoutSet = true;

    //
    // Find all the base level constants
    //
    compMap.createMap ([] (juce::Component& c)
                       { return ! c.getProperties().getWithDefault ("usesLayoutSupport", false); });

    ConstantsStack::ScopedSave ssp (constants);

    for (const auto& file : files)
    {
        try
        {
            juce::var j;
            auto res = juce::JSON::parse (removeJsonComments (file.contents), j);
            if (! res.wasOk())
                throw std::runtime_error (res.getErrorMessage().toStdString());

            //
            // Position all components
            //
            constants.set ("parX", parent.getX());
            constants.set ("parY", parent.getY());
            constants.set ("parCX", parent.getBounds().getCentreX());
            constants.set ("parCY", parent.getBounds().getCentreY());
            constants.set ("parW", parent.getWidth());
            constants.set ("parH", parent.getHeight());
            constants.set ("parW2", parent.getWidth() / 2);
            constants.set ("parH2", parent.getHeight() / 2);
            constants.set ("parR", parent.getRight());
            constants.set ("parB", parent.getBottom());

            loadConstants (j);
            loadMacros (j);

            if (j.hasProperty ("w") && j.hasProperty ("h"))
            {
                auto w = parse (j["w"], 0);
                auto h = parse (j["h"], 0);

                parent.setSize (w, h);
            }

            if (j.hasProperty ("components"))
            {
                auto components = j["components"];
                setComponentsLayout ({}, components);
            }
        }
        catch (const std::exception& e)
        {
            auto err = juce::String::fromUTF8 (e.what());

            DBG ("json error: " << file.name << " " << err);

            jassertfalse;
        }
    }

    if (layoutChanged)
        layoutChanged();

    compMap.clearMap();
}

void LayoutSupport::setConstant (const juce::String& name, int value)
{
    constants.set (name, value);
}

void LayoutSupport::setComponentsLayout (const juce::String& currentPath,
                                         const juce::var& components)
{
    juce::Component* prevComponent = nullptr;

    for (auto i = 0; const auto& component : getAllProperties (components))
    {
        //
        // Parse component specific constants
        //
        ConstantsStack::ScopedSave ssp (constants);

        constants.set ("i", i);

        auto updatePrevious = [&]
        {
            //
            // Previous component constants
            //
            constants.set ("prevX", prevComponent != nullptr ? prevComponent->getX() : 0);
            constants.set ("prevY", prevComponent != nullptr ? prevComponent->getY() : 0);
            constants.set ("prevCX", prevComponent != nullptr ? prevComponent->getBounds().getCentreX() : 0);
            constants.set ("prevCY", prevComponent != nullptr ? prevComponent->getBounds().getCentreY() : 0);
            constants.set ("prevW", prevComponent != nullptr ? prevComponent->getWidth() : 0);
            constants.set ("prevH", prevComponent != nullptr ? prevComponent->getHeight() : 0);
            constants.set ("prevR", prevComponent != nullptr ? prevComponent->getRight() : 0);
            constants.set ("prevB", prevComponent != nullptr ? prevComponent->getBottom() : 0);
        };

        updatePrevious();

        if (component.hasProperty ("constants"))
            for (auto& c : getAllProperties (component["constants"]))
                for (auto& itm : getPropertySet (c))
                    constants.set (itm.key.toStdString(), parse (itm.value, 0));

        //
        // Ignore some commands
        //
        if (component.hasProperty ("if"))
            if (parse (component["if"], 0) == 0)
                continue;

        //
        // If there is an id, position this component. If no id, then
        // its bounds are set in C++.
        //
        if (component.hasProperty ("id"))
        {
            //
            // Size and position component
            //

            juce::String idString = component["id"].toString();
            juce::StringArray ids;

            if (idString.contains (","))
                ids = expandTokens (juce::StringArray::fromTokens (idString, ",", ""));
            else
                ids = expandTokens ({ idString });

            for (auto idIdx = 0; auto& id : ids)
            {
                constants.set ("idIdx", idIdx);
                //
                // Create component on demand
                //
                if (component.hasProperty ("factory") && compMap.findComponent (currentPath + "/" + id) == nullptr)
                {
                    if (componentFactory)
                    {
                        if (auto par = compMap.findComponent (currentPath))
                        {
                            if (auto [comp, _toBack] = componentFactory (component["factory"]); comp != nullptr)
                            {
                                createdComponents.add (comp);
                                par->addAndMakeVisible (comp);

                                comp->setName (id);
                                if (_toBack)
                                    comp->toBack();

                                compMap.addComponent (*comp);
                            }
                        }
                    }
                    else
                    {
                        // you are trying to use a componentFactory, but you didn't provide one
                        jassertfalse;
                    }
                }

                if (component.hasProperty ("macro"))
                    prevComponent = setPosition (currentPath, id, idIdx++, expandMacro (component));
                else
                    prevComponent = setPosition (currentPath, id, idIdx++, component);

                if (prevComponent != nullptr)
                {
                    //
                    // Handle tab order
                    //
                    if (component.hasProperty ("order"))
                        prevComponent->setExplicitFocusOrder (int (component["order"]) + 1);
                    else
                        prevComponent->setExplicitFocusOrder (i + 1);

                    if (component.hasProperty ("parentOrder"))
                        if (auto p = curComponent->getParentComponent())
                            p->setExplicitFocusOrder (int (component["parentOrder"]) + 1);
                }

                if (ids.size() > 0)
                    updatePrevious();

                ++i;
            }
        }
        else if (component.hasProperty ("grid"))
        {
            if (auto foundComp = compMap.findComponent (currentPath))
                if (! foundComp->getBounds().isEmpty())
                    setGridPositions (currentPath, component);
        }
        else
        {
            //
            // Handle sub components
            //
            if (component.hasProperty ("components"))
                setComponentsLayout (currentPath, component["components"]);
        }
    }
}

LayoutSupport::Bounds LayoutSupport::getBounds (int idIdx, const juce::var& component)
{
    //
    // Set position and size
    //
    auto hasX = component.hasProperty ("r") || component.hasProperty ("x") || component.hasProperty ("cx");
    auto hasY = component.hasProperty ("b") || component.hasProperty ("y") || component.hasProperty ("cy");
    auto hasPosition = (hasX && hasY) || component.hasProperty ("xy");
    auto hasW = component.hasProperty ("w") || component.hasProperty ("size") || (component.hasProperty ("r") && component.hasProperty ("x"));
    auto hasH = component.hasProperty ("h") || component.hasProperty ("size") || (component.hasProperty ("y") && component.hasProperty ("b"));
    auto hasSize = hasW && hasH;

    auto x = 0;
    auto y = 0;
    auto w = 0;
    auto h = 0;

    if (component.hasProperty ("r") && component.hasProperty ("x"))
        w = parse (component["r"], idIdx) - parse (component["x"], idIdx);

    if (component.hasProperty ("y") && component.hasProperty ("b"))
        h = parse (component["b"], idIdx) - parse (component["y"], idIdx);

    if (component.hasProperty ("xd"))
    {
        int delta;
        auto prefix = juce::String ("prev");
        auto reference = juce::String();

        if (component["xd"].isString() && component["xd"].toString().contains (","))
        {
            auto tokens = juce::StringArray::fromTokens (component["xd"].toString(), ",", "");
            tokens.trim();

            delta = parse (tokens[0], idIdx);

            reference = "('" + tokens[1] + "')";
            prefix = "get";
        }
        else
        {
            delta = parse (component["xd"], idIdx);
        }

        w = parse (prefix + "W" + reference, idIdx);
        h = parse (prefix + "H" + reference, idIdx);
        y = parse (prefix + "Y" + reference, idIdx);

        if (delta < 0)
            x = parse (prefix + "X" + reference, idIdx) - w + delta;
        else
            x = parse (prefix + "R" + reference, idIdx) + delta;

        hasPosition = true;
        hasSize = true;
    }

    if (component.hasProperty ("cxd"))
    {
        int delta;
        auto prefix = juce::String ("prev");
        auto reference = juce::String();

        if (component["cxd"].isString() && component["cxd"].toString().contains (","))
        {
            auto tokens = juce::StringArray::fromTokens (component["cxd"].toString(), ",", "");
            tokens.trim();

            delta = parse (tokens[0], idIdx);

            reference = "('" + tokens[1] + "')";
            prefix = "get";
        }
        else
        {
            delta = parse (component["cxd"], idIdx);
        }

        w = parse (prefix + "W" + reference, idIdx);
        h = parse (prefix + "H" + reference, idIdx);
        y = parse (prefix + "Y" + reference, idIdx);

        if (delta < 0)
            x = parse (prefix + "CX" + reference, idIdx) - delta - w / 2;
        else
            x = parse (prefix + "CX" + reference, idIdx) + delta - w / 2;

        hasPosition = true;
        hasSize = true;
    }

    if (component.hasProperty ("yd"))
    {
        int delta;
        auto prefix = juce::String ("prev");
        auto reference = juce::String();

        if (component["yd"].isString() && component["yd"].toString().contains (","))
        {
            auto tokens = juce::StringArray::fromTokens (component["yd"].toString(), ",", "");
            tokens.trim();

            delta = parse (tokens[0], idIdx);

            reference = "('" + tokens[1] + "')";
            prefix = "get";
        }
        else
        {
            delta = parse (component["yd"], idIdx);
        }

        w = parse (prefix + "W" + reference, idIdx);
        h = parse (prefix + "H" + reference, idIdx);
        x = parse (prefix + "X" + reference, idIdx);

        if (delta < 0)
            y = parse (prefix + "Y" + reference, idIdx) - h + delta;
        else
            y = parse (prefix + "B" + reference, idIdx) + delta;

        hasPosition = true;
        hasSize = true;
    }

    if (component.hasProperty ("cyd"))
    {
        int delta;
        auto prefix = juce::String ("prev");
        auto reference = juce::String();

        if (component["cyd"].isString() && component["cyd"].toString().contains (","))
        {
            auto tokens = juce::StringArray::fromTokens (component["cyd"].toString(), ",", "");
            tokens.trim();

            delta = parse (tokens[0], idIdx);

            reference = "('" + tokens[1] + "')";
            prefix = "get";
        }
        else
        {
            delta = parse (component["cyd"], idIdx);
        }

        w = parse (prefix + "W" + reference, idIdx);
        h = parse (prefix + "H" + reference, idIdx);
        x = parse (prefix + "X" + reference, idIdx);

        if (delta < 0)
            y = parse (prefix + "CY" + reference, idIdx) - delta - h / 2;
        else
            y = parse (prefix + "CY" + reference, idIdx) + delta - h / 2;

        hasPosition = true;
        hasSize = true;
    }

    if (component.hasProperty ("x"))
        x = parse (component["x"], idIdx);
    if (component.hasProperty ("y"))
        y = parse (component["y"], idIdx);

    if (component.hasProperty ("xy"))
    {
        auto str = component["xy"].toString();
        auto tokens = juce::StringArray::fromTokens (str, ",", "");

        x = parse (tokens[0], idIdx);
        y = parse (tokens[1], idIdx);
    }

    if (component.hasProperty ("inset") && component["inset"].isString())
    {
        if (auto c = getComp (component["inset"].toString()))
        {
            auto r = c->getBounds().reduced (component.hasProperty ("border") ? int (component["border"]) : 0);
            x = r.getX();
            y = r.getY();
            w = r.getWidth();
            h = r.getHeight();

            hasPosition = true;
            hasSize = true;
        }
    }
    if (component.hasProperty ("bounds"))
    {
        if (component["bounds"] == "parent")
        {
            x = 0;
            y = 0;
            w = juce::roundToInt (*constants.get ("parW"));
            h = juce::roundToInt (*constants.get ("parH"));

            hasPosition = true;
            hasSize = true;
        }
        else if (component["bounds"] == "prev")
        {
            x = parse ("prevX", idIdx);
            y = parse ("prevY", idIdx);
            w = parse ("prevW", idIdx);
            h = parse ("prevH", idIdx);

            hasPosition = true;
            hasSize = true;
        }
        else if (auto reduced = component["bounds"].toString(); reduced.contains ("reduced"))
        {
            auto tokens = juce::StringArray::fromTokens (reduced, ",", "");

            x = 0;
            y = 0;
            w = juce::roundToInt (*constants.get ("parW"));
            h = juce::roundToInt (*constants.get ("parH"));

            if (tokens.size() == 2)
            {
                x += tokens[1].getIntValue();
                y += tokens[1].getIntValue();
                w -= tokens[1].getIntValue() * 2;
                h -= tokens[1].getIntValue() * 2;
            }
            else if (tokens.size() == 3)
            {
                x += tokens[1].getIntValue();
                y += tokens[2].getIntValue();
                w -= tokens[1].getIntValue() * 2;
                h -= tokens[2].getIntValue() * 2;
            }
            else if (tokens.size() == 5)
            {
                x += tokens[1].getIntValue();
                y += tokens[2].getIntValue();
                w -= tokens[1].getIntValue() + tokens[3].getIntValue();
                h -= tokens[2].getIntValue() + tokens[4].getIntValue();
            }

            hasPosition = true;
            hasSize = true;
        }
        else if (auto str = component["bounds"].toString(); str.contains (","))
        {
            auto tokens = juce::StringArray::fromTokens (str, ",", "");

            if (tokens.size() == 4)
            {
                x = parse (tokens[0], idIdx);
                y = parse (tokens[1], idIdx);
                w = parse (tokens[2], idIdx);
                h = parse (tokens[3], idIdx);

                hasPosition = true;
                hasSize = true;
            }
            else if (tokens.size() == 2)
            {
                x = 0;
                y = 0;
                w = parse (tokens[0], idIdx);
                h = parse (tokens[1], idIdx);

                hasPosition = false;
                hasSize = true;
            }
        }
        else
        {
            jassertfalse;
        }
    }
    else if (component.hasProperty ("size"))
    {
        auto sizeId = component["size"].toString();

        w = parse (sizeId + ".w", 0);
        h = parse (sizeId + ".h", 0);
    }

    if (component.hasProperty ("w"))
        w = parse (component["w"], idIdx);
    if (component.hasProperty ("h"))
        h = parse (component["h"], idIdx);
    if (hasSize && component.hasProperty ("cx"))
        x = parse (component["cx"], idIdx) - w / 2;
    if (hasSize && component.hasProperty ("cy"))
        y = parse (component["cy"], idIdx) - h / 2;
    if (hasSize && component.hasProperty ("r"))
        x = parse (component["r"], idIdx) - w;
    if (hasSize && component.hasProperty ("b"))
        y = parse (component["b"], idIdx) - h;

    LayoutSupport::Bounds res;
    res.x = x;
    res.y = y;
    res.h = h;
    res.w = w;
    res.hasSize = hasSize;
    res.hasPosition = hasPosition;

    return res;
}

juce::var LayoutSupport::expandMacro (const juce::var& component_)
{
    auto component = component_;

    juce::String str = component["macro"].toString();

    auto function = str.upToFirstOccurrenceOf ("(", false, false).trim();

    auto params = juce::StringArray::fromTokens (str.fromFirstOccurrenceOf ("(", false, false).upToFirstOccurrenceOf (")", false, false).trim(), ",", "");
    params.trim();
    params.removeEmptyStrings();

    auto itr = macros.find (function);

    if (itr == macros.end())
        return component;

    for (auto& itm : getPropertySet (itr->second))
    {
        auto key = itm.key;
        auto val = itm.value;

        if (key == "id")
            continue;

        if (val.isString())
        {
            juce::String txt = val.toString();

            for (int i = params.size(); --i >= 0;)
                txt = txt.replace (juce::String::formatted ("P%d", i + 1), params[i]);

            setProperty (component, key, txt);
        }
        else
        {
            setProperty (component, key, val);
        }
    }

    return component;
}

juce::Component* LayoutSupport::setPosition (const juce::String& currentPath,
                                             const juce::String& id,
                                             int idIdx,
                                             const juce::var& component)
{
    const auto path = currentPath + "/" + id;

    if (auto foundComp = compMap.findComponent (path))
    {
        juce::ScopedValueSetter<juce::Component*> svs (curComponent, foundComp);

        //
        // Set accessibility title
        //
        if (component.hasProperty ("title"))
            curComponent->setTitle (component["title"].toString());

        if (curComponent->getTitle().isEmpty())
        {
            auto title = curComponent->getName();
            if (title.endsWith ("L"))
                title = title.dropLastCharacters (1);

            curComponent->setTitle (title);
        }

        //
        // Tooltip
        //
        if (component.hasProperty ("tip"))
            if (auto ttc = dynamic_cast<juce::SettableTooltipClient*> (curComponent))
                ttc->setTooltip (component["tip"].toString());

        //
        // Handle properties
        //
        if (component.hasProperty ("properties"))
        {
            auto p = component["properties"];
            for (auto& itm : getPropertySet (p))
            {
                auto name = itm.key;
                auto value = itm.value;

                jassert (name.isNotEmpty());

                if (value.isString())
                    curComponent->getProperties().set (name, value.toString());
                else if (value.isDouble())
                    curComponent->getProperties().set (name, double (value));
                else if (value.isInt())
                    curComponent->getProperties().set (name, int (value));
                else if (value.isBool())
                    curComponent->getProperties().set (name, bool (value));
            }
            curComponent->resized();
            curComponent->repaint();
        }

        auto hasVisible = component.hasProperty ("visible");
        auto bounds = getBounds (idIdx, component);

        if (bounds.hasPosition)
            curComponent->setTopLeftPosition (bounds.x, bounds.y);
        if (bounds.hasSize)
            curComponent->setSize (bounds.w, bounds.h);

        if (component.hasProperty ("zorder"))
        {
            auto p = curComponent->getParentComponent();
            int zorder = component["zorder"];
            if (p->getChildComponent (zorder) != curComponent)
            {
                p->removeChildComponent (curComponent);
                p->addChildComponent (curComponent, zorder);
            }
        }

        if (hasVisible && component["visible"].isBool())
            curComponent->setVisible (bool (component["visible"]));
        else if (hasVisible && component["visible"].isString())
            curComponent->setVisible (parse (component["visible"], idIdx) != 0);

        //
        // Handle sub components
        //
        if (component.hasProperty ("components"))
        {
            ConstantsStack::ScopedSave ssp (constants);

            constants.set ("parX", foundComp->getX());
            constants.set ("parY", foundComp->getY());
            constants.set ("parCX", foundComp->getBounds().getCentreX());
            constants.set ("parCY", foundComp->getBounds().getCentreY());
            constants.set ("parW", foundComp->getWidth());
            constants.set ("parH", foundComp->getHeight());
            constants.set ("parW2", foundComp->getWidth() / 2);
            constants.set ("parH2", foundComp->getHeight() / 2);
            constants.set ("parR", foundComp->getRight());
            constants.set ("parB", foundComp->getBottom());

            setComponentsLayout (path, component["components"]);
        }

        return curComponent;
    }
    else
    {
        bool optional = false;
        if (component.hasProperty ("optional"))
            optional = component["optional"];

        if (! optional)
        {
            DBG ("Can't find: " + path);

#if 0
            for (auto s : compMap.dump ())
                DBG ("   " + s);
#endif

            jassertfalse;
        }

        return nullptr;
    }
}

void LayoutSupport::setGridPositions (const juce::String& currentPath, const juce::var& component)
{
    //
    // get ids
    //
    juce::String idString = component["grid"].toString();
    juce::StringArray ids;

    if (idString.contains (","))
        ids = expandTokens (juce::StringArray::fromTokens (idString, ",", ""));
    else
        ids = expandTokens ({ idString });

    ids.trim ();

    auto bounds = getBounds (0, component);

    jassert (bounds.hasPosition && bounds.hasSize);
    if (! bounds.hasPosition || ! bounds.hasSize)
        return;

    juce::Rectangle<int> rc (bounds.x, bounds.y, bounds.w, bounds.h);

    using juce::operator""_px;
    using juce::operator""_fr;

    juce::Grid grid;

    auto cols = parse (getPropertyWithDefault (component, "cols", 1), 0);
    auto rows = parse (getPropertyWithDefault (component, "rows", 1), 0);

    grid.columnGap  = juce::Grid::Px (parse (getPropertyWithDefault (component, "colGap", 0), 0));
    grid.rowGap     = juce::Grid::Px (parse (getPropertyWithDefault (component, "rowGap", 0), 0));

    auto getTrackInfo = [] (const juce::String& str, int idx)
    {
        auto tokens = juce::StringArray::fromTokens (str, "/", "");
        tokens.trim();
        auto token = tokens[idx + 1];

        if (token.endsWith ("px"))
            return juce::Grid::TrackInfo (juce::Grid::Px (token.retainCharacters ("0123456789").getIntValue()));

        if (token.endsWith ("fr"))
            return juce::Grid::TrackInfo (juce::Grid::Fr (token.retainCharacters ("0123456789").getIntValue()));

        return juce::Grid::TrackInfo (1_fr);
    };

    for (auto i = 0; i < cols; i++)
        grid.templateColumns.add (getTrackInfo (ids[i], 0));

    for (auto i = 0; i < rows; i++)
        grid.templateRows.add (getTrackInfo (ids[cols * i], 1));

    for (auto id : ids)
    {
        const auto path = juce::String (currentPath) + "/" + id.upToFirstOccurrenceOf ("/", false, false).trim();

        if (auto foundComp = compMap.findComponent (path))
            grid.items.add (juce::GridItem (foundComp));
        else
            grid.items.add (juce::GridItem());
    }

    grid.performLayout (rc);
}

void LayoutSupport::fileChanged (const juce::File& f, gin::FileSystemWatcher::FileSystemEvent)
{
    if (f.existsAsFile() && layoutFiles.contains (f))
    {
        juce::Array<JsonFile> contents;
        for (auto layoutFile : layoutFiles)
        {
            auto text = layoutFile.loadFileAsString();
            if (text.isEmpty())
                return;

            contents.add ({ layoutFile.getFileName(), text });
        }

        setLayoutInternal (contents);
    }
}

int LayoutSupport::parse (const juce::var& equation, int equationIndex)
{
    try
    {
        if (equation.isString())
        {
            auto equationText =  equation.toString().replace ("'", "\"");
            if (equationText.contains (","))
            {
                auto tokens = juce::StringArray::fromTokens (equationText, ",", "");
                equationText = tokens[std::min (equationIndex, tokens.size() - 1)];
            }

            equationText = equationText.trim();

            //
            // Early out if just numbers
            //
            if (equationText.containsOnly ("0123456789"))
            {
                return equationText.getIntValue();
            }

            //
            // If the equation is just a simple variable, we don't need to parse, just return value
            //
            if (! equationText.containsAnyOf ("+-/*(% "))
            {
                auto val = constants.get (equationText.toRawUTF8());
                if (val.has_value())
                    return juce::roundToInt (*val);
            }

            //
            // Otherwise, evaluate expression
            //
            auto parser = ParserCache::getInstance()->findParser (equationText);

            parser->compMap = &compMap;
            parser->curComponent = curComponent;
            parser->constants = &constants;

            return juce::roundToInt (parser->evaluate());
        }

        return equation;
    }
    catch (...)
    {
        DBG ("Failed to parse equation");
        return 0;
    }
}

juce::Component* LayoutSupport::getComp (const juce::String& cid)
{
    if (auto foundComp = compMap.findComponent (cid))
        return foundComp;

    if (curComponent != nullptr)
    {
        auto cwd = compMap.getComponentPath (*curComponent);
        auto fullpath = getParentPath (cwd);
        fullpath = getChildPath (fullpath, cid);

        if (auto foundComp = compMap.findComponent (fullpath))
            return foundComp;
    }

    DBG ("Unknown component: " + cid);
    jassertfalse;
    return nullptr;
}

void LayoutSupport::dumpComponents (juce::Component& root)
{
    auto worthAdding = [] (juce::var v)
    {
        return juce::JSON::toString (v).contains ("id");
    };

    std::function<void (juce::Component&, juce::DynamicObject&)> populate = [&] (juce::Component& c, juce::DynamicObject& o)
    {
        auto name = getComponentID (c);

        if (name != "" || &c == &root)
        {
            auto r = c.getBounds();

            if (name != "")
            {
                o.setProperty ("id", juce::String (name));
                o.setProperty ("x", r.getX());
                o.setProperty ("y", r.getY());
                o.setProperty ("w", r.getWidth());
                o.setProperty ("h", r.getHeight());
            }

            juce::Array<juce::var> childrenArr;

            for (auto child : c.getChildren())
            {
                auto co = new juce::DynamicObject();
                populate (*child, *co);
                juce::var childVar (co);

                if (worthAdding (childVar))
                    childrenArr.add (childVar);
            }

            if (childrenArr.size() > 0)
                o.setProperty ("components", childrenArr);
        }
        else
        {
            for (auto child : c.getChildren())
                populate (*child, o);
        }
    };

    auto obj = new juce::DynamicObject();
    populate (root, *obj);

    auto json = juce::JSON::toString (juce::var (obj), true);

    DBG (json);
    juce::SystemClipboard::copyTextToClipboard (json);
}
