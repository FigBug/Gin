/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
namespace remote_detail
{
    using Result = RemoteServer::CommandResult;

    inline juce::var rectToVar (juce::Rectangle<int> r)
    {
        juce::Array<juce::var> a;
        a.add (r.getX()); a.add (r.getY()); a.add (r.getWidth()); a.add (r.getHeight());
        return a;
    }

    inline juce::var pointToVar (juce::Point<int> p)
    {
        juce::Array<juce::var> a;
        a.add (p.x); a.add (p.y);
        return a;
    }

    inline bool varToRect (const juce::var& v, juce::Rectangle<int>& r)
    {
        if (! v.isArray() || v.size() < 4)
            return false;
        r = { int (v[0]), int (v[1]), int (v[2]), int (v[3]) };
        return true;
    }

    inline juce::Rectangle<int> screenBounds (juce::Component& c)
    {
        return c.getScreenBounds();
    }

    inline juce::String roleName (juce::AccessibilityRole role)
    {
        using R = juce::AccessibilityRole;
        switch (role)
        {
            case R::button:         return "button";
            case R::toggleButton:   return "toggleButton";
            case R::radioButton:    return "radioButton";
            case R::comboBox:       return "comboBox";
            case R::image:          return "image";
            case R::slider:         return "slider";
            case R::label:          return "label";
            case R::staticText:     return "staticText";
            case R::editableText:   return "editableText";
            case R::menuItem:       return "menuItem";
            case R::menuBar:        return "menuBar";
            case R::popupMenu:      return "popupMenu";
            case R::table:          return "table";
            case R::tableHeader:    return "tableHeader";
            case R::column:         return "column";
            case R::row:            return "row";
            case R::cell:           return "cell";
            case R::hyperlink:      return "hyperlink";
            case R::list:           return "list";
            case R::listItem:       return "listItem";
            case R::tree:           return "tree";
            case R::treeItem:       return "treeItem";
            case R::progressBar:    return "progressBar";
            case R::group:          return "group";
            case R::dialogWindow:   return "dialogWindow";
            case R::window:         return "window";
            case R::scrollBar:      return "scrollBar";
            case R::tooltip:        return "tooltip";
            case R::splashScreen:   return "splashScreen";
            case R::ignored:        return "ignored";
            case R::unspecified:    return "unspecified";
            default:                return "unknown";
        }
    }

    inline juce::ModifierKeys parseMods (const juce::var& v, juce::ModifierKeys base = {})
    {
        auto mods = base;

        auto apply = [&] (juce::String s)
        {
            s = s.trim().toLowerCase();
            if (s == "shift")                               mods = mods.withFlags (juce::ModifierKeys::shiftModifier);
            else if (s == "ctrl" || s == "control")         mods = mods.withFlags (juce::ModifierKeys::ctrlModifier);
            else if (s == "alt" || s == "option")           mods = mods.withFlags (juce::ModifierKeys::altModifier);
            else if (s == "cmd" || s == "command" || s == "meta" || s == "win") mods = mods.withFlags (juce::ModifierKeys::commandModifier);
            else if (s == "popup")                          mods = mods.withFlags (juce::ModifierKeys::popupMenuClickModifier);
        };

        if (v.isArray())
        {
            for (auto& m : *v.getArray())
                apply (m.toString());
        }
        else if (v.isString())
        {
            for (auto& m : juce::StringArray::fromTokens (v.toString(), "+, ", {}))
                apply (m);
        }

        return mods;
    }

    inline int buttonModifier (const juce::String& button)
    {
        auto b = button.trim().toLowerCase();
        if (b == "right")   return juce::ModifierKeys::rightButtonModifier | juce::ModifierKeys::popupMenuClickModifier;
        if (b == "middle")  return juce::ModifierKeys::middleButtonModifier;
        return juce::ModifierKeys::leftButtonModifier;
    }

    inline void collectDescendants (juce::Component& c, bool visibleOnly, juce::Array<juce::Component*>& out)
    {
        for (int i = 0; i < c.getNumChildComponents(); i++)
        {
            auto child = c.getChildComponent (i);
            if (visibleOnly && ! child->isVisible())
                continue;

            out.add (child);
            collectDescendants (*child, visibleOnly, out);
        }
    }

    inline juce::Array<juce::Component*> getRoots (juce::Component* root, bool visibleOnly)
    {
        juce::Array<juce::Component*> roots;
        if (root != nullptr)
        {
            roots.add (root);
        }
        else
        {
            auto& desktop = juce::Desktop::getInstance();
            for (int i = 0; i < desktop.getNumComponents(); i++)
                if (auto c = desktop.getComponent (i))
                    if (! visibleOnly || c->isVisible())
                        roots.add (c);
        }
        return roots;
    }

    inline bool classMatches (juce::Component& c, const juce::String& cls)
    {
        auto name = RemoteServer::getClassName (c);
        if (name == cls || name.endsWith ("::" + cls))
            return true;

        // allow matching a template base, e.g. .Foo matches Foo<int>
        auto stripped = name.upToFirstOccurrenceOf ("<", false, false);
        return stripped == cls || stripped.endsWith ("::" + cls);
    }

    inline bool stepMatches (juce::Component& c, const juce::String& step)
    {
        if (step == "*")
            return true;

        if (step.startsWith ("#"))
            return c.getComponentID() == step.substring (1);

        if (step.startsWith ("."))
            return classMatches (c, step.substring (1));

        if (step.startsWith ("~"))
        {
            auto needle = step.substring (1).unquoted().toLowerCase();
            return c.getName().toLowerCase().contains (needle)
                || c.getComponentID().toLowerCase().contains (needle)
                || RemoteServer::getComponentText (c).toLowerCase().contains (needle)
                || c.getTitle().toLowerCase().contains (needle);
        }

        auto text = step.unquoted();

        return c.getName() == text || RemoteServer::getComponentText (c) == text || c.getTitle() == text;
    }

    inline juce::Component* componentAtPath (const juce::String& path, juce::Component* root)
    {
        auto parts = juce::StringArray::fromTokens (path, "/", {});
        parts.removeEmptyStrings();
        if (parts.isEmpty())
            return nullptr;

        juce::Component* c = nullptr;
        int start = 0;

        if (root != nullptr)
        {
            c = root;
        }
        else
        {
            auto& desktop = juce::Desktop::getInstance();
            int idx = parts[0].getIntValue();
            if (idx < 0 || idx >= desktop.getNumComponents())
                return nullptr;
            c = desktop.getComponent (idx);
            start = 1;
        }

        for (int i = start; i < parts.size() && c != nullptr; i++)
        {
            int idx = parts[i].getIntValue();
            c = (idx >= 0 && idx < c->getNumChildComponents()) ? c->getChildComponent (idx) : nullptr;
        }

        return c;
    }

    // Splits a selector into steps, respecting quotes
    inline juce::StringArray tokenise (const juce::String& selector)
    {
        juce::StringArray steps;
        juce::String current;
        juce::juce_wchar quote = 0;

        for (auto t = selector.getCharPointer(); ! t.isEmpty(); ++t)
        {
            auto ch = *t;

            if (quote != 0)
            {
                current += juce::String::charToString (ch);
                if (ch == quote)
                    quote = 0;
            }
            else if (ch == '\'' || ch == '"')
            {
                quote = ch;
                current += juce::String::charToString (ch);
            }
            else if (juce::CharacterFunctions::isWhitespace (ch))
            {
                if (current.isNotEmpty())
                    steps.add (current);
                current.clear();
            }
            else
            {
                current += juce::String::charToString (ch);
            }
        }

        if (current.isNotEmpty())
            steps.add (current);

        return steps;
    }

    // Component properties can hold methods, binary blobs and NaNs, none of which survive JSON::toString
    inline juce::var sanitiseForJson (const juce::var& v, int depth = 0)
    {
        if (v.isVoid() || v.isBool() || v.isInt() || v.isInt64() || v.isString())
            return v;

        if (v.isDouble())
            return std::isfinite (double (v)) ? v : juce::var();

        if (depth > 8)
            return "<nested>";

        if (v.isArray())
        {
            juce::Array<juce::var> out;
            for (auto& item : *v.getArray())
                out.add (sanitiseForJson (item, depth + 1));
            return out;
        }

        if (auto obj = v.getDynamicObject())
        {
            auto out = new juce::DynamicObject();
            for (auto& prop : obj->getProperties())
                out->setProperty (prop.name, sanitiseForJson (prop.value, depth + 1));
            return juce::var (out);
        }

        if (v.isMethod())       return "<method>";
        if (v.isBinaryData())   return "<binary " + juce::String (v.getBinaryData()->getSize()) + " bytes>";
        if (v.isObject())       return "<object>";
        return v.toString();
    }

    inline void drawHighlight (juce::Graphics& g, juce::Rectangle<float> r, const juce::String& label, juce::Colour colour)
    {
        g.setColour (colour);
        g.drawRect (r, 2.0f);

        if (label.isNotEmpty())
        {
           #if JUCE_MAJOR_VERSION >= 8
            g.setFont (juce::Font (juce::FontOptions (11.0f)));
            auto textWidth = juce::GlyphArrangement::getStringWidth (g.getCurrentFont(), label);
           #else
            g.setFont (11.0f);
            auto textWidth = g.getCurrentFont().getStringWidthFloat (label);
           #endif
            auto w = juce::jmax (12.0f, textWidth + 6.0f);
            auto tag = juce::Rectangle<float> (r.getX(), juce::jmax (0.0f, r.getY() - 14.0f), w, 14.0f);
            g.fillRect (tag);
            g.setColour (colour.contrasting());
            g.drawText (label, tag, juce::Justification::centred, false);
        }
    }
}

//==============================================================================
juce::Array<juce::Component*> RemoteServer::findComponents (const juce::String& selector, bool visibleOnly, juce::Component* root)
{
    using namespace remote_detail;

    auto steps = tokenise (selector.trim());
    if (steps.isEmpty())
        return getRoots (root, visibleOnly);

    auto matches = findComponents (steps, visibleOnly, root);

    // "Component Viewer" is more likely one button than a Component containing a Viewer,
    // so if the chained reading finds nothing, try the whole thing as a single name
    if (matches.isEmpty() && steps.size() > 1 && ! selector.containsAnyOf ("@/[]*\"'"))
    {
        bool laterStepsArePlain = true;
        for (int i = 1; i < steps.size(); i++)
            if (steps[i].startsWithChar ('#') || steps[i].startsWithChar ('.') || steps[i].startsWithChar ('~'))
                laterStepsArePlain = false;

        if (laterStepsArePlain)
            matches = findComponents (juce::StringArray (selector.trim()), visibleOnly, root);
    }

    return matches;
}

juce::Array<juce::Component*> RemoteServer::findComponents (const juce::StringArray& steps, bool visibleOnly, juce::Component* root)
{
    using namespace remote_detail;

    juce::Array<juce::Component*> current;
    bool first = true;

    for (auto step : steps)
    {
        int pick = -1;
        if (step.endsWithChar (']') && step.containsChar ('['))
        {
            pick = step.fromLastOccurrenceOf ("[", false, false).dropLastCharacters (1).getIntValue();
            step = step.upToLastOccurrenceOf ("[", false, false);
        }

        juce::Array<juce::Component*> next;

        if (step.startsWith ("@"))
        {
            auto xy = juce::StringArray::fromTokens (step.substring (1), ",", {});
            if (xy.size() == 2)
                if (auto c = getComponentAt ({ xy[0].getIntValue(), xy[1].getIntValue() }))
                    next.add (c);
        }
        else if (step.startsWith ("/"))
        {
            if (first)
            {
                if (auto c = componentAtPath (step, root))
                    next.add (c);
            }
            else
            {
                for (auto base : current)
                    if (auto c = componentAtPath (step, base))
                        next.addIfNotAlreadyThere (c);
            }
        }
        else
        {
            juce::Array<juce::Component*> candidates;

            if (first)
            {
                for (auto r : getRoots (root, visibleOnly))
                {
                    candidates.add (r);
                    collectDescendants (*r, visibleOnly, candidates);
                }
            }
            else
            {
                for (auto base : current)
                    collectDescendants (*base, visibleOnly, candidates);
            }

            for (auto c : candidates)
                if (stepMatches (*c, step))
                    next.addIfNotAlreadyThere (c);
        }

        if (pick >= 0)
        {
            juce::Array<juce::Component*> picked;
            if (pick < next.size())
                picked.add (next[pick]);
            next = picked;
        }

        current = next;
        first = false;

        if (current.isEmpty())
            break;
    }

    return current;
}

juce::Component* RemoteServer::findComponent (const juce::String& selector, bool visibleOnly, juce::Component* root)
{
    auto matches = findComponents (selector, visibleOnly, root);
    return matches.isEmpty() ? nullptr : matches.getFirst();
}

//==============================================================================
juce::var RemoteServer::describeComponent (juce::Component& c, bool full)
{
    using namespace remote_detail;

    auto obj = new juce::DynamicObject();
    juce::var result (obj);

    obj->setProperty ("class", getClassName (c));
    if (c.getName().isNotEmpty())           obj->setProperty ("name", c.getName());
    if (c.getComponentID().isNotEmpty())    obj->setProperty ("id", c.getComponentID());
    obj->setProperty ("path", getComponentPath (c));
    obj->setProperty ("bounds", rectToVar (c.getBounds()));
    obj->setProperty ("screen", rectToVar (screenBounds (c)));

    auto text = getComponentText (c);
    if (text.isNotEmpty())                  obj->setProperty ("text", text);

    auto value = getComponentValue (c);
    if (! value.isVoid())                   obj->setProperty ("value", value);

    if (c.getTitle().isNotEmpty())          obj->setProperty ("title", c.getTitle());

    if (! c.isVisible())                    obj->setProperty ("visible", false);
    if (! c.isEnabled())                    obj->setProperty ("enabled", false);
    if (c.getAlpha() < 1.0f)                obj->setProperty ("alpha", c.getAlpha());
    if (c.isOpaque())                       obj->setProperty ("opaque", true);
    if (c.hasKeyboardFocus (false))         obj->setProperty ("focused", true);

    {
        bool self = true, children = true;
        c.getInterceptsMouseClicks (self, children);
        if (! self || ! children)
        {
            juce::Array<juce::var> a;
            a.add (self); a.add (children);
            obj->setProperty ("interceptsMouse", a);
        }
    }

    if (auto ttc = dynamic_cast<juce::SettableTooltipClient*> (&c))
        if (ttc->getTooltip().isNotEmpty())
            obj->setProperty ("tooltip", ttc->getTooltip());

    if (c.getNumChildComponents() > 0)
        obj->setProperty ("childCount", c.getNumChildComponents());

    if (full)
    {
        obj->setProperty ("showing", c.isShowing());
        obj->setProperty ("mouseOver", c.isMouseOver (true));
        obj->setProperty ("onDesktop", c.isOnDesktop());
        obj->setProperty ("wantsKeyboardFocus", c.getWantsKeyboardFocus());
        obj->setProperty ("paintingUnclipped", c.isPaintingUnclipped());
        obj->setProperty ("hasPeer", c.getPeer() != nullptr);
        obj->setProperty ("modal", c.isCurrentlyModal());
        obj->setProperty ("blockedByModal", c.isCurrentlyBlockedByAnotherModalComponent());

        if (auto s = dynamic_cast<juce::Slider*> (&c))
        {
            auto range = new juce::DynamicObject();
            range->setProperty ("min", s->getMinimum());
            range->setProperty ("max", s->getMaximum());
            range->setProperty ("interval", s->getInterval());
            obj->setProperty ("range", juce::var (range));
            obj->setProperty ("valueText", s->getTextFromValue (s->getValue()));
        }

        if (auto cb = dynamic_cast<juce::ComboBox*> (&c))
        {
            juce::Array<juce::var> items;
            for (int i = 0; i < cb->getNumItems(); i++)
            {
                auto item = new juce::DynamicObject();
                item->setProperty ("id", cb->getItemId (i));
                item->setProperty ("text", cb->getItemText (i));
                items.add (juce::var (item));
            }
            obj->setProperty ("items", items);
        }

        if (c.getProperties().size() > 0)
        {
            auto props = new juce::DynamicObject();
            for (int i = 0; i < c.getProperties().size(); i++)
                props->setProperty (c.getProperties().getName (i), sanitiseForJson (c.getProperties().getValueAt (i)));
            obj->setProperty ("properties", juce::var (props));
        }

        if (auto handler = c.getAccessibilityHandler())
        {
            auto a11y = new juce::DynamicObject();
            a11y->setProperty ("role", roleName (handler->getRole()));
            if (handler->getTitle().isNotEmpty())       a11y->setProperty ("title", handler->getTitle());
            if (handler->getDescription().isNotEmpty()) a11y->setProperty ("description", handler->getDescription());
            if (handler->getHelp().isNotEmpty())        a11y->setProperty ("help", handler->getHelp());

            if (auto vi = handler->getValueInterface())
            {
                a11y->setProperty ("value", vi->getCurrentValueAsString());
                a11y->setProperty ("readOnly", vi->isReadOnly());
            }

            auto state = handler->getCurrentState();
            juce::StringArray flags;
            if (state.isCheckable())    flags.add (state.isChecked() ? "checked" : "unchecked");
            if (state.isExpandable())   flags.add (state.isExpanded() ? "expanded" : "collapsed");
            if (state.isSelectable())   flags.add (state.isSelected() ? "selected" : "unselected");
            if (state.isFocusable())    flags.add ("focusable");
            if (state.isIgnored())      flags.add ("ignored");
            if (! flags.isEmpty())
                a11y->setProperty ("state", flags.joinIntoString (","));

            juce::StringArray actions;
            using A = juce::AccessibilityActionType;
            if (handler->getActions().contains (A::press))      actions.add ("press");
            if (handler->getActions().contains (A::toggle))     actions.add ("toggle");
            if (handler->getActions().contains (A::focus))      actions.add ("focus");
            if (handler->getActions().contains (A::showMenu))   actions.add ("showMenu");
            if (! actions.isEmpty())
                a11y->setProperty ("actions", actions.joinIntoString (","));

            obj->setProperty ("accessibility", juce::var (a11y));
        }

        juce::Array<juce::var> ancestors;
        for (auto p = c.getParentComponent(); p != nullptr; p = p->getParentComponent())
        {
            auto a = new juce::DynamicObject();
            a->setProperty ("class", getClassName (*p));
            if (p->getName().isNotEmpty())        a->setProperty ("name", p->getName());
            if (p->getComponentID().isNotEmpty()) a->setProperty ("id", p->getComponentID());
            ancestors.add (juce::var (a));
        }
        obj->setProperty ("ancestors", ancestors);
    }

    std::vector<ComponentInfoProvider> providers;
    {
        juce::ScopedLock sl (commandLock);
        providers = infoProviders;
    }
    for (auto& p : providers)
        p (c, *obj);

    return result;
}

juce::var RemoteServer::describeTree (juce::Component& c, int depth, bool visibleOnly, int maxNodes)
{
    int nodes = 0;
    bool truncated = false;

    std::function<juce::var (juce::Component&, int)> visit = [&] (juce::Component& comp, int level) -> juce::var
    {
        auto v = describeComponent (comp, false);
        nodes++;

        if (depth >= 0 && level >= depth)
            return v;

        juce::Array<juce::var> children;
        for (int i = 0; i < comp.getNumChildComponents(); i++)
        {
            auto child = comp.getChildComponent (i);
            if (visibleOnly && ! child->isVisible())
                continue;

            if (nodes >= maxNodes)
            {
                truncated = true;
                break;
            }

            children.add (visit (*child, level + 1));
        }

        if (! children.isEmpty())
            v.getDynamicObject()->setProperty ("children", children);

        return v;
    };

    auto result = visit (c, 0);
    if (truncated)
        result.getDynamicObject()->setProperty ("truncated", true);
    return result;
}

//==============================================================================
void RemoteServer::addBuiltInCommands()
{
    using namespace remote_detail;

    // Visible components first, so ".Slider[0]" means the same thing to every command,
    // then hidden ones so state can still be read and written on things that are not showing
    auto resolveTarget = [] (const juce::var& args, bool allowHidden, juce::Component*& out) -> juce::String
    {
        auto selector = getArg (args, "target").toString();
        out = findComponent (selector, true);
        if (out == nullptr && allowHidden)
            out = findComponent (selector, false);
        if (out == nullptr)
            return "No component matches '" + selector + "'";
        return {};
    };

    // position of a click: explicit x,y in screen space, else the centre of the target, offset if given
    auto resolvePoint = [resolveTarget] (const juce::var& args, juce::Component*& comp, juce::Point<float>& pos) -> juce::String
    {
        comp = nullptr;

        if (args.hasProperty ("target") && getArg (args, "target").toString().isNotEmpty())
        {
            if (auto err = resolveTarget (args, false, comp); err.isNotEmpty())
                return err;

            auto b = comp->getScreenBounds().toFloat();
            pos = b.getCentre();

            if (args.hasProperty ("x"))
                pos.x = b.getX() + float (args["x"]);
            if (args.hasProperty ("y"))
                pos.y = b.getY() + float (args["y"]);
        }
        else if (args.hasProperty ("x") && args.hasProperty ("y"))
        {
            pos = { float (args["x"]), float (args["y"]) };
            comp = getComponentAt (pos.toInt());
        }
        else
        {
            return "Need a target selector or x,y screen coordinates";
        }

        return {};
    };

    //==============================================================================
    addCommand ("ping", "Server and application info", "{}", [this] (const juce::var&)
    {
        auto obj = new juce::DynamicObject();
        obj->setProperty ("app", getAppName());
        obj->setProperty ("pid", getProcessId());
        obj->setProperty ("port", boundPort);
        obj->setProperty ("juce", juce::String (JUCE_MAJOR_VERSION) + "." + juce::String (JUCE_MINOR_VERSION) + "." + juce::String (JUCE_BUILDNUMBER));
        obj->setProperty ("os", juce::SystemStats::getOperatingSystemName());
        obj->setProperty ("windows", juce::Desktop::getInstance().getNumComponents());
        return Result (juce::var (obj));
    });

    addCommand ("commands", "List available commands", "{}", [this] (const juce::var&)
    {
        juce::Array<juce::var> list;
        juce::ScopedLock sl (commandLock);
        for (auto& [name, cmd] : commands)
        {
            auto obj = new juce::DynamicObject();
            obj->setProperty ("name", name);
            obj->setProperty ("description", cmd.description);
            obj->setProperty ("args", juce::JSON::parse (cmd.argsSpec));
            obj->setProperty ("async", cmd.asyncHandler != nullptr);
            list.add (juce::var (obj));
        }
        return Result (list);
    });

    addCommand ("windows", "List top level windows", "{}", [this] (const juce::var&)
    {
        juce::Array<juce::var> list;
        auto& desktop = juce::Desktop::getInstance();
        for (int i = 0; i < desktop.getNumComponents(); i++)
        {
            if (auto c = desktop.getComponent (i))
            {
                auto v = describeComponent (*c, false);
                if (auto obj = v.getDynamicObject())
                {
                    obj->setProperty ("hasPeer", c->getPeer() != nullptr);
                    obj->setProperty ("peerFocused", c->getPeer() != nullptr && c->getPeer()->isFocused());
                    obj->setProperty ("modal", c->isCurrentlyModal());
                    obj->setProperty ("alwaysOnTop", c->isAlwaysOnTop());
                }
                list.add (v);
            }
        }
        return Result (list);
    });

    addCommand ("tree", "Component hierarchy as nested json",
                R"({"target":"string: selector of the subtree root, default all windows","depth":"int: levels to descend, default unlimited","visibleOnly":"bool: skip hidden components, default true","maxNodes":"int: default 5000"})",
                [this] (const juce::var& args)
    {
        auto depth = int (getArg (args, "depth", -1));
        auto visibleOnly = bool (getArg (args, "visibleOnly", true));
        auto maxNodes = int (getArg (args, "maxNodes", 5000));
        auto selector = getArg (args, "target").toString();

        juce::Array<juce::var> list;
        auto roots = findComponents (selector, visibleOnly);
        if (roots.isEmpty())
            return Result::fail ("No component matches '" + selector + "'");

        for (auto r : roots)
            list.add (describeTree (*r, depth, visibleOnly, maxNodes));

        return Result (list.size() == 1 ? list[0] : juce::var (list));
    });

    addCommand ("find", "Find components matching a selector",
                R"({"selector":"string!: see selector syntax","visibleOnly":"bool: default true","limit":"int: default 100"})",
                [this] (const juce::var& args)
    {
        auto selector = getArg (args, "selector").toString();
        auto visibleOnly = bool (getArg (args, "visibleOnly", true));
        auto limit = int (getArg (args, "limit", 100));

        juce::Array<juce::var> list;
        for (auto c : findComponents (selector, visibleOnly))
        {
            if (list.size() >= limit)
                break;
            list.add (describeComponent (*c, false));
        }
        return Result (list);
    });

    addCommand ("describe", "Everything known about one component",
                R"({"target":"string!: selector"})",
                [this, resolveTarget] (const juce::var& args)
    {
        juce::Component* c = nullptr;
        if (auto err = resolveTarget (args, true, c); err.isNotEmpty())
            return Result::fail (err);
        return Result (describeComponent (*c, true));
    });

    addCommand ("at", "Component under a screen point and its ancestors",
                R"({"x":"int!: screen x","y":"int!: screen y"})",
                [this] (const juce::var& args)
    {
        juce::Point<int> p (int (getArg (args, "x")), int (getArg (args, "y")));
        auto c = getComponentAt (p);
        if (c == nullptr)
            return Result::fail ("Nothing at " + p.toString());

        juce::Array<juce::var> chain;
        for (auto comp = c; comp != nullptr; comp = comp->getParentComponent())
        {
            auto v = describeComponent (*comp, false);
            v.getDynamicObject()->setProperty ("local", pointToVar (comp->getLocalPoint (nullptr, p)));
            chain.add (v);
        }
        return Result (chain);
    });

    //==============================================================================
    addCommand ("screenshot", "Render a window or component to a png",
                R"({"target":"string: selector, default first window","file":"string: output path, default temp","scale":"number: default 1","region":"array: [x,y,w,h] within target","highlight":"array: selectors to outline","labels":"bool: label highlights with their index, default true","base64":"bool: also return png data"})",
                [this] (const juce::var& args)
    {
        auto selector = getArg (args, "target").toString();
        auto scale = float (getArg (args, "scale", 1.0));

        juce::Component* target = nullptr;
        if (selector.isNotEmpty())
        {
            target = findComponent (selector, true);
            if (target == nullptr)
                return Result::fail ("No component matches '" + selector + "'");
        }
        else
        {
            auto& desktop = juce::Desktop::getInstance();
            for (int i = 0; i < desktop.getNumComponents() && target == nullptr; i++)
                if (auto c = desktop.getComponent (i); c->isVisible() && c->getPeer() != nullptr)
                    target = c;
            if (target == nullptr)
                return Result::fail ("No visible window");
        }

        auto area = target->getLocalBounds();
        juce::Rectangle<int> region;
        if (varToRect (getArg (args, "region"), region))
            area = area.getIntersection (region);

        if (area.isEmpty())
            return Result::fail ("Nothing to capture");

        auto image = target->createComponentSnapshot (area, true, scale);
        if (image.isNull())
            return Result::fail ("Snapshot failed");

        auto highlight = getArg (args, "highlight");
        auto labels = bool (getArg (args, "labels", true));
        juce::Array<juce::var> highlighted;

        if (highlight.isArray())
        {
            juce::Graphics g (image);
            const juce::Colour colours[] = { juce::Colours::red, juce::Colours::lime, juce::Colours::deepskyblue, juce::Colours::orange, juce::Colours::magenta, juce::Colours::yellow };
            int idx = 0;

            for (auto& sel : *highlight.getArray())
            {
                for (auto c : findComponents (sel.toString(), true, target))
                {
                    auto r = target->getLocalArea (c, c->getLocalBounds()).toFloat();
                    r.translate (float (-area.getX()), float (-area.getY()));
                    r *= scale;

                    drawHighlight (g, r, labels ? juce::String (idx) : juce::String(), colours[idx % 6]);

                    auto v = describeComponent (*c, false);
                    v.getDynamicObject()->setProperty ("index", idx);
                    highlighted.add (v);
                    idx++;
                }
            }
        }

        juce::File file;
        auto path = getArg (args, "file").toString();
        if (path.isNotEmpty())
        {
            file = juce::File::getCurrentWorkingDirectory().getChildFile (path);
        }
        else
        {
            auto dir = getDiscoveryDirectory().getChildFile ("screenshots");
            dir.createDirectory();
            file = dir.getChildFile (getAppName().retainCharacters ("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") + "_" + juce::String (++screenshotCounter) + ".png");
        }

        file.getParentDirectory().createDirectory();
        file.deleteFile();

        juce::MemoryOutputStream pngData;
        juce::PNGImageFormat png;
        if (! png.writeImageToStream (image, pngData))
            return Result::fail ("Could not encode png");

        if (! file.replaceWithData (pngData.getData(), pngData.getDataSize()))
            return Result::fail ("Could not write " + file.getFullPathName());

        auto obj = new juce::DynamicObject();
        obj->setProperty ("file", file.getFullPathName());
        obj->setProperty ("width", image.getWidth());
        obj->setProperty ("height", image.getHeight());
        obj->setProperty ("scale", scale);
        obj->setProperty ("target", getComponentPath (*target));
        obj->setProperty ("screen", rectToVar (target->localAreaToGlobal (area)));
        if (! highlighted.isEmpty())
            obj->setProperty ("highlighted", highlighted);
        if (bool (getArg (args, "base64", false)))
            obj->setProperty ("base64", juce::Base64::toBase64 (pngData.getData(), pngData.getDataSize()));

        return Result (juce::var (obj));
    });

    //==============================================================================
    addAsyncCommand ("click", "Click a component or screen point",
                     R"({"target":"string: selector","x":"number: screen x, or offset within target","y":"number: screen y, or offset within target","button":"string: left|right|middle, default left","count":"int: 1 or 2, default 1","mods":"array: shift|ctrl|alt|cmd"})",
                     [resolvePoint] (Context& ctx, const juce::var& args)
    {
        auto buttonMod = buttonModifier (getArg (args, "button", "left").toString());
        auto count = juce::jlimit (1, 3, int (getArg (args, "count", 1)));
        auto mods = parseMods (getArg (args, "mods"));

        juce::Point<float> pos;
        juce::Component* comp = nullptr;

        auto res = ctx.runOnMessageThread ([&]
        {
            if (auto err = resolvePoint (args, comp, pos); err.isNotEmpty())
                return Result::fail (err);
            if (! injectMouse (pos, mods, comp))
                return Result::fail ("No window at " + pos.toString());
            return Result (juce::var (true));
        });
        if (! res.ok())
            return res;

        for (int i = 0; i < count; i++)
        {
            ctx.sleep (10);
            res = ctx.runOnMessageThread ([&] { return Result (injectMouse (pos, mods.withFlags (buttonMod), nullptr)); });
            ctx.sleep (20);
            res = ctx.runOnMessageThread ([&] { return Result (injectMouse (pos, mods, nullptr)); });
        }

        auto obj = new juce::DynamicObject();
        obj->setProperty ("screen", pointToVar (pos.toInt()));
        if (comp != nullptr)
            obj->setProperty ("target", getComponentPath (*comp));
        return Result (juce::var (obj));
    });

    addAsyncCommand ("drag", "Press, move and release",
                     R"({"target":"string: selector to start from","x":"number: start screen x, or offset within target","y":"number: start screen y, or offset within target","dx":"number: distance to move horizontally","dy":"number: distance to move vertically","toX":"number: end screen x, alternative to dx","toY":"number: end screen y, alternative to dy","button":"string: left|right|middle","mods":"array: shift|ctrl|alt|cmd","steps":"int: default 12","durationMs":"int: default 250"})",
                     [resolvePoint] (Context& ctx, const juce::var& args)
    {
        auto buttonMod = buttonModifier (getArg (args, "button", "left").toString());
        auto mods = parseMods (getArg (args, "mods"));
        auto steps = juce::jmax (1, int (getArg (args, "steps", 12)));
        auto duration = juce::jmax (0, int (getArg (args, "durationMs", 250)));

        juce::Point<float> start, end;
        juce::Component* comp = nullptr;

        auto res = ctx.runOnMessageThread ([&]
        {
            if (auto err = resolvePoint (args, comp, start); err.isNotEmpty())
                return Result::fail (err);

            end = start;
            if (args.hasProperty ("toX")) end.x = float (args["toX"]); else end.x += float (getArg (args, "dx", 0.0));
            if (args.hasProperty ("toY")) end.y = float (args["toY"]); else end.y += float (getArg (args, "dy", 0.0));

            if (! injectMouse (start, mods, comp))
                return Result::fail ("No window at " + start.toString());
            return Result (juce::var (true));
        });
        if (! res.ok())
            return res;

        ctx.sleep (10);
        ctx.runOnMessageThread ([&] { return Result (injectMouse (start, mods.withFlags (buttonMod), nullptr)); });

        for (int i = 1; i <= steps; i++)
        {
            ctx.sleep (duration / steps);
            auto p = start + (end - start) * (float (i) / float (steps));
            ctx.runOnMessageThread ([&] { return Result (injectMouse (p, mods.withFlags (buttonMod), nullptr)); });
        }

        ctx.sleep (10);
        ctx.runOnMessageThread ([&] { return Result (injectMouse (end, mods, nullptr)); });

        auto obj = new juce::DynamicObject();
        obj->setProperty ("from", pointToVar (start.toInt()));
        obj->setProperty ("to", pointToVar (end.toInt()));
        return Result (juce::var (obj));
    });

    addAsyncCommand ("mouse", "Low level mouse control: move, down or up",
                     R"({"action":"string!: move|down|up","target":"string: selector","x":"number: screen x, or offset within target","y":"number: screen y, or offset within target","button":"string: left|right|middle","mods":"array: shift|ctrl|alt|cmd"})",
                     [resolvePoint] (Context& ctx, const juce::var& args)
    {
        auto action = getArg (args, "action", "move").toString().toLowerCase();
        auto buttonMod = buttonModifier (getArg (args, "button", "left").toString());
        auto mods = parseMods (getArg (args, "mods"));

        return ctx.runOnMessageThread ([&]
        {
            juce::Point<float> pos;
            juce::Component* comp = nullptr;
            if (auto err = resolvePoint (args, comp, pos); err.isNotEmpty())
                return Result::fail (err);

            bool ok = false;
            if (action == "down")       ok = injectMouse (pos, mods.withFlags (buttonMod), comp);
            else if (action == "up")    ok = injectMouse (pos, mods, comp);
            else                        ok = injectMouse (pos, mods, comp);

            return ok ? Result (pointToVar (pos.toInt())) : Result::fail ("No window at " + pos.toString());
        });
    });

    addAsyncCommand ("wheel", "Scroll the mouse wheel over a component or point",
                     R"({"target":"string: selector","x":"number: screen x, or offset within target","y":"number: screen y, or offset within target","dx":"number: horizontal delta","dy":"number: vertical delta, a notch is about 0.1","mods":"array: shift|ctrl|alt|cmd"})",
                     [resolvePoint] (Context& ctx, const juce::var& args)
    {
        auto mods = parseMods (getArg (args, "mods"));
        auto dx = float (getArg (args, "dx", 0.0));
        auto dy = float (getArg (args, "dy", 0.0));

        return ctx.runOnMessageThread ([&]
        {
            juce::Point<float> pos;
            juce::Component* comp = nullptr;
            if (auto err = resolvePoint (args, comp, pos); err.isNotEmpty())
                return Result::fail (err);

            return injectWheel (pos, dx, dy, mods, comp) ? Result (pointToVar (pos.toInt())) : Result::fail ("No window at " + pos.toString());
        });
    });

    addCommand ("key", "Send a key press, e.g. 'return', 'escape', 'cmd+s', 'shift+tab', 'F1', 'a'",
                R"({"key":"string!: KeyPress description","target":"string: selector whose window receives the key, default focused window"})",
                [] (const juce::var& args)
    {
        auto desc = getArg (args, "key").toString().replace ("+", " + ");
        auto kp = juce::KeyPress::createFromDescription (desc);
        if (! kp.isValid())
            return Result::fail ("Can't parse key '" + desc + "'");

        juce::Component* target = nullptr;
        auto selector = getArg (args, "target").toString();
        if (selector.isNotEmpty())
        {
            target = findComponent (selector, true);
            if (target == nullptr)
                return Result::fail ("No component matches '" + selector + "'");
        }

        return injectKey (kp, target) ? Result (kp.getTextDescription()) : Result::fail ("No window to send keys to");
    });

    addAsyncCommand ("type", "Type a string one character at a time",
                     R"({"text":"string!: text to type","target":"string: selector whose window receives the keys","delayMs":"int: between characters, default 5"})",
                     [] (Context& ctx, const juce::var& args)
    {
        auto text = getArg (args, "text").toString();
        auto delay = int (getArg (args, "delayMs", 5));
        auto selector = getArg (args, "target").toString();

        int sent = 0;
        for (auto t = text.getCharPointer(); ! t.isEmpty(); ++t)
        {
            auto ch = *t;
            auto res = ctx.runOnMessageThread ([&]
            {
                juce::Component* target = selector.isNotEmpty() ? findComponent (selector, true) : nullptr;

                int code = int (ch);
                if (ch == '\n')                                 code = juce::KeyPress::returnKey;
                else if (ch == '\t')                            code = juce::KeyPress::tabKey;
                else if (juce::CharacterFunctions::isLetter (ch)) code = int (juce::CharacterFunctions::toUpperCase (ch));

                juce::ModifierKeys mods;
                if (juce::CharacterFunctions::isUpperCase (ch))
                    mods = juce::ModifierKeys::shiftModifier;

                return injectKey (juce::KeyPress (code, mods, ch), target) ? Result (juce::var (true)) : Result::fail ("No window to send keys to");
            });

            if (! res.ok())
                return res;

            sent++;
            if (! ctx.sleep (delay))
                break;
        }

        return Result (sent);
    });

    addCommand ("focus", "Give a component keyboard focus and bring its window to the front",
                R"({"target":"string!: selector"})",
                [resolveTarget] (const juce::var& args)
    {
        juce::Component* c = nullptr;
        if (auto err = resolveTarget (args, false, c); err.isNotEmpty())
            return Result::fail (err);

        if (auto peer = c->getPeer())
        {
            peer->toFront (true);
            peer->grabFocus();
        }
        c->grabKeyboardFocus();
        return Result (c->hasKeyboardFocus (true));
    });

    //==============================================================================
    addCommand ("get", "Read the value of a slider, button, combo box, text editor or label",
                R"({"target":"string!: selector"})",
                [resolveTarget] (const juce::var& args)
    {
        juce::Component* c = nullptr;
        if (auto err = resolveTarget (args, true, c); err.isNotEmpty())
            return Result::fail (err);

        auto v = getComponentValue (*c);
        if (v.isVoid())
            return Result::fail ("No value on " + getClassName (*c));
        return Result (v);
    });

    addCommand ("set", "Set the value of a slider, button, combo box, text editor or label, with notifications",
                R"({"target":"string!: selector","value":"any!: number, bool, string or array"})",
                [this, resolveTarget] (const juce::var& args)
    {
        juce::Component* c = nullptr;
        if (auto err = resolveTarget (args, true, c); err.isNotEmpty())
            return Result::fail (err);

        if (auto err = setComponentValue (*c, getArg (args, "value")); err.isNotEmpty())
            return Result::fail (err);

        return Result (describeComponent (*c, false));
    });

    addCommand ("press", "Trigger a button without going through the mouse",
                R"({"target":"string!: selector"})",
                [resolveTarget] (const juce::var& args)
    {
        juce::Component* c = nullptr;
        if (auto err = resolveTarget (args, true, c); err.isNotEmpty())
            return Result::fail (err);

        if (auto b = dynamic_cast<juce::Button*> (c))
        {
            b->triggerClick();
            return Result (juce::var (true));
        }

        if (auto handler = c->getAccessibilityHandler())
            if (handler->getActions().invoke (juce::AccessibilityActionType::press))
                return Result (juce::var (true));

        return Result::fail (getClassName (*c) + " is not pressable");
    });

    addCommand ("resize", "Resize a component, usually a window",
                R"({"target":"string: selector, default first window","width":"int!","height":"int!"})",
                [this] (const juce::var& args)
    {
        auto selector = getArg (args, "target", "/0").toString();
        auto c = findComponent (selector, true);
        if (c == nullptr)
            return Result::fail ("No component matches '" + selector + "'");

        auto w = int (getArg (args, "width", c->getWidth()));
        auto h = int (getArg (args, "height", c->getHeight()));

        if (auto rw = dynamic_cast<juce::ResizableWindow*> (c))
            rw->setSize (w, h);
        else
            c->setSize (w, h);

        return Result (describeComponent (*c, false));
    });

    //==============================================================================
    addAsyncCommand ("wait", "Wait for a component to exist, become visible or disappear",
                     R"({"selector":"string!: selector","state":"string: exists|visible|gone, default visible","timeoutMs":"int: default 5000","intervalMs":"int: default 50"})",
                     [this] (Context& ctx, const juce::var& args)
    {
        auto selector = getArg (args, "selector").toString();
        auto state = getArg (args, "state", "visible").toString().toLowerCase();
        auto timeout = int (getArg (args, "timeoutMs", 5000));
        auto interval = juce::jmax (10, int (getArg (args, "intervalMs", 50)));

        auto start = juce::Time::getMillisecondCounter();

        for (;;)
        {
            auto res = ctx.runOnMessageThread ([&]
            {
                auto c = findComponent (selector, false);
                bool satisfied = false;

                if (state == "gone")            satisfied = (c == nullptr || ! c->isShowing());
                else if (state == "exists")     satisfied = (c != nullptr);
                else                            satisfied = (c != nullptr && c->isShowing());

                if (! satisfied)
                    return Result (juce::var());
                return Result (c != nullptr ? describeComponent (*c, false) : juce::var (true));
            });

            if (! res.ok())
                return res;
            if (! res.value.isVoid())
                return res;

            if (int (juce::Time::getMillisecondCounter() - start) > timeout)
                return Result::fail ("Timed out waiting for '" + selector + "' to be " + state);

            if (! ctx.sleep (interval))
                return Result::fail ("Server stopping");
        }
    });

    addAsyncCommand ("sleep", "Pause for a while, letting the UI run",
                     R"({"ms":"int!: milliseconds"})",
                     [] (Context& ctx, const juce::var& args)
    {
        auto ms = int (getArg (args, "ms", 100));
        ctx.sleep (ms);
        return Result (ms);
    });

    addAsyncCommand ("log", "Return log lines captured since the last call",
                     R"({"clear":"bool: default true","tail":"int: only the last n lines"})",
                     [this] (Context&, const juce::var& args)
    {
        auto clear = bool (getArg (args, "clear", true));
        auto tail = int (getArg (args, "tail", 0));

        juce::ScopedLock sl (logLock);
        juce::Array<juce::var> list;
        int startIdx = tail > 0 ? juce::jmax (0, logLines.size() - tail) : 0;
        for (int i = startIdx; i < logLines.size(); i++)
            list.add (logLines[i]);

        if (clear)
            logLines.clear();

        return Result (list);
    });
}
