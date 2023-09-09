void Program::loadProcessor (Processor& p)
{
    jassert (fullyLoaded);
    if (! fullyLoaded)
        return;

    for (auto pp : p.getPluginParameters())
        pp->setUserValueNotifingHost (pp->getUserDefaultValue());

    int w = p.state.getProperty ("width", -1);
    int h = p.state.getProperty ("height", -1);

    p.state.removeAllProperties (nullptr);
    p.state.removeAllChildren (nullptr);

    if (state.isValid())
        p.state.copyPropertiesAndChildrenFrom (state, nullptr);

    if (w != -1) p.state.setProperty ("width", w, nullptr);
    if (h != -1) p.state.setProperty ("height", h, nullptr);

    for (const auto& s : states)
        if (auto pp = p.getParameter (s.uid))
            if (! pp->isMetaParameter())
                pp->setUserValueNotifingHost (s.value);
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
    return programDir.getChildFile (juce::File::createLegalFileName (name) + ".xml");
}

void Program::loadFromFile (juce::File f, bool loadFully)
{
    juce::XmlDocument doc (f);
    std::unique_ptr<juce::XmlElement> rootE (doc.getDocumentElement());
    if (rootE)
    {
        states.clear();

        name = rootE->getStringAttribute ("name");
        author = rootE->getStringAttribute ("author");
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

    juce::File xmlFile = f.getChildFile (juce::File::createLegalFileName (name) + ".xml");
    xmlFile.replaceWithText (rootE->toString());
}

void Program::deleteFromDir (juce::File f)
{
    getPresetFile (f).deleteFile();
}
