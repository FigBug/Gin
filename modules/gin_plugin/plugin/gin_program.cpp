void Program::loadProcessor (Processor& p)
{
    jassert (fullyLoaded);
    if (! fullyLoaded)
        return;

    for (auto pp : p.getPluginParameters())
        if (p.loadingState || ! p.isParamLocked (pp))
            pp->setUserValueNotifingHost (pp->getUserDefaultValue());

    auto inst = p.state.getChildWithName ("instance").createCopy();

    p.state.removeAllProperties (nullptr);
    p.state.removeAllChildren (nullptr);

    if (state.isValid())
        p.state.copyPropertiesAndChildrenFrom (state, nullptr);

    if (auto oldInst = p.state.getChildWithName ("instance"); oldInst.isValid())
        p.state.removeChild (oldInst, nullptr);

    if (inst.isValid())
        p.state.addChild (inst, 0, nullptr);

    for (const auto& s : states)
    {
        if (auto pp = p.getParameter (s.uid))
        {
            if (! pp->isMetaParameter())
                if (p.loadingState || ! p.isParamLocked (pp))
                    pp->setUserValueNotifingHost (s.value);
        }
        else
        {
            DBG("Unknown parameter: " + s.uid);
            jassertfalse;
        }
    }
}

void Program::saveProcessor (Processor& p)
{
    fullyLoaded = true;

    states.clear();

    if (p.state.isValid())
        state = p.state.createCopy();
    else
        state = {};

    auto& params = p.getPluginParameters();
    for (auto param : params)
        if (! param->isMetaParameter())
            if (! juce::approximatelyEqual (param->getUserDefaultValue(), param->getUserValue()))
                states.add (param->getState());
}

juce::File Program::getPresetFile (juce::File programDir)
{
    if (file.existsAsFile())
        return file;

    return programDir.getChildFile (juce::File::createLegalFileName (name) + ".xml");
}

void Program::loadFromFile (juce::File f, bool loadFully)
{
    file = f;

    juce::XmlDocument doc (f);
    std::unique_ptr<juce::XmlElement> rootE (doc.getDocumentElement());
    if (rootE)
    {
        states.clear();

        name = rootE->getStringAttribute ("name").trim();
        author = rootE->getStringAttribute ("author").trim();
        tags = juce::StringArray::fromTokens (rootE->getStringAttribute ("tags"), " ", "");

        if (loadFully)
        {
            fullyLoaded = true;

            if (auto s = rootE->getChildByName ("state"))
            {
                state = juce::ValueTree::fromXml (*s);
            }
            else
            {
                auto stateXml = rootE->getStringAttribute ("valueTree");
                state = juce::ValueTree::fromXml (stateXml);
            }

            auto paramE = rootE->getChildByName ("param");
            while (paramE)
            {
                juce::String uid = paramE->getStringAttribute ("uid");
                float  val = (float) paramE->getDoubleAttribute ("val");

                Parameter::ParamState s;
                s.uid   = uid;
                s.value = val;
                states.add (s);

                paramE = paramE->getNextElementWithTagName ("param");
            }
        }
    }
}

void Program::saveToDir (juce::File f)
{
    jassert(fullyLoaded);
    if (! fullyLoaded)
        return;

    std::unique_ptr<juce::XmlElement> rootE (new juce::XmlElement ("state"));

    rootE->setAttribute("name", name);
    rootE->setAttribute ("author", author);
    rootE->setAttribute ("tags", tags.joinIntoString (" "));

    if (auto xml = state.createXml())
        rootE->addChildElement (xml.release());

    for (const auto& s : states)
    {
        auto paramE = new juce::XmlElement ("param");

        paramE->setAttribute ("uid", s.uid);
        paramE->setAttribute ("val", s.value);

        rootE->addChildElement (paramE);
    }

    juce::File xmlFile = f.getChildFile (juce::File::createLegalFileName (name.trim()) + ".xml");
    xmlFile.replaceWithText (rootE->toString());
}

void Program::deleteFromDir (juce::File f)
{
    getPresetFile (f).deleteFile();
    file = juce::File();
}
