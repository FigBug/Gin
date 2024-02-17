
void TextHistory::addText (const juce::String& t)
{
    if (t == currentText)
        return;
    
    while (historyStack.size() - 1 > stackPointer)
        historyStack.removeLast();
    
    auto fordardPatch = diff.diff (currentText, t);
    auto backwardPatch = diff.diff (t, currentText);
    
    historyStack.add ({fordardPatch, backwardPatch});
    currentText = t;
    
    while (historyStack.size() > limit)
    {
        historyStack.remove (0);
        stackPointer--;
    }
}

const juce::String& TextHistory::getCurrentText()
{
    return currentText;
}

void TextHistory::undo()
{
    if (canUndo())
    {
        currentText = diff.applyPatch (currentText, historyStack[stackPointer].backwardPatch);
        stackPointer--;
    }
}

void TextHistory::redo()
{
    if (canRedo())
    {
        currentText = diff.applyPatch (currentText, historyStack[stackPointer + 1].forwardPatch);
        stackPointer++;
    }
}

bool TextHistory::canUndo()
{
    return stackPointer > 0;
}

bool TextHistory::canRedo()
{
    return stackPointer < historyStack.size() - 1;
}

void TextHistory::setHistoryLimit (int limit_)
{
    limit = limit_;
}
