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
        XmlDocument treeDoc (valueTree);
        if (std::unique_ptr<XmlElement> vtE = treeDoc.getDocumentElement())
        {
            auto srcState = ValueTree::fromXml (*vtE);
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

void Program::loadFromFile (File f)
{
    XmlDocument doc (f);
    std::unique_ptr<XmlElement> rootE (doc.getDocumentElement());
    if (rootE)
    {
        states.clear();

        name = rootE->getStringAttribute ("name");
        author = rootE->getStringAttribute ("author");
        tags = StringArray::fromTokens (rootE->getStringAttribute ("tags"), " ", "");

        valueTree = rootE->getStringAttribute ("valueTree");

        XmlElement* paramE = rootE->getChildByName ("param");
        while (paramE)
        {
            String uid = paramE->getStringAttribute ("uid");
            float  val = (float) paramE->getDoubleAttribute ("val");

            Parameter::ParamState state;
            state.uid   = uid;
            state.value = val;
            states.add (state);

            paramE = paramE->getNextElementWithTagName("param");
        }
    }
}

void Program::saveToDir (File f)
{
    std::unique_ptr<XmlElement> rootE (new XmlElement ("state"));

    rootE->setAttribute("name", name);
    rootE->setAttribute ("author", author);
    rootE->setAttribute ("tags", tags.joinIntoString (" "));
    rootE->setAttribute ("valueTree", valueTree);

    for (Parameter::ParamState state : states)
    {
        XmlElement* paramE = new XmlElement ("param");

        paramE->setAttribute ("uid", state.uid);
        paramE->setAttribute ("val", state.value);

        rootE->addChildElement (paramE);
    }

    File xmlFile = f.getChildFile (File::createLegalFileName (name) + ".xml");
    xmlFile.replaceWithText (rootE->toString());
}

void Program::deleteFromDir (File f)
{
    f.getChildFile (File::createLegalFileName (name) + ".xml").deleteFile();
}
