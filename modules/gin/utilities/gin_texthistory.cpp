
bool TextHistory::addText (const juce::String& t)
{
    if (t == currentText)
        return false;
    
    while (historyStack.size() - 1 > stackPointer)
        historyStack.removeLast();
    
    auto fordardPatch = Diff::bsDiff (currentText, t);
    auto backwardPatch = Diff::bsDiff (t, currentText);

    historyStack.add ({fordardPatch, backwardPatch});
    currentText = t;
    stackPointer++;
    
    while (historyStack.size() > limit)
    {
        historyStack.remove (0);
        stackPointer--;
    }
    
    if (onStateChanged)
        onStateChanged();
    
    return true;
}

const juce::String& TextHistory::getCurrentText()
{
    return currentText;
}

void TextHistory::undo()
{
    if (canUndo())
    {
        currentText = Diff::bsApplyPatch (currentText, historyStack[stackPointer].backwardPatch);
        stackPointer--;
        
        if (onStateChanged)
            onStateChanged();
    }
}

void TextHistory::redo()
{
    if (canRedo())
    {
        currentText = Diff::bsApplyPatch (currentText, historyStack[stackPointer + 1].forwardPatch);
        stackPointer++;
        
        if (onStateChanged)
            onStateChanged();
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
