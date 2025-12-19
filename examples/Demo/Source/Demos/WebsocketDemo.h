/*
  ==============================================================================

    WebsocketDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct WebsocketDemo : public DemoComponent
{
    WebsocketDemo()
    {
        setName ("Websocket");

        addAndMakeVisible (inText);
        addAndMakeVisible (outText);
        addAndMakeVisible (sendButton);

        outText.setText ("Hello World");
        inText.setMultiLine (true);
        inText.setReadOnly (true);

        sendButton.onClick = [this]
        {
            websocket.send (outText.getText());
        };

        websocket.onConnect = [this]
        {
            inText.moveCaretToEnd();
            inText.insertTextAtCaret ("Connected\n");
        };
        websocket.onDisconnect = [this]
        {
            inText.moveCaretToEnd();
            inText.insertTextAtCaret ("Disconnected\n");
        };
        websocket.onText = [this] (const juce::String& txt)
        {
            inText.moveCaretToEnd();
            inText.insertTextAtCaret (txt + "\n");
        };

        websocket.connect();
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced (8);

        auto top = r.removeFromTop (20);

        sendButton.setBounds (top.removeFromRight (80));
        top.removeFromRight (8);
        outText.setBounds (top);

        r.removeFromTop (8);
        inText.setBounds (r);
    }

    juce::TextEditor inText;
    juce::TextEditor outText;
    juce::TextButton sendButton {"Send"};

    gin::AsyncWebsocket websocket {juce::URL ("wss://ws.postman-echo.com/raw") };
};
