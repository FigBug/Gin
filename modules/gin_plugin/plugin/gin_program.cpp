void Program::loadProcessor (Processor& p)
{
    for (auto pp : p.getPluginParameters())
        pp->setUserValueNotifingHost (pp->getUserDefaultValue());

    int w = p.state.getProperty ("width", -1);
    int h = p.state.getProperty ("height", -1);

    p.state.removeAllProperties (nullptr);
    p.state.removeAllChildren (nullptr);

    if (valueTree.isNotEmpty())
    {
        juce::XmlDocument treeDoc (valueTree);
        if (std::unique_ptr<juce::XmlElement> vtE = treeDoc.getDocumentElement())
        {
            auto srcState = juce::ValueTree::fromXml (*vtE);
            p.state.copyPropertiesAndChildrenFrom (srcState, nullptr);
        }
    }

    if (w != -1) p.state.setProperty ("width", w, nullptr);
    if (h != -1) p.state.setProperty ("height", h, nullptr);

    for (Parameter::ParamState state : states)
        if (auto pp = p.getParameter (state.uid))
            if (! pp->isMetaParameter())
                pp->setUserValueNotifingHost (state.value);
}

void Program::saveProcessor (Processor& p)
{
    states.clear();

    if (p.state.isValid())
        valueTree = p.state.toXmlString();

    auto& params = p.getPluginParameters();
    for (auto param : params)
        if (! param->isMetaParameter())
            states.add (param->getState());
}

void Program::loadFromFile (juce::File f)
{
    juce::XmlDocument doc (f);
    std::unique_ptr<juce::XmlElement> rootE (doc.getDocumentElement());
    if (rootE)
    {
        states.clear();

        name = rootE->getStringAttribute ("name");
        author = rootE->getStringAttribute ("author");
        tags = juce::StringArray::fromTokens (rootE->getStringAttribute ("tags"), " ", "");

        valueTree = rootE->getStringAttribute ("valueTree");

        juce::XmlElement* paramE = rootE->getChildByName ("param");
        while (paramE)
        {
            juce::String uid = paramE->getStringAttribute ("uid");
            float  val = (float) paramE->getDoubleAttribute ("val");

            Parameter::ParamState state;
            state.uid   = uid;
            state.value = val;
            states.add (state);

            paramE = paramE->getNextElementWithTagName("param");
        }
    }
}

void Program::saveToDir (juce::File f)
{
    std::unique_ptr<juce::XmlElement> rootE (new juce::XmlElement ("state"));

    rootE->setAttribute("name", name);
    rootE->setAttribute ("author", author);
    rootE->setAttribute ("tags", tags.joinIntoString (" "));
    rootE->setAttribute ("valueTree", valueTree);

    for (Parameter::ParamState state : states)
    {
        juce::XmlElement* paramE = new juce::XmlElement ("param");

        paramE->setAttribute ("uid", state.uid);
        paramE->setAttribute ("val", state.value);

        rootE->addChildElement (paramE);
    }

    juce::File xmlFile = f.getChildFile (juce::File::createLegalFileName (name) + ".xml");
    xmlFile.replaceWithText (rootE->toString());
}

void Program::deleteFromDir (juce::File f)
{
    f.getChildFile (juce::File::createLegalFileName (name) + ".xml").deleteFile();
}
