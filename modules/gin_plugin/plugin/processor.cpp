
//==============================================================================
GinProcessor::GinProcessor()
{
    loadAllPrograms();

    state = ValueTree (Identifier ("state"));

    stateUpdated();
}

GinProcessor::~GinProcessor()
{
}

std::unique_ptr<PropertiesFile> GinProcessor::getSettings()
{
#if JUCE_MAC
    File dir = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Preferences").getChildFile ("SocaLabs");
#else
    File dir = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("SocaLabs");
#endif
    dir.createDirectory();

    PropertiesFile::Options options;

    return std::make_unique<PropertiesFile> (dir.getChildFile ("plugin_settings.xml"), options);
}

//==============================================================================
void GinProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (auto p : getPluginParameters())
        p->prepareToPlay (sampleRate, samplesPerBlock);
}

void GinProcessor::reset()
{
    for (auto p : getPluginParameters())
        p->reset();
}

bool GinProcessor::isSmoothing()
{
    for (auto p : allParameters)
        if (p->isSmoothingActive())
            return true;

    return false;
}

void GinProcessor::addPluginParameter (Parameter* p)
{
    addParameter (p);
    allParameters.add (p);

    parameterMap[p->getUid()] = p;
}

Parameter* GinProcessor::createParam (String uid, String name, String shortName, String label,
                                      NormalisableRange<float> range, float defaultValue,
                                      SmoothingType st,
                                      std::function<String (const Parameter&, float)> textFunction)
{
    Parameter* p = nullptr;

    if (st.time > 0.0f)
    {
        if (st.type == SmoothingType::linear)
        {
            auto sp = new SmoothedParameter<ValueSmoother<float>> (*this, uid, name, shortName, label, range, defaultValue, textFunction);
            sp->setSmoothingTime (st.time);
            p = sp;
        }
        else if (st.type == SmoothingType::eased)
        {
            auto sp = new SmoothedParameter<EasedValueSmoother<float>> (*this, uid, name, shortName, label, range, defaultValue, textFunction);
            sp->setSmoothingTime (st.time);
            p = sp;
        }
    }
    else
    {
        p = new Parameter (*this, uid, name, shortName, label, range, defaultValue, textFunction);
    }

    jassert (p != nullptr);
    return p;
}

Parameter* GinProcessor::addIntParam (String uid, String name, String shortName, String label,
                                      NormalisableRange<float> range, float defaultValue,
                                      SmoothingType st,
                                      std::function<String (const Parameter&, float)> textFunction)
{
    if (auto p = createParam (uid, name, shortName, label, range, defaultValue, st, textFunction))
    {
        internalParameters.add (p);
        allParameters.add (p);
        parameterMap[p->getUid()] = p;
        return p;
    }
    return nullptr;
}

Parameter* GinProcessor::addExtParam (String uid, String name, String shortName, String label,
                                      NormalisableRange<float> range, float defaultValue,
                                      SmoothingType st,
                                      std::function<String (const Parameter&, float)> textFunction)
{
    if (auto p = createParam (uid, name, shortName, label, range, defaultValue, st, textFunction))
    {
        addParameter (p);
        allParameters.add (p);
        parameterMap[p->getUid()] = p;
        return p;
    }
    return nullptr;
}

Parameter* GinProcessor::getParameter (const String& uid)
{
    if (parameterMap.find (uid) != parameterMap.end())
        return parameterMap[uid];

    return nullptr;
}

float GinProcessor::parameterValue (const String& uid)
{
    if (parameterMap.find (uid) != parameterMap.end())
        return parameterMap[uid]->getUserValue();

    return 0;
}

int GinProcessor::parameterIntValue (const String& uid)
{
    if (parameterMap.find (uid) != parameterMap.end())
        return int (parameterMap[uid]->getUserValue());

    return 0;
}

bool GinProcessor::parameterBoolValue (const String& uid)
{
    if (parameterMap.find (uid) != parameterMap.end())
        return parameterMap[uid]->getUserValue() > 0;

    return false;
}

const Array<Parameter*>& GinProcessor::getPluginParameters()
{
    return allParameters;
}

//==============================================================================
const String GinProcessor::getName() const
{
   #ifdef JucePlugin_Name
    return JucePlugin_Name;
   #else
    return {};
   #endif
}

bool GinProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GinProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double GinProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GinProcessor::getNumPrograms()
{
    return programs.size();
}

int GinProcessor::getCurrentProgram()
{
    return currentProgram;
}

void GinProcessor::setCurrentProgram (int index)
{
    if (index == getCurrentProgram())
        return;
    if ((Time::getCurrentTime() - lastStateLoad) < RelativeTime::seconds (2))
        return;

    if (index >= 0 && index < programs.size())
    {
        programs[index]->loadProcessor (this);
        currentProgram = index;

        updateHostDisplay();
        sendChangeMessage();
        stateUpdated();
    }
}

const String GinProcessor::getProgramName (int index)
{
    return programs[index]->name;
}

void GinProcessor::changeProgramName (int index, const String& newName)
{
    programs[index]->deleteFromDir (getProgramDirectory());
    programs[index]->name = newName;
    programs[index]->saveToDir (getProgramDirectory());

    updateHostDisplay();
    sendChangeMessage();
}

void GinProcessor::loadAllPrograms()
{
    programs.clear();

    // create the default program
    auto defaultProgram = new GinProgram();
    defaultProgram->name = "Default";
    defaultProgram->saveProcessor (this);

    programs.add (defaultProgram);

    // load programs from disk
    File dir = getProgramDirectory();

    Array<File> programFiles;
    dir.findChildFiles (programFiles, File::findFiles, false, "*.xml");
    programFiles.sort();

    for (File f : programFiles)
    {
        auto program = new GinProgram();
        program->loadFromFile (f);
        programs.add (program);
    }
}

void GinProcessor::extractProgram (const String& name, const MemoryBlock& data)
{
    File dir = getProgramDirectory();
    auto f = dir.getChildFile (name);
    if (! f.existsAsFile())
    {
        f.replaceWithData (data.getData(), data.getSize());

        auto program = new GinProgram();
        program->loadFromFile (f);
        programs.add (program);
    }
}

void GinProcessor::saveProgram (String name)
{
    updateState();

    for (int i = programs.size(); --i >= 0;)
        if (programs[i]->name == name)
            deleteProgram (i);

    GinProgram* newProgram = new GinProgram();
    newProgram->name = name;
    newProgram->saveProcessor (this);
    newProgram->saveToDir (getProgramDirectory());

    programs.add (newProgram);
    currentProgram = programs.size() - 1;

    updateHostDisplay();
    sendChangeMessage();
}

void GinProcessor::deleteProgram (int index)
{
    programs[index]->deleteFromDir (getProgramDirectory());
    programs.remove (index);
    if (index <= currentProgram)
        currentProgram--;

    updateHostDisplay();
    sendChangeMessage();
}

File GinProcessor::getProgramDirectory()
{
  #ifdef JucePlugin_Name
   #if JUCE_MAC
    File dir = File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile("Application Support/com.socalabs/" JucePlugin_Name "/programs");
   #else
    File dir = File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile("com.socalabs/" JucePlugin_Name "/programs");
   #endif
  #else
    // Shouldn't be using processor in something that isn't a plugin
    jassertfalse;
    File dir;
  #endif

    if (!dir.isDirectory())
        dir.createDirectory();
    return dir;
}

//==============================================================================

void GinProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    updateState();

    std::unique_ptr<XmlElement> rootE (new XmlElement ("state"));

    if (state.isValid())
        rootE->setAttribute ("valueTree", state.toXmlString());

    rootE->setAttribute ("program", currentProgram);

    for (auto p : getPluginParameters())
    {
        if (! p->isMetaParameter())
        {
            auto pstate = p->getState();

            auto paramE = new XmlElement ("param");

            paramE->setAttribute ("uid", pstate.uid);
            paramE->setAttribute ("val", pstate.value);

            rootE->addChildElement (paramE);
        }
    }

    MemoryOutputStream os (destData, true);
    auto text = rootE->toString();
    os.write (text.toRawUTF8(), text.getNumBytesAsUTF8());
}

void GinProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    XmlDocument doc (String::fromUTF8 ((const char*)data, sizeInBytes));
    std::unique_ptr<XmlElement> rootE (doc.getDocumentElement());
    if (rootE)
    {
        if (rootE->hasAttribute ("valueTree"))
        {
            String xml = rootE->getStringAttribute ("valueTree");
            XmlDocument treeDoc (xml);
            if (std::unique_ptr<XmlElement> vtE = treeDoc.getDocumentElement())
            {
                auto srcState = ValueTree::fromXml (*vtE);
                state.removeAllProperties (nullptr);
                state.removeAllChildren (nullptr);
                state.copyPropertiesAndChildrenFrom (srcState, nullptr);
            }
        }

        currentProgram = rootE->getIntAttribute ("program");

        XmlElement* paramE = rootE->getChildByName ("param");
        while (paramE)
        {
            String uid = paramE->getStringAttribute ("uid");
            float  val = paramE->getStringAttribute ("val").getFloatValue();

            if (auto p = getParameter (uid))
            {
                if (! p->isMetaParameter())
                    p->setUserValue (val);
            }

            paramE = paramE->getNextElementWithTagName ("param");
        }
    }
    stateUpdated();

    lastStateLoad = Time::getCurrentTime();
}
