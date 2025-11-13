#pragma once

/**
    Undo/redo history management for text editing with efficient diff-based storage.

    TextHistory provides unlimited undo/redo functionality for text documents
    using a diff-based approach (binary patches) for memory-efficient storage.
    Instead of storing complete text snapshots, only the differences between
    states are stored.

    Key Features:
    - Unlimited undo/redo with configurable history limit
    - Efficient diff-based storage (binary patches)
    - Change notification via callback
    - Minimal memory footprint
    - Linear history (no branching)

    The implementation uses binary diff patches (forward and backward) to
    efficiently store changes, making it suitable for large documents or
    long editing sessions.

    Usage:
    @code
    TextHistory history;
    history.setHistoryLimit(100); // Keep last 100 changes

    // Register for state changes
    history.onStateChanged = []() {
        DBG("History state changed");
    };

    // Add text changes
    history.addText("Initial text");
    history.addText("Modified text");
    history.addText("Final text");

    // Undo changes
    if (history.canUndo())
    {
        history.undo();
        juce::String text = history.getCurrentText(); // Returns "Modified text"
    }

    // Redo changes
    if (history.canRedo())
    {
        history.redo();
        juce::String text = history.getCurrentText(); // Returns "Final text"
    }
    @endcode

    Implementation Details:
    - Uses binary diff algorithm for patch generation
    - Maintains stack of forward/backward patches
    - Stack pointer tracks current position in history
    - Adding new text while mid-history discards forward history

    @see juce::UndoManager for alternative undo/redo approach
*/
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
