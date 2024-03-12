#pragma once

class TextHistory
{
public:
    void setHistoryLimit (int numItems);

    void undo();
    void redo();
    bool canUndo();
    bool canRedo();

    bool addText (const juce::String&);
    const juce::String& getCurrentText();

    std::function<void ()> onStateChanged;

private:
    juce::String currentText;

    struct HistoryItem
    {
        std::vector<uint8_t> forwardPatch;
        std::vector<uint8_t> backwardPatch;
    };

    juce::Array<HistoryItem> historyStack;
    int stackPointer = -1;
    int limit = 100;
};
//-------------------------------------------------------------------------------------------------
