//==============================================================================
ProcessorOptions::ProcessorOptions()
{
   #ifdef JucePlugin_Name
    pluginName = JucePlugin_Name;
   #endif

   #ifdef JucePlugin_VersionString
    pluginVersion = JucePlugin_VersionString;
   #endif

   #ifdef JucePlugin_Manufacturer
    developer = JucePlugin_Manufacturer;
   #endif

   #ifdef JucePlugin_ManufacturerWebsite
    auto tokens = juce::StringArray::fromTokens (juce::URL (JucePlugin_ManufacturerWebsite).getDomain(), ".", "");
    std::reverse (tokens.strings.begin(), tokens.strings.end());
    devId = tokens.joinIntoString (".");

    url = JucePlugin_ManufacturerWebsite;
    urlTitle = juce::String ("Visit ") + url;
    updatesURL = juce::String (JucePlugin_ManufacturerWebsite) + "updates.xml";
   #endif

   #if JucePlugin_WantsMidiInput
    wantsMidi = true;
   #endif

   #if JucePlugin_ProducesMidiOutput
    makesMidi = true;
   #endif
}

//==============================================================================
Processor::Processor (bool init_, ProcessorOptions po)
    : processorOptions (po)
{
    lf = std::make_unique<gin::CopperLookAndFeel>();

    if (init_)
        init();
}

Processor::~Processor()
{
}

void Processor::init()
{
    state = juce::ValueTree (juce::Identifier ("state"));
    loadAllPrograms();
}

juce::PropertiesFile* Processor::getSettings()
{
    if (settings == nullptr)
    {
       #if JUCE_MAC
        auto dir = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory).getChildFile ("Preferences").getChildFile (processorOptions.developer);
       #else
        auto dir = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory).getChildFile (processorOptions.developer);
       #endif
        dir.createDirectory();
        
        juce::PropertiesFile::Options options;
        
        settings = std::make_unique<juce::PropertiesFile> (dir.getChildFile ("plugin_settings.xml"), options);
    }
    return settings.get();
}

//==============================================================================
void Processor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (auto p : getPluginParameters())
        p->prepareToPlay (sampleRate, samplesPerBlock);
}

void Processor::reset()
{
    for (auto p : getPluginParameters())
        p->reset();
}

bool Processor::isSmoothing()
{
    for (auto p : allParameters)
        if (p->isSmoothingActive())
            return true;

    return false;
}

void Processor::addPluginParameter (gin::Parameter* p)
{
   #if BUILD_INTERNAL_PLUGINS
    addHostedParameter (std::unique_ptr<gin::Parameter> (p));
   #else
    addParameter (p);
   #endif

    allParameters.add (p);

    parameterMap[p->getUid()] = p;
}

std::unique_ptr<gin::Parameter> Processor::createParam (juce::String uid, juce::String name, juce::String shortName, juce::String label,
                                                        juce::NormalisableRange<float> range, float defaultValue,
                                                        SmoothingType st,
                                                        std::function<juce::String (const gin::Parameter&, float)> textFunction)
{
    std::unique_ptr<gin::Parameter> p;

    if (st.time > 0.0f)
    {
        if (st.type == SmoothingType::linear)
        {
            auto sp = new gin::SmoothedParameter<ValueSmoother<float>> (*this, uid, name, shortName, label, range, defaultValue, textFunction);
            sp->setSmoothingTime (st.time);
            p = std::unique_ptr<gin::Parameter> (sp);
        }
        else if (st.type == SmoothingType::eased)
        {
            auto sp = new gin::SmoothedParameter<EasedValueSmoother<float>> (*this, uid, name, shortName, label, range, defaultValue, textFunction);
            sp->setSmoothingTime (st.time);
            p = std::unique_ptr<gin::Parameter> (sp);
        }
    }
    else
    {
        p = std::make_unique<gin::Parameter> (*this, uid, name, shortName, label, range, defaultValue, textFunction);
    }

    jassert (p != nullptr);
    return p;
}

gin::Parameter* Processor::addIntParam (juce::String uid, juce::String name, juce::String shortName, juce::String label,
                                        juce::NormalisableRange<float> range, float defaultValue,
                                        SmoothingType st,
                                        std::function<juce::String (const gin::Parameter&, float)> textFunction)
{
    if (auto p = createParam (uid, name, shortName, label, range, defaultValue, st, textFunction))
    {
        auto rawPtr = p.get();
        p->setInternal (true);
        allParameters.add (rawPtr);
        parameterMap[p->getUid()] = rawPtr;
        internalParameters.add (p.release());
        return rawPtr;
    }
    return nullptr;
}

gin::Parameter* Processor::addExtParam (juce::String uid, juce::String name, juce::String shortName, juce::String label,
                                        juce::NormalisableRange<float> range, float defaultValue,
                                        SmoothingType st,
                                        std::function<juce::String (const gin::Parameter&, float)> textFunction)
{
    if (auto p = createParam (uid, name, shortName, label, range, defaultValue, st, textFunction))
    {
        auto rawPtr = p.get();
        allParameters.add (rawPtr);
        parameterMap[p->getUid()] = rawPtr;
       #if BUILD_INTERNAL_PLUGINS
        addHostedParameter (std::move (p));
       #else
        addParameter (p.release());
       #endif
        return rawPtr;
    }
    return nullptr;
}

Parameter* Processor::getParameter (const juce::String& uid)
{
    if (parameterMap.find (uid) != parameterMap.end())
        return parameterMap[uid];

    return nullptr;
}

float Processor::parameterValue (const juce::String& uid)
{
    if (parameterMap.find (uid) != parameterMap.end())
        return parameterMap[uid]->getUserValue();

    return 0;
}

int Processor::parameterIntValue (const juce::String& uid)
{
    if (parameterMap.find (uid) != parameterMap.end())
        return int (parameterMap[uid]->getUserValue());

    return 0;
}

bool Processor::parameterBoolValue (const juce::String& uid)
{
    if (parameterMap.find (uid) != parameterMap.end())
        return parameterMap[uid]->getUserValue() > 0;

    return false;
}

const juce::Array<gin::Parameter*>& Processor::getPluginParameters()
{
    return allParameters;
}

//==============================================================================
const juce::String Processor::getName() const
{
    return processorOptions.pluginName;
}

bool Processor::acceptsMidi() const
{
    return processorOptions.wantsMidi;
}

bool Processor::producesMidi() const
{
    return processorOptions.makesMidi;
}

double Processor::getTailLengthSeconds() const
{
    return 0.0;
}

int Processor::getNumPrograms()
{
    if (maxPrograms == 0)
        maxPrograms = programs.size() + 50;

    return maxPrograms;
}

int Processor::getCurrentProgram()
{
    return currentProgram;
}

void Processor::setCurrentProgram (int index)
{
    if (index == getCurrentProgram())
        return;
    if ((juce::Time::getCurrentTime() - lastStateLoad) < juce::RelativeTime::seconds (2))
        return;

    if (index >= 0 && index < programs.size())
    {
        auto p = programs[index];

        if (! p->fullyLoaded )
            p->loadFromFile (p->getPresetFile (getProgramDirectory()), true);

        p->loadProcessor (*this);
        currentProgram = index;

        updateHostDisplay();
        sendChangeMessage();
        stateUpdated();
    }
}

Program* Processor::getProgram (const juce::String& name)
{
    for (auto p : programs)
        if (p->name == name)
            return p;

    return nullptr;
}

void Processor::setCurrentProgram (juce::String name)
{
    int index = 0;
    for (auto p : programs)
    {
        if (p->name == name)
        {
            if (! p->fullyLoaded )
                p->loadFromFile (p->getPresetFile (getProgramDirectory()), true);

            p->loadProcessor (*this);
            currentProgram = index;

            updateHostDisplay();
            sendChangeMessage();
            stateUpdated();
            return;
        }
        index++;
    }
}

const juce::String Processor::getProgramName (int index)
{
    if (auto p = programs[index])
        return p->name;

    return "----";
}

bool Processor::hasProgram (juce::String name)
{
    for (auto p : programs)
        if (p->name == name)
            return true;

    return false;
}

void Processor::changeProgramName (int index, const juce::String& newName)
{
    programs[index]->deleteFromDir (getProgramDirectory());
    programs[index]->name = newName;
    programs[index]->saveToDir (getProgramDirectory());

    updateHostDisplay();
    sendChangeMessage();
}

void Processor::loadAllPrograms()
{
    updateState();

    programs.clear();

    // create the default program
    auto defaultProgram = new Program();
    defaultProgram->name = "Default";
    defaultProgram->saveProcessor (*this);

    programs.add (defaultProgram);

    // load programs from disk
    juce::File dir = getProgramDirectory();

    juce::Array<juce::File> programFiles;
    dir.findChildFiles (programFiles, juce::File::findFiles, false, "*.xml");
    programFiles.sort();

    for (auto f : programFiles)
    {
        auto program = new Program();
        program->loadFromFile (f, false);
        programs.add (program);
    }
}

void Processor::extractProgram (const juce::String& name, const juce::MemoryBlock& data)
{
    juce::File dir = getProgramDirectory();
    auto f = dir.getChildFile (name);
    if (! f.existsAsFile())
    {
        f.replaceWithData (data.getData(), data.getSize());

        auto program = new Program();
        program->loadFromFile (f, false);
        programs.add (program);
    }
}

void Processor::saveProgram (juce::String name, juce::String author, juce::String tags)
{
    updateState();

    for (int i = programs.size(); --i >= 0;)
        if (programs[i]->name == name)
            deleteProgram (i);

    auto newProgram = new Program();
    newProgram->name = name;
    newProgram->author = author;
    newProgram->tags = juce::StringArray::fromTokens (tags, " ", "");
    newProgram->saveProcessor (*this);
    newProgram->saveToDir (getProgramDirectory());

    programs.add (newProgram);
    currentProgram = programs.size() - 1;

    updateHostDisplay();
    sendChangeMessage();
}

void Processor::deleteProgram (int index)
{
    programs[index]->deleteFromDir (getProgramDirectory());
    programs.remove (index);
    if (index <= currentProgram)
        currentProgram--;

    updateHostDisplay();
    sendChangeMessage();
}

juce::File Processor::getProgramDirectory()
{
   #if JUCE_MAC
    juce::File dir = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory).getChildFile ("Application Support/" + processorOptions.devId + "/" + processorOptions.pluginName + "/programs");
   #else
    juce::File dir = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory).getChildFile (processorOptions.devId + "/" + processorOptions.pluginName + "/programs");
   #endif

    if (! dir.isDirectory())
        dir.createDirectory();

    return dir;
}

//==============================================================================

void Processor::getStateInformation (juce::MemoryBlock& destData)
{
    updateState();

    std::unique_ptr<juce::XmlElement> rootE (new juce::XmlElement ("state"));

    if (state.isValid())
        rootE->addChildElement (state.createXml().release());

    rootE->setAttribute ("program", currentProgram);

    for (auto p : getPluginParameters())
    {
        if (! p->isMetaParameter())
        {
            auto pstate = p->getState();

            auto paramE = new juce::XmlElement ("param");

            paramE->setAttribute ("uid", pstate.uid);
            paramE->setAttribute ("val", pstate.value);

            rootE->addChildElement (paramE);
        }
    }

    juce::MemoryOutputStream os (destData, true);
    auto text = rootE->toString();
    os.write (text.toRawUTF8(), text.getNumBytesAsUTF8());
}

void Processor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::XmlDocument doc (juce::String::fromUTF8 ((const char*)data, sizeInBytes));
    std::unique_ptr<juce::XmlElement> rootE (doc.getDocumentElement());
    if (rootE)
    {
        if (auto s = rootE->getChildByName ("state"))
        {
            auto srcState = juce::ValueTree::fromXml (*s);
            state.removeAllProperties (nullptr);
            state.removeAllChildren (nullptr);
            state.copyPropertiesAndChildrenFrom (srcState, nullptr);
        }
        else if (rootE->hasAttribute ("valueTree"))
        {
            juce::String xml = rootE->getStringAttribute ("valueTree");
            juce::XmlDocument treeDoc (xml);
            if (std::unique_ptr<juce::XmlElement> vtE = treeDoc.getDocumentElement())
            {
                auto srcState = juce::ValueTree::fromXml (*vtE);
                state.removeAllProperties (nullptr);
                state.removeAllChildren (nullptr);
                state.copyPropertiesAndChildrenFrom (srcState, nullptr);
            }
        }

        currentProgram = rootE->getIntAttribute ("program");

        juce::XmlElement* paramE = rootE->getChildByName ("param");
        while (paramE)
        {
            juce::String uid = paramE->getStringAttribute ("uid");
            float  val = paramE->getStringAttribute ("val").getFloatValue();

            if (auto p = getParameter (uid))
                if (! p->isMetaParameter())
                    p->setUserValue (val);

            paramE = paramE->getNextElementWithTagName ("param");
        }
    }
    stateUpdated();

    lastStateLoad = juce::Time::getCurrentTime();
}
