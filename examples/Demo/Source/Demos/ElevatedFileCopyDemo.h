/*
  ==============================================================================

    ElevatedFileCopyDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
#if defined JUCE_MAC || defined JUCE_WINDOWS
struct ElevatedFileCopyDemo : public DemoComponent
{
public:
    ElevatedFileCopyDemo()
    {
        setName ("Elevated File Copy");

        addAndMakeVisible (srcDir);
        addAndMakeVisible (dstDir);
        addAndMakeVisible (copyButton);

       #ifdef JUCE_MAC
        srcDir.setText ("/src");
        dstDir.setText ("/dst");
       #endif
       #ifdef JUCE_WINDOWS
        srcDir.setText ("c:\\src");
        dstDir.setText ("d:\\dst");
       #endif

        copyButton.onClick = [this] { copyFiles(); };
    }

    void copyFiles()
    {
        juce::File src (srcDir.getText());
        juce::File dst (dstDir.getText());

        juce::Array<juce::File> files;
        src.findChildFiles (files, juce::File::findFiles, false);

        gin::ElevatedFileCopy efc;

        for (auto f :files)
            efc.copyFile (f, dst.getChildFile (f.getFileName()));

        efc.execute (true);
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);

        srcDir.setBounds (rc.removeFromTop (25));
        rc.removeFromTop (4);
        dstDir.setBounds (rc.removeFromTop (25));
        rc.removeFromTop (4);
        copyButton.setBounds (rc.removeFromTop (25).removeFromLeft (100));
    }

    juce::TextEditor srcDir, dstDir;
    juce::TextButton copyButton { "Copy" };
};
#endif
