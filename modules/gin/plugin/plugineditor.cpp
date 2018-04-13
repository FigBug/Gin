#include "plugineditor.h"

//==============================================================================
UpdateChecker::UpdateChecker (GinAudioProcessorEditor& editor_)
  : Thread ("Update"), editor (editor_)
{
    if (ScopedPointer<PropertiesFile> props = editor.slProc.getSettings())
    {
       #ifdef JucePlugin_Name
        String url = props->getValue (JucePlugin_Name "_updateUrl");
        int last   = props->getIntValue (JucePlugin_Name "_lastUpdateCheck");
       #else
        jassertfalse;
        String url = "";
        int last = 0;
       #endif
        
        if (url.isNotEmpty())
        {
            editor.updateReady (url);
        }
        else if (time (NULL) > last + 86400)
        {
            startTimer (Random::getSystemRandom().nextInt ({1500, 2500}));
        }
    }
}

UpdateChecker::~UpdateChecker()
{
    while (isThreadRunning())
        Thread::sleep (10);
}

void UpdateChecker::timerCallback()
{
    stopTimer();
    startThread();
}

void UpdateChecker::run()
{
   #ifdef JucePlugin_Name
    XmlDocument doc (URL ("https://socalabs.com/version.xml").readEntireTextStream());
    if (ScopedPointer<XmlElement> root = doc.getDocumentElement())
    {
        if (ScopedPointer<PropertiesFile> props = editor.slProc.getSettings())
        {
            props->setValue (JucePlugin_Name "_lastUpdateCheck", int (time (NULL)));

            auto* child = root->getChildElement (0);
            while (child)
            {
                String name = child->getStringAttribute ("name");
                String ver  = child->getStringAttribute ("num");
                String url  = child->getStringAttribute ("url");
                
                if (name == JucePlugin_Name && versionStringToInt (ver) > versionStringToInt (JucePlugin_VersionString))
                {
                    props->setValue (JucePlugin_Name "_updateUrl", url);

                    const MessageManagerLock mmLock;
                    editor.updateReady (url);
                    break;
                }
                
                child = child->getNextElement();
            }
        }
    }
   #endif
}

//==============================================================================
NewsChecker::NewsChecker (GinAudioProcessorEditor& editor_)
: Thread ("News"), editor (editor_)
{
    if (ScopedPointer<PropertiesFile> props = editor.slProc.getSettings())
    {
        String url = props->getValue ("newsUrl");
        int last   = props->getIntValue ("lastNewsCheck");
        
        if (url.isNotEmpty())
        {
            editor.newsReady (url);
        }
        else if (time (NULL) > last + 86400)
        {
            startTimer (Random::getSystemRandom().nextInt ({1500, 2500}));
        }
    }
}

NewsChecker::~NewsChecker()
{
    while (isThreadRunning())
        Thread::sleep (10);
}

void NewsChecker::timerCallback()
{
    stopTimer();
    startThread();
}

void NewsChecker::run()
{
    XmlDocument doc (URL ("https://socalabs.com/feed/").readEntireTextStream());
    if (ScopedPointer<XmlElement> root = doc.getDocumentElement())
    {
        if (ScopedPointer<PropertiesFile> props = editor.slProc.getSettings())
        {
            if (auto* rss = root->getChildByName("channel"))
            {
                if (auto* item = rss->getChildByName ("item"))
                {
                    if (auto* link = item->getChildByName("link"))
                    {
                        props->setValue ("lastNewsCheck", int (time (NULL)));
                        
                        String url = link->getAllSubText();
                        
                        StringArray readNews = StringArray::fromTokens (props->getValue ("readNews"), "|", "");
                        if (readNews.isEmpty())
                        {
                            readNews.add (url);
                            props->setValue("readNews", readNews.joinIntoString ("|"));
                        }

                        if (! readNews.contains(url))
                        {
                            props->setValue ("newsUrl", url);
                            
                            const MessageManagerLock mmLock;
                            editor.newsReady (url);
                        }
                    }
                }
            }
        }
    }
}

//==============================================================================
GinAudioProcessorEditor::GinAudioProcessorEditor (GinProcessor& p, int cx_, int cy_) noexcept
  : GinAudioProcessorEditorBase (p), slProc (p), cx (cx_), cy (cy_)
{
    setLookAndFeel (&lf);
    
    addAndMakeVisible (&programs);
    addAndMakeVisible (&addButton);
    addAndMakeVisible (&deleteButton);
    addAndMakeVisible (&helpButton);
    addAndMakeVisible (&socaButton);
    addChildComponent (&newsButton);
    addChildComponent (&updateButton);

    programs.addListener (this);
    addButton.addListener (this);
    deleteButton.addListener (this);
    helpButton.addListener (this);
    socaButton.addListener (this);
    newsButton.addListener (this);
    updateButton.addListener (this);
    
    programs.setTooltip ("Select Preset");
    addButton.setTooltip ("Add Preset");
    deleteButton.setTooltip ("Delete Preset");
    helpButton.setTooltip ("Help >> About");
    newsButton.setTooltip ("News from SocaLabs");
    socaButton.setTooltip ("Visit www.socalabs.com");
    updateButton.setTooltip ("Update avaliable");
    
    refreshPrograms();
    
    updateChecker = new UpdateChecker (*this);
    newsChecker = new NewsChecker (*this);
}

GinAudioProcessorEditor::~GinAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void GinAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}

void GinAudioProcessorEditor::resized()
{
    GinAudioProcessorEditorBase::resized();
    
    const int pw = 100;
    const int ph = 20;
    const int tw = pw + ph + ph + 10;
    
    programs.setBounds (getWidth() / 2 - tw / 2, 30, pw, ph);
    addButton.setBounds (programs.getRight() + 5, programs.getY(), ph, ph);
    deleteButton.setBounds (addButton.getRight() + 5, programs.getY(), ph, ph);
    
    socaButton.setBounds (5, 5, ph, ph);
    newsButton.setBounds (socaButton.getBounds().translated (ph + 5, 0));
    helpButton.setBounds (getWidth() - ph - 5, 5, ph, ph);
    updateButton.setBounds (helpButton.getBounds().translated (- ph - 5, 0));
}

Rectangle<int> GinAudioProcessorEditor::getControlsArea()
{
    return getLocalBounds();
}

Rectangle<int> GinAudioProcessorEditor::getGridArea (int x, int y, int w, int h)
{
    return Rectangle<int> (inset + x * cx, headerHeight + y * cy + inset, w * cx, h * cy);
}

ParamComponent* GinAudioProcessorEditor::componentForId (const String& uid)
{
    for (auto* c : controls)
    {
        if (c->getUid() == uid)
            return c;
    }
    return nullptr;
}

void GinAudioProcessorEditor::setGridSize (int x, int y)
{
    setSize (x * cx + inset * 2, y * cy + inset * 2 + headerHeight);
}

void GinAudioProcessorEditor::refreshPrograms()
{
    programs.clear();
    
    for (int i = 0; i < processor.getNumPrograms(); i++)
        programs.addItem (processor.getProgramName (i), i + 1);
    
    programs.setSelectedItemIndex (slProc.getCurrentProgram(), dontSendNotification);
    deleteButton.setEnabled (slProc.getCurrentProgram() != 0);
}

void GinAudioProcessorEditor::buttonClicked (Button* b)
{
    if (b == &addButton)
    {
        AlertWindow w ("", "Create preset", AlertWindow::NoIcon, this);
        w.addTextEditor ("name", "", "Name:");
        w.addButton ("OK", 1);
        w.addButton ("Cancel", 0);
        w.setLookAndFeel (&lf);
        
        if (w.runModalLoop())
        {
            String txt = File::createLegalFileName (w.getTextEditor ("name")->getText());
            if (txt.isNotEmpty())
            {
                slProc.saveProgram (txt);
                refreshPrograms();
            }
        }
    }
    else if (b == &deleteButton)
    {
        AlertWindow w ("", "Delete preset '" + processor.getProgramName (programs.getSelectedItemIndex()) + "'?", AlertWindow::NoIcon, this);
        w.addButton ("Yes", 1);
        w.addButton ("No", 0);
        w.setLookAndFeel (&lf);
        
        if (w.runModalLoop())
        {
            slProc.deleteProgram (programs.getSelectedItemIndex());
            refreshPrograms();
        }
    }
    else if (b == &helpButton)
    {
        String msg;
        
       #ifdef JucePlugin_VersionString
        msg += JucePlugin_Name " v" JucePlugin_VersionString " (" __DATE__ ")\n\n";
       #endif
        msg += "Programming:\nRoland Rabien\nDavid Rowland\nROLI JUCE Framework\n";
        if (additionalProgramming.isNotEmpty())
            msg += additionalProgramming;
        msg += "\n\n";
        msg += "Copyright ";
        msg += String (__DATE__ + 7);
        
        AlertWindow w ("---- About ----", msg, AlertWindow::NoIcon, this);
        w.addButton ("OK", 1);
        w.setLookAndFeel (&lf);
        
        w.runModalLoop();
    }
    else if (b == &updateButton)
    {
        URL (updateUrl).launchInDefaultBrowser();
        updateButton.setVisible (false);
        
       #ifdef JucePlugin_Name
        if (ScopedPointer<PropertiesFile> props = slProc.getSettings())
            props->setValue (JucePlugin_Name "_updateUrl", "");
       #endif
    }
    else if (b == &socaButton)
    {
        URL ("http://www.socalabs.com").launchInDefaultBrowser();
    }
    else if (b == &newsButton)
    {
        URL (newsUrl).launchInDefaultBrowser();
        newsButton.setVisible (false);
        
        if (ScopedPointer<PropertiesFile> props = slProc.getSettings())
        {
            props->setValue ("newsUrl", "");
            
            StringArray readNews = StringArray::fromTokens (props->getValue ("readNews"), "|", "");
            readNews.add (newsUrl);
            props->setValue("readNews", readNews.joinIntoString ("|"));
        }
    }
}

void GinAudioProcessorEditor::comboBoxChanged (ComboBox* c)
{
    if (c == &programs)
    {
        int idx = programs.getSelectedItemIndex();
        deleteButton.setEnabled (idx != 0);
        processor.setCurrentProgram (idx);
    }
}

void GinAudioProcessorEditor::updateReady (String updateUrl_)
{
    updateUrl = updateUrl_;
    updateButton.setVisible (true);
}

void GinAudioProcessorEditor::newsReady (String newsUrl_)
{
    newsUrl = newsUrl_;
    newsButton.setVisible (true);
}
