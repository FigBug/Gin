#pragma once

class TextHistory
{
public:
	void setHistoryLimit (int numItems);

	void undo();
	void redo();
	bool canUndo();
	bool canRedo();
    
    void addText (const juce::String&);
    const juce::String& getCurrentText();

private:
	juce::String currentText;

	struct HistoryItem
	{
        juce::String forwardPatch;
        juce::String backwardPatch;
	};

    Diff diff;
    
	juce::Array<HistoryItem> historyStack;
	int stackPointer = -1;
	int limit = 100;
};
//-------------------------------------------------------------------------------------------------

