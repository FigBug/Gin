
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
   #if ! JUCE_IOS && ! JUCE_ANDROID
    watcher.addListener (this);
   #endif
}

Layout::~Layout()
{
   #if ! JUCE_IOS && ! JUCE_ANDROID
    watcher.removeListener (this);
   #endif
}

void Layout::setConstant (const juce::String& name, double val)
{
    constants[name] = val;
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

            DBG(cid + " not found");
            jassertfalse;
            return nullptr;
        };

        parser.addFunction ("getX", [getComp] (int, const juce::String& id) { if (auto c = getComp (id))    return c->getX();       return 0; });
        parser.addFunction ("getY", [getComp] (int, const juce::String& id) { if (auto c = getComp (id))    return c->getY();       return 0; });
        parser.addFunction ("getW", [getComp] (int, const juce::String& id) { if (auto c = getComp (id))    return c->getWidth();   return 0; });
        parser.addFunction ("getH", [getComp] (int, const juce::String& id) { if (auto c = getComp (id))    return c->getHeight();  return 0; });
        parser.addFunction ("getR", [getComp] (int, const juce::String& id) { if (auto c = getComp (id))    return c->getRight();   return 0; });
        parser.addFunction ("getB", [getComp] (int, const juce::String& id) { if (auto c = getComp (id))    return c->getBottom();  return 0; });
        
        parser.addFunction ("getCX", [getComp] (int, const juce::String& id) { if (auto c = getComp (id))   return c->getBounds().getCentreX(); return 0; });
        parser.addFunction ("getCY", [getComp] (int, const juce::String& id) { if (auto c = getComp (id))   return c->getBounds().getCentreY(); return 0; });

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
                   #if ! JUCE_IOS && ! JUCE_ANDROID
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
    doComponent (constants, {}, obj);
    componentMap = {};
                    
    return true;
}

void Layout::doComponent (const std::map<juce::String, double>& constantsIn, const juce::String& currentPath, const juce::var& component)
{
    auto curConstants = constantsIn;
    
    if (component.hasProperty ("constants"))
        if (auto obj = component.getDynamicObject())
            for (auto nv : obj->getProperties())
                curConstants[nv.name.toString()] = double (nv.value);
    
    if (component.hasProperty ("if"))
        if (parse (curConstants, component["if"], 0) == 0)
            return;
    
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
            prevComponent = setBounds (curConstants, currentPath, id, idIdx++, component);
    }
    else
    {
        if (component.hasProperty ("children"))
        {
            auto arr = component["children"];
            if (arr.isArray())
                for (auto c : *arr.getArray())
                    doComponent (curConstants, currentPath, c);
        }
    }
}

juce::Component* Layout::setBounds (const std::map<juce::String, double>& constantsIn, const juce::String& currentPath, const juce::String& id, int idIdx, const juce::var& component)
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
    std::optional<int> cx;
    std::optional<int> cy;
    std::optional<int> r;
    std::optional<int> b;
    std::optional<int> w;
    std::optional<int> h;

    if (component.hasProperty ("x"))    x = parse (constantsIn, component["x"], idIdx);
    if (component.hasProperty ("y"))    y = parse (constantsIn, component["y"], idIdx);
    if (component.hasProperty ("r"))    r = parse (constantsIn, component["r"], idIdx);
    if (component.hasProperty ("b"))    b = parse (constantsIn, component["b"], idIdx);

    if (component.hasProperty ("w"))    w = parse (constantsIn, component["w"], idIdx);
    if (component.hasProperty ("h"))    h = parse (constantsIn, component["h"], idIdx);
    
    if (component.hasProperty ("cx"))   cx = parse (constantsIn, component["cx"], idIdx);
    if (component.hasProperty ("cy"))   cy = parse (constantsIn, component["cy"], idIdx);

    if (cx.has_value() && w.has_value()) x = *cx - *w / 2;
    if (cy.has_value() && h.has_value()) y = *cy - *h / 2;
    
    if (r.has_value() && x.has_value()) w = *r - *x;
    if (b.has_value() && y.has_value()) h = *b - *y;
    if (r.has_value() && w.has_value()) x = *r - *w;
    if (b.has_value() && h.has_value()) y = *b - *h;

    if (component.hasProperty ("bounds"))
    {
        if (component["bounds"] == "parent")
        {
            if (auto p = curComponent->getParentComponent())
            {
                x = 0;
                y = 0;
                w = p->getWidth();
                h = p->getHeight();
            }
        }
        else if (component["bounds"] == "prev")
        {
            if (prevComponent)
            {
                x = prevComponent->getX();
                y = prevComponent->getY();
                w = prevComponent->getWidth();
                h = prevComponent->getHeight();
            }
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
                doComponent (constantsIn, path, c);
    }
    
    if (component.hasProperty ("properties"))
        if (auto obj = component.getDynamicObject())
            for (auto nv : obj->getProperties())
                curComponent->getProperties().set (nv.name, nv.value);

    return curComponent;
}

#if ! JUCE_IOS && ! JUCE_ANDROID
void Layout::fileChanged (const juce::File& f, gin::FileSystemWatcher::FileSystemEvent)
{
    if (f == layoutFile)
        if (const auto str = layoutFile.loadFileAsString(); str.isNotEmpty())
            parseLayout (str);
}
#endif

std::map<juce::String, juce::Component*> Layout::findAllComponents() const
{
    std::map<juce::String, juce::Component*> components;

    std::function<void (juce::Component&)> findAll = [&] (const juce::Component& p)
    {
        for (auto* c : p.getChildren())
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

int Layout::parse (const std::map<juce::String, double>& constantsIn, const juce::var& equation, int equationIndex)
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
            
            if (equationText.containsOnly ("1234567890."))
                return int (equationText.getDoubleValue());
            
            for (auto itr : constantsIn)
                parser.addConstant (itr.first, itr.second);

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

//==============================================================================
#if JUCE_UNIT_TESTS

class LayoutPathTests : public juce::UnitTest
{
public:
    LayoutPathTests() : juce::UnitTest ("Layout Path Utilities", "gin_gui") {}

    void runTest() override
    {
        testGetParentPath();
        testAddTrailingSlash();
        testIsAbsolutePath();
        testGetChildPath();
        testExpandTokens();
    }

private:
    void testGetParentPath()
    {
        beginTest ("Get Parent Path");

        expect (getParentPath ("/foo/bar/baz") == "/foo/bar", "Should get parent of /foo/bar/baz");
        expect (getParentPath ("/foo/bar") == "/foo", "Should get parent of /foo/bar");
        expect (getParentPath ("/foo") == "/", "Should get parent of /foo");
        expect (getParentPath ("/") == "/", "Parent of / should be /");
        expect (getParentPath ("foo") == "foo", "No slash should return same");
    }

    void testAddTrailingSlash()
    {
        beginTest ("Add Trailing Slash");

        expect (addTrailingSlash ("/foo") == "/foo/", "Should add trailing slash");
        expect (addTrailingSlash ("/foo/") == "/foo/", "Should not double slash");
        expect (addTrailingSlash ("") == "/", "Empty string should become /");
    }

    void testIsAbsolutePath()
    {
        beginTest ("Is Absolute Path");

        expect (isAbsolutePath ("/foo/bar"), "Should recognize /foo/bar as absolute");
        expect (isAbsolutePath ("/"), "Should recognize / as absolute");
        expect (isAbsolutePath ("~/foo"), "Should recognize ~/foo as absolute");
        expect (!isAbsolutePath ("foo/bar"), "Should recognize foo/bar as relative");
        expect (!isAbsolutePath ("./foo"), "Should recognize ./foo as relative");
        expect (!isAbsolutePath ("../foo"), "Should recognize ../foo as relative");
    }

    void testGetChildPath()
    {
        beginTest ("Get Child Path");

        // Basic child path
        expect (getChildPath ("/foo", "bar") == "/foo/bar", "Should append child");
        expect (getChildPath ("/foo/", "bar") == "/foo/bar", "Should handle trailing slash");

        // Absolute paths
        expect (getChildPath ("/foo", "/bar") == "/bar", "Absolute path should override");
        expect (getChildPath ("/foo", "~/bar") == "~/bar", "Home path should override");

        // Current directory
        expect (getChildPath ("/foo", "./bar") == "/foo/bar", "Should handle ./");
        expect (getChildPath ("/foo", "./bar/baz") == "/foo/bar/baz", "Should handle ./ with subpath");

        // Parent directory
        expect (getChildPath ("/foo/bar", "../baz") == "/foo/baz", "Should handle ../");
        expect (getChildPath ("/foo/bar/qux", "../../baz") == "/foo/baz", "Should handle ../../");
        expect (getChildPath ("/foo", "../bar") == "/bar", "Should handle ../ at root level");

        // Multiple dots
        expect (getChildPath ("/foo/bar", ".././baz") == "/foo/baz", "Should handle .././");
    }

    void testExpandTokens()
    {
        beginTest ("Expand Tokens");

        // No expansion
        juce::StringArray simple {"foo", "bar"};
        auto result1 = expandTokens (simple);
        expect (result1.size() == 2, "Should not expand simple strings");
        expect (result1[0] == "foo", "First should be foo");
        expect (result1[1] == "bar", "Second should be bar");

        // Range expansion
        juce::StringArray range {"item[0..2]"};
        auto result2 = expandTokens (range);
        expect (result2.size() == 3, "Should expand to 3 items");
        expect (result2[0] == "item0", "First should be item0");
        expect (result2[1] == "item1", "Second should be item1");
        expect (result2[2] == "item2", "Third should be item2");

        // Range with prefix and postfix
        juce::StringArray complex {"btn[1..3]_label"};
        auto result3 = expandTokens (complex);
        expect (result3.size() == 3, "Should expand to 3 items");
        expect (result3[0] == "btn1_label", "First should be btn1_label");
        expect (result3[1] == "btn2_label", "Second should be btn2_label");
        expect (result3[2] == "btn3_label", "Third should be btn3_label");

        // Mixed array
        juce::StringArray mixed {"static", "dyn[0..1]", "end"};
        auto result4 = expandTokens (mixed);
        expect (result4.size() == 4, "Should expand to 4 items");
        expect (result4[0] == "static", "First should be static");
        expect (result4[1] == "dyn0", "Second should be dyn0");
        expect (result4[2] == "dyn1", "Third should be dyn1");
        expect (result4[3] == "end", "Fourth should be end");
    }
};

static LayoutPathTests layoutPathTests;

#endif
