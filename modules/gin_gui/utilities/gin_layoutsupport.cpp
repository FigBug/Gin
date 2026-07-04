
// String constants
static const juce::String kMac          { "mac" };
static const juce::String kWin          { "win" };
static const juce::String kLinux        { "linux" };
static const juce::String kParX         { "parX" };
static const juce::String kParY         { "parY" };
static const juce::String kParCX        { "parCX" };
static const juce::String kParCY        { "parCY" };
static const juce::String kParW         { "parW" };
static const juce::String kParH         { "parH" };
static const juce::String kParW2        { "parW2" };
static const juce::String kParH2        { "parH2" };
static const juce::String kParR         { "parR" };
static const juce::String kParB         { "parB" };
static const juce::String kPrevX        { "prevX" };
static const juce::String kPrevY        { "prevY" };
static const juce::String kPrevCX       { "prevCX" };
static const juce::String kPrevCY       { "prevCY" };
static const juce::String kPrevW        { "prevW" };
static const juce::String kPrevH        { "prevH" };
static const juce::String kPrevR        { "prevR" };
static const juce::String kPrevB        { "prevB" };
static const juce::String kI            { "i" };
static const juce::String kIdIdx        { "idIdx" };
static const juce::String kParent       { "parent" };
static const juce::String kPrev         { "prev" };
static const juce::String kMinX         { "minX" };
static const juce::String kMinY         { "minY" };
static const juce::String kMaxX         { "maxX" };
static const juce::String kMaxY         { "maxY" };

static const juce::Identifier kConstants    { "constants" };
static const juce::Identifier kMacros       { "macros" };
static const juce::Identifier kComponents   { "components" };
static const juce::Identifier kId           { "id" };
static const juce::Identifier kFactory      { "factory" };
static const juce::Identifier kMacro        { "macro" };
static const juce::Identifier kOrder        { "order" };
static const juce::Identifier kParentOrder  { "parentOrder" };
static const juce::Identifier kGrid         { "grid" };
static const juce::Identifier kIf           { "if" };
static const juce::Identifier kTitle        { "title" };
static const juce::Identifier kTip          { "tip" };
static const juce::Identifier kProperties   { "properties" };
static const juce::Identifier kVisible      { "visible" };
static const juce::Identifier kZorder       { "zorder" };
static const juce::Identifier kOptional     { "optional" };
static const juce::Identifier kW            { "w" };
static const juce::Identifier kH            { "h" };
static const juce::Identifier kPW           { "pW" };
static const juce::Identifier kPH           { "pH" };
static const juce::Identifier kX            { "x" };
static const juce::Identifier kY            { "y" };
static const juce::Identifier kR            { "r" };
static const juce::Identifier kB            { "b" };
static const juce::Identifier kCx           { "cx" };
static const juce::Identifier kCy           { "cy" };
static const juce::Identifier kXy           { "xy" };
static const juce::Identifier kXd           { "xd" };
static const juce::Identifier kCxd          { "cxd" };
static const juce::Identifier kYd           { "yd" };
static const juce::Identifier kCyd          { "cyd" };
static const juce::Identifier kSize         { "size" };
static const juce::Identifier kInset        { "inset" };
static const juce::Identifier kBorder       { "border" };
static const juce::Identifier kBounds       { "bounds" };
static const juce::Identifier kReduced      { "reduced" };
static const juce::Identifier kCols         { "cols" };
static const juce::Identifier kRows         { "rows" };
static const juce::Identifier kColGap       { "colGap" };
static const juce::Identifier kRowGap       { "rowGap" };
static const juce::Identifier kUsesLayoutSupport { "usesLayoutSupport" };
static const juce::Identifier kPrefixPrev   { "prev" };
static const juce::Identifier kPrefixGet    { "get" };

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
        if (constants[size_t (i)].first == name)
            return constants[size_t (i)].second;

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

        DBG("var not found: " + juce::String ( name ));
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
    constants.set (kMac, 1.0);
    constants.set (kWin, 0.0);
    constants.set (kLinux, 0.0);
#elif JUCE_WINDOWS
    constants.set (kMac, 0.0);
    constants.set (kWin, 1.0);
    constants.set (kLinux, 0.0);
#elif JUCE_LINUX
    constants.set (kMac, 0.0);
    constants.set (kWin, 0.0);
    constants.set (kLinux, 1.0);
#endif

    parent.getProperties().set (kUsesLayoutSupport, true);

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
    if (j.hasProperty (kConstants))
    {
        for (auto& c : getAllProperties (j[kConstants]))
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
    if (j.hasProperty (kMacros))
    {
        for (auto& c : getAllProperties (j[kMacros]))
        {
            auto id = c[kId].toString();
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

void LayoutSupport::setLayouts (const juce::StringArray& rawJson)
{
    juce::Array<JsonFile> files;

    for (auto [n, j] : juce::enumerate (rawJson))
        files.add ({ juce::String::formatted ("raw%d", int (n)), j });

    setLayoutInternal (files);
}

void LayoutSupport::setLayoutInternal (const juce::Array<JsonFile>& files)
{
    layoutSet = true;

    //
    // Find all the base level constants
    //
    compMap.createMap ([] (juce::Component& c)
                       { return ! c.getProperties().getWithDefault (kUsesLayoutSupport, false); });

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
            constants.set (kParX, parent.getX());
            constants.set (kParY, parent.getY());
            constants.set (kParCX, parent.getBounds().getCentreX());
            constants.set (kParCY, parent.getBounds().getCentreY());
            constants.set (kParW, parent.getWidth());
            constants.set (kParH, parent.getHeight());
            constants.set (kParW2, parent.getWidth() / 2);
            constants.set (kParH2, parent.getHeight() / 2);
            constants.set (kParR, parent.getRight());
            constants.set (kParB, parent.getBottom());

            loadConstants (j);
            loadMacros (j);

            if (j.hasProperty (kW) && j.hasProperty (kH))
            {
                auto w = parse (j[kW], 0);
                auto h = parse (j[kH], 0);

                parent.setSize (w, h);
            }

            if (j.hasProperty (kComponents))
            {
                auto components = j[kComponents];
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

        constants.set (kI, i);

        auto updatePrevious = [&]
        {
            //
            // Previous component constants
            //
            constants.set (kPrevX, prevComponent != nullptr ? prevComponent->getX() : 0);
            constants.set (kPrevY, prevComponent != nullptr ? prevComponent->getY() : 0);
            constants.set (kPrevCX, prevComponent != nullptr ? prevComponent->getBounds().getCentreX() : 0);
            constants.set (kPrevCY, prevComponent != nullptr ? prevComponent->getBounds().getCentreY() : 0);
            constants.set (kPrevW, prevComponent != nullptr ? prevComponent->getWidth() : 0);
            constants.set (kPrevH, prevComponent != nullptr ? prevComponent->getHeight() : 0);
            constants.set (kPrevR, prevComponent != nullptr ? prevComponent->getRight() : 0);
            constants.set (kPrevB, prevComponent != nullptr ? prevComponent->getBottom() : 0);
        };

        updatePrevious();

        if (component.hasProperty (kConstants))
            for (auto& c : getAllProperties (component[kConstants]))
                for (auto& itm : getPropertySet (c))
                    constants.set (itm.key.toStdString(), parse (itm.value, 0));

        //
        // Ignore some commands
        //
        if (component.hasProperty (kIf))
            if (parse (component[kIf], 0) == 0)
                continue;

        //
        // If there is an id, position this component. If no id, then
        // its bounds are set in C++.
        //
        if (component.hasProperty (kId))
        {
            //
            // Size and position component
            //

            juce::String idString = component[kId].toString();
            juce::StringArray ids;

            if (idString.contains (","))
                ids = expandTokens (juce::StringArray::fromTokens (idString, ",", ""));
            else
                ids = expandTokens ({ idString });

            for (auto idIdx = 0; auto& id : ids)
            {
                constants.set (kIdIdx, idIdx);
                //
                // Create component on demand
                //
                if (component.hasProperty (kFactory) && compMap.findComponent (currentPath + "/" + id) == nullptr)
                {
                    if (componentFactory)
                    {
                        if (auto par = compMap.findComponent (currentPath))
                        {
                            if (auto [comp, _toBack] = componentFactory (component[kFactory]); comp != nullptr)
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

                if (component.hasProperty (kMacro))
                    prevComponent = setPosition (currentPath, id, idIdx++, expandMacro (component));
                else
                    prevComponent = setPosition (currentPath, id, idIdx++, component);

                if (prevComponent != nullptr)
                {
                    //
                    // Handle tab order
                    //
                    if (component.hasProperty (kOrder))
                        prevComponent->setExplicitFocusOrder (int (component[kOrder]) + 1);
                    else
                        prevComponent->setExplicitFocusOrder (i + 1);

                    if (component.hasProperty (kParentOrder))
                        if (auto p = curComponent->getParentComponent())
                            p->setExplicitFocusOrder (int (component[kParentOrder]) + 1);
                }

                if (ids.size() > 0)
                    updatePrevious();

                ++i;
            }
        }
        else if (component.hasProperty (kGrid))
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
            if (component.hasProperty (kComponents))
                setComponentsLayout (currentPath, component[kComponents]);
        }
    }
}

LayoutSupport::Bounds LayoutSupport::getBounds (int idIdx, const juce::var& component)
{
    //
    // Set position and size
    //
    auto x = curComponent ? curComponent->getX () : 0;
    auto y = curComponent ? curComponent->getY () : 0;
    auto w = curComponent ? curComponent->getWidth () : 0;
    auto h = curComponent ? curComponent->getHeight () : 0;

    if (component.hasProperty (kR) && component.hasProperty (kX))
        w = parse (component[kR], idIdx) - parse (component[kX], idIdx);

    if (component.hasProperty (kY) && component.hasProperty (kB))
        h = parse (component[kB], idIdx) - parse (component[kY], idIdx);

    if (component.hasProperty (kXd))
    {
        int delta;
        auto prefix = kPrefixPrev;
        auto reference = juce::String();

        if (component[kXd].isString() && component[kXd].toString().contains (","))
        {
            auto tokens = juce::StringArray::fromTokens (component[kXd].toString(), ",", "");
            tokens.trim();

            delta = parse (tokens[0], idIdx);

            reference = "('" + tokens[1] + "')";
            prefix = kPrefixGet;
        }
        else
        {
            delta = parse (component[kXd], idIdx);
        }

        w = parse (prefix + "W" + reference, idIdx);
        h = parse (prefix + "H" + reference, idIdx);
        y = parse (prefix + "Y" + reference, idIdx);

        if (delta < 0)
            x = parse (prefix + "X" + reference, idIdx) - w + delta;
        else
            x = parse (prefix + "R" + reference, idIdx) + delta;
    }

    if (component.hasProperty (kCxd))
    {
        int delta;
        auto prefix = kPrefixPrev;
        auto reference = juce::String();

        if (component[kCxd].isString() && component[kCxd].toString().contains (","))
        {
            auto tokens = juce::StringArray::fromTokens (component[kCxd].toString(), ",", "");
            tokens.trim();

            delta = parse (tokens[0], idIdx);

            reference = "('" + tokens[1] + "')";
            prefix = kPrefixGet;
        }
        else
        {
            delta = parse (component[kCxd], idIdx);
        }

        w = parse (prefix + "W" + reference, idIdx);
        h = parse (prefix + "H" + reference, idIdx);
        y = parse (prefix + "Y" + reference, idIdx);

        if (delta < 0)
            x = parse (prefix + "CX" + reference, idIdx) - delta - w / 2;
        else
            x = parse (prefix + "CX" + reference, idIdx) + delta - w / 2;
    }

    if (component.hasProperty (kYd))
    {
        int delta;
        auto prefix = kPrefixPrev;
        auto reference = juce::String();

        if (component[kYd].isString() && component[kYd].toString().contains (","))
        {
            auto tokens = juce::StringArray::fromTokens (component[kYd].toString(), ",", "");
            tokens.trim();

            delta = parse (tokens[0], idIdx);

            reference = "('" + tokens[1] + "')";
            prefix = kPrefixGet;
        }
        else
        {
            delta = parse (component[kYd], idIdx);
        }

        w = parse (prefix + "W" + reference, idIdx);
        h = parse (prefix + "H" + reference, idIdx);
        x = parse (prefix + "X" + reference, idIdx);

        if (delta < 0)
            y = parse (prefix + "Y" + reference, idIdx) - h + delta;
        else
            y = parse (prefix + "B" + reference, idIdx) + delta;
    }

    if (component.hasProperty (kCyd))
    {
        int delta;
        auto prefix = kPrefixPrev;
        auto reference = juce::String();

        if (component[kCyd].isString() && component[kCyd].toString().contains (","))
        {
            auto tokens = juce::StringArray::fromTokens (component[kCyd].toString(), ",", "");
            tokens.trim();

            delta = parse (tokens[0], idIdx);

            reference = "('" + tokens[1] + "')";
            prefix = kPrefixGet;
        }
        else
        {
            delta = parse (component[kCyd], idIdx);
        }

        w = parse (prefix + "W" + reference, idIdx);
        h = parse (prefix + "H" + reference, idIdx);
        x = parse (prefix + "X" + reference, idIdx);

        if (delta < 0)
            y = parse (prefix + "CY" + reference, idIdx) - delta - h / 2;
        else
            y = parse (prefix + "CY" + reference, idIdx) + delta - h / 2;
    }

    if (component.hasProperty (kX))
        x = parse (component[kX], idIdx);
    if (component.hasProperty (kY))
        y = parse (component[kY], idIdx);

    if (component.hasProperty (kXy))
    {
        auto str = component[kXy].toString();
        auto tokens = juce::StringArray::fromTokens (str, ",", "");

        x = parse (tokens[0], idIdx);
        y = parse (tokens[1], idIdx);
    }

    if (component.hasProperty (kInset) && component[kInset].isString())
    {
        if (auto c = getComp (component[kInset].toString()))
        {
            auto r = c->getBounds().reduced (component.hasProperty (kBorder) ? int (component[kBorder]) : 0);
            x = r.getX();
            y = r.getY();
            w = r.getWidth();
            h = r.getHeight();
        }
    }
    if (component.hasProperty (kBounds))
    {
        if (component[kBounds] == kParent)
        {
            x = 0;
            y = 0;
            w = juce::roundToInt (*constants.get (kParW));
            h = juce::roundToInt (*constants.get (kParH));
        }
        else if (component[kBounds] == kPrev)
        {
            x = parse (kPrevX, idIdx);
            y = parse (kPrevY, idIdx);
            w = parse (kPrevW, idIdx);
            h = parse (kPrevH, idIdx);
        }
        else if (auto reduced = component[kBounds].toString(); reduced.contains (kReduced))
        {
            auto tokens = juce::StringArray::fromTokens (reduced, ",", "");

            x = 0;
            y = 0;
            w = juce::roundToInt (*constants.get (kParW));
            h = juce::roundToInt (*constants.get (kParH));

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
        }
        else if (auto str = component[kBounds].toString(); str.contains (","))
        {
            auto tokens = juce::StringArray::fromTokens (str, ",", "");

            if (tokens.size() == 4)
            {
                x = parse (tokens[0], idIdx);
                y = parse (tokens[1], idIdx);
                w = parse (tokens[2], idIdx);
                h = parse (tokens[3], idIdx);
            }
            else if (tokens.size() == 2)
            {
                x = 0;
                y = 0;
                w = parse (tokens[0], idIdx);
                h = parse (tokens[1], idIdx);
            }
        }
        else
        {
            jassertfalse;
        }
    }
    else if (component.hasProperty (kSize))
    {
        auto sizeId = component[kSize].toString();

        w = parse (sizeId + ".w", 0);
        h = parse (sizeId + ".h", 0);
    }

    if (component.hasProperty (kW))
        w = parse (component[kW], idIdx);
    if (component.hasProperty (kH))
        h = parse (component[kH], idIdx);
    if (component.hasProperty (kCx))
        x = parse (component[kCx], idIdx) - w / 2;
    if (component.hasProperty (kCy))
        y = parse (component[kCy], idIdx) - h / 2;
    if (component.hasProperty (kR))
        x = parse (component[kR], idIdx) - w;
    if (component.hasProperty (kB))
        y = parse (component[kB], idIdx) - h;

    LayoutSupport::Bounds res;
    res.x = x;
    res.y = y;
    res.h = h;
    res.w = w;

    if (component.hasProperty (kPW))
        res.postWidth = component[kPW].toString();

    if (component.hasProperty (kPH))
        res.postHeight = component[kPH].toString();

    return res;
}

juce::var LayoutSupport::expandMacro (const juce::var& component_)
{
    auto component = component_;

    juce::String str = component[kMacro].toString();

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

        if (key == kId.toString())
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
        if (component.hasProperty (kTitle))
            curComponent->setTitle (component[kTitle].toString());

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
        if (component.hasProperty (kTip))
            if (auto ttc = dynamic_cast<juce::SettableTooltipClient*> (curComponent))
                ttc->setTooltip (component[kTip].toString());

        //
        // Handle properties
        //
        if (component.hasProperty (kProperties))
        {
            auto p = component[kProperties];
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

        auto hasVisible = component.hasProperty (kVisible);
        auto bounds = getBounds (idIdx, component);

        curComponent->setTopLeftPosition (bounds.x, bounds.y);
        curComponent->setSize (bounds.w, bounds.h);

        if ((bounds.postWidth.has_value() != bounds.postHeight.has_value()) && (bounds.w >= 0 || bounds.h >= 0))
            curComponent->setSize (bounds.w, bounds.h);

        if (component.hasProperty (kZorder))
        {
            auto p = curComponent->getParentComponent();
            int zorder = component[kZorder];
            if (p->getChildComponent (zorder) != curComponent)
            {
                p->removeChildComponent (curComponent);
                p->addChildComponent (curComponent, zorder);
            }
        }

        if (hasVisible && component[kVisible].isBool())
            curComponent->setVisible (bool (component[kVisible]));
        else if (hasVisible && component[kVisible].isString())
            curComponent->setVisible (parse (component[kVisible], idIdx) != 0);

        //
        // Handle sub components
        //
        if (component.hasProperty (kComponents))
        {
            ConstantsStack::ScopedSave ssp (constants);

            constants.set (kParX, foundComp->getX());
            constants.set (kParY, foundComp->getY());
            constants.set (kParCX, foundComp->getBounds().getCentreX());
            constants.set (kParCY, foundComp->getBounds().getCentreY());
            constants.set (kParW, foundComp->getWidth());
            constants.set (kParH, foundComp->getHeight());
            constants.set (kParW2, foundComp->getWidth() / 2);
            constants.set (kParH2, foundComp->getHeight() / 2);
            constants.set (kParR, foundComp->getRight());
            constants.set (kParB, foundComp->getBottom());

            setComponentsLayout (path, component[kComponents]);
        }

        if (bounds.postWidth.has_value() || bounds.postHeight.has_value())
        {
            ConstantsStack::ScopedSave ssp (constants);

            auto getChildBounds = [](const juce::Component& p)
            {
                if (p.getNumChildComponents() == 0)
                    return std::make_tuple (0, 0, 0, 0);

                auto minX = std::numeric_limits<int>::max();
                auto minY = std::numeric_limits<int>::max();
                auto maxX = std::numeric_limits<int>::min();
                auto maxY = std::numeric_limits<int>::min();

                for (int i = 0; i < p.getNumChildComponents(); ++i)
                {
                    auto b = p.getChildComponent (i)->getBounds();

                    minX = std::min (minX, b.getX());
                    minY = std::min (minY, b.getY());
                    maxX = std::max (maxX, b.getRight());
                    maxY = std::max (maxY, b.getBottom());
                }

                return std::make_tuple (minX, minY, maxX, maxY);
            };

            auto [minX, minY, maxX, maxY] = getChildBounds (*curComponent);

            constants.set (kMinX, minX);
            constants.set (kMinY, minY);
            constants.set (kMaxX, maxX);
            constants.set (kMaxY, maxY);

            auto w = bounds.w;
            auto h = bounds.h;

            if (bounds.postWidth.has_value())
                w = parse (*bounds.postWidth, 0);
            if (bounds.postHeight.has_value())
                h = parse (*bounds.postHeight, 0);

            curComponent->setSize (w, h);
        }

        return curComponent;
    }
    else
    {
        bool optional = false;
        if (component.hasProperty (kOptional))
            optional = component[kOptional];

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
    juce::String idString = component[kGrid].toString();
    juce::StringArray ids;

    if (idString.contains (","))
        ids = expandTokens (juce::StringArray::fromTokens (idString, ",", ""));
    else
        ids = expandTokens ({ idString });

    ids.trim ();
    
    if (auto foundComp = compMap.findComponent (currentPath))
    {
        juce::ScopedValueSetter<juce::Component*> svs (curComponent, foundComp);
        
        auto bounds = getBounds (0, component);
        
        juce::Rectangle<int> rc (bounds.x, bounds.y, bounds.w, bounds.h);
        
        using juce::operator""_px;
        using juce::operator""_fr;
        
        juce::Grid grid;
        
        auto cols = parse (getPropertyWithDefault (component, kCols, 1), 0);
        auto rows = parse (getPropertyWithDefault (component, kRows, 1), 0);
        
        grid.columnGap  = juce::Grid::Px (parse (getPropertyWithDefault (component, kColGap, 0), 0));
        grid.rowGap     = juce::Grid::Px (parse (getPropertyWithDefault (component, kRowGap, 0), 0));
        
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
            
            if (auto foundChildComp = compMap.findComponent (path))
                grid.items.add (juce::GridItem (foundChildComp));
            else
                grid.items.add (juce::GridItem());
        }
        
        grid.performLayout (rc);
    }
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
        return juce::JSON::toString (v).contains (kId);
    };

    std::function<void (juce::Component&, juce::DynamicObject&)> populate = [&] (juce::Component& c, juce::DynamicObject& o)
    {
        auto name = getComponentID (c);

        if (name != "" || &c == &root)
        {
            auto r = c.getBounds();

            if (name != "")
            {
                o.setProperty (kId, juce::String (name));
                o.setProperty (kX, r.getX());
                o.setProperty (kY, r.getY());
                o.setProperty (kW, r.getWidth());
                o.setProperty (kH, r.getHeight());
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
                o.setProperty (kComponents, childrenArr);
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
