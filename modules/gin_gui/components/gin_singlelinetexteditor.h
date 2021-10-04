/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2015 - ROLI Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#pragma once

//==============================================================================
/**
    An editable text box.

    A TextEditor can either be in single- or multi-line mode, and supports mixed
    fonts and colours.

    This is a hacked up version of the JUCE TextEditor that supports all
    justification modes

    @see TextEditor::Listener, Label
*/
class SingleLineTextEditor  : public juce::Component,
                              public juce::TextInputTarget,
                              public juce::SettableTooltipClient
{
public:
    //==============================================================================
    /** Creates a new, empty text editor.

        @param componentName        the name to pass to the component for it to use as its name
    */
    explicit SingleLineTextEditor (const juce::String& componentName = juce::String());

    /** Destructor. */
    ~SingleLineTextEditor() override;

    /** Indicates whether the tab key should be accepted and used to input a tab character,
        or whether it gets ignored.

        By default the tab key is ignored, so that it can be used to switch keyboard focus
        between components.
    */
    void setTabKeyUsedAsCharacter (bool shouldTabKeyBeUsed);

    /** Returns true if the tab key is being used for input.
        @see setTabKeyUsedAsCharacter
    */
    bool isTabKeyUsedAsCharacter() const                        { return tabKeyUsed; }

    /** This can be used to change whether escape and return keypress events are
        propagated up to the parent component.
        The default here is true, meaning that these events are not allowed to reach the
        parent, but you may want to allow them through so that they can trigger other
        actions, e.g. closing a dialog box, etc.
    */
    void setEscapeAndReturnKeysConsumed (bool shouldBeConsumed) noexcept;

    //==============================================================================
    /** Changes the editor to read-only mode.

        By default, the text editor is not read-only. If you're making it read-only, you
        might also want to call setCaretVisible (false) to get rid of the caret.

        The text can still be highlighted and copied when in read-only mode.

        @see isReadOnly, setCaretVisible
    */
    void setReadOnly (bool shouldBeReadOnly);

    /** Returns true if the editor is in read-only mode. */
    bool isReadOnly() const noexcept;

    //==============================================================================
    /** Makes the caret visible or invisible.
        By default the caret is visible.
        @see setCaretColour, setCaretPosition
    */
    void setCaretVisible (bool shouldBeVisible);

    /** Returns true if the caret is enabled.
        @see setCaretVisible
    */
    bool isCaretVisible() const noexcept                            { return caretVisible && ! isReadOnly(); }

    //==============================================================================
    /** Allows a right-click menu to appear for the editor.

        (This defaults to being enabled).

        If enabled, right-clicking (or command-clicking on the Mac) will pop up a menu
        of options such as cut/copy/paste, undo/redo, etc.
    */
    void setPopupMenuEnabled (bool menuEnabled);

    /** Returns true if the right-click menu is enabled.
        @see setPopupMenuEnabled
    */
    bool isPopupMenuEnabled() const noexcept                        { return popupMenuEnabled; }

    /** Returns true if a popup-menu is currently being displayed. */
    bool isPopupMenuCurrentlyActive() const noexcept                { return menuActive; }

    //==============================================================================
    /** A set of colour IDs to use to change the colour of various aspects of the editor.

        These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
        methods.

        NB: You can also set the caret colour using CaretComponent::caretColourId

        @see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
    */
    enum ColourIds
    {
        backgroundColourId       = 0x1000200, /**< The colour to use for the text component's background - this can be
                                                   transparent if necessary. */

        textColourId             = 0x1000201, /**< The colour that will be used when text is added to the editor. Note
                                                   that because the editor can contain multiple colours, calling this
                                                   method won't change the colour of existing text - to do that, call
                                                   applyFontToAllText() after calling this method.*/

        highlightColourId        = 0x1000202, /**< The colour with which to fill the background of highlighted sections of
                                                   the text - this can be transparent if you don't want to show any
                                                   highlighting.*/

        highlightedTextColourId  = 0x1000203, /**< The colour with which to draw the text in highlighted sections. */

        outlineColourId          = 0x1000205, /**< If this is non-transparent, it will be used to draw a box around
                                                   the edge of the component. */

        focusedOutlineColourId   = 0x1000206, /**< If this is non-transparent, it will be used to draw a box around
                                                   the edge of the component when it has focus. */

        shadowColourId           = 0x1000207, /**< If this is non-transparent, it'll be used to draw an inner shadow
                                                   around the edge of the editor. */
    };

    //==============================================================================
    /** Sets the font to use for newly added text.

        This will change the font that will be used next time any text is added or entered
        into the editor. It won't change the font of any existing text - to do that, use
        applyFontToAllText() instead.

        @see applyFontToAllText
    */
    void setFont (const juce::Font& newFont);

    /** Applies a font to all the text in the editor.
        This will also set the current font to use for any new text that's added.
        @see setFont
    */
    void applyFontToAllText (const juce::Font& newFont);

    /** Returns the font that's currently being used for new text.
        @see setFont
    */
    const juce::Font& getFont() const noexcept            { return currentFont; }

    void setJustificationType (juce::Justification just)  { justification = just; }

    //==============================================================================
    /** If set to true, focusing on the editor will highlight all its text.

        (Set to false by default).

        This is useful for boxes where you expect the user to re-enter all the
        text when they focus on the component, rather than editing what's already there.
    */
    void setSelectAllWhenFocused (bool shouldSelectAll);

    /** When the text editor is empty, it can be set to display a message.

        This is handy for things like telling the user what to type in the box - the
        string is only displayed, it's not taken to actually be the contents of
        the editor.
    */
    void setTextToShowWhenEmpty (const juce::String& text, juce::Colour colourToUse);

    //==============================================================================
    /**
     Receives callbacks from a TextEditor component when it changes.

     @see TextEditor::addListener
     */
    class JUCE_API  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener()  {}

        /** Called when the user changes the text in some way. */
        virtual void sltextEditorTextChanged (SingleLineTextEditor&) {}

        /** Called when the user presses the return key. */
        virtual void sltextEditorReturnKeyPressed (SingleLineTextEditor&) {}

        /** Called when the user presses the escape key. */
        virtual void sltextEditorEscapeKeyPressed (SingleLineTextEditor&) {}

        /** Called when the text editor loses focus. */
        virtual void sltextEditorFocusLost (SingleLineTextEditor&) {}
    };

    //==============================================================================

    /** Registers a listener to be told when things happen to the text.
        @see removeListener
    */
    void addListener (SingleLineTextEditor::Listener* newListener);

    /** Deregisters a listener.
        @see addListener
    */
    void removeListener (SingleLineTextEditor::Listener* listenerToRemove);

    //==============================================================================
    /** Returns the entire contents of the editor. */
    juce::String getText() const;

    /** Returns a section of the contents of the editor. */
    juce::String getTextInRange (const juce::Range<int>& textRange) const override;

    /** Returns true if there are no characters in the editor.
        This is far more efficient than calling getText().isEmpty().
    */
    bool isEmpty() const;

    /** Sets the entire content of the editor.

        This will clear the editor and insert the given text (using the current text colour
        and font). You can set the current text colour using
        @code setColour (TextEditor::textColourId, ...);
        @endcode

        @param newText                  the text to add
        @param sendTextChangeMessage    if true, this will cause a change message to
                                        be sent to all the listeners.
        @see insertTextAtCaret
    */
    void setText (const juce::String& newText,
                  bool sendTextChangeMessage = true);

    /** Returns a Value object that can be used to get or set the text.

        Bear in mind that this operate quite slowly if your text box contains large
        amounts of text, as it needs to dynamically build the string that's involved.
        It's best used for small text boxes.
    */
    juce::Value& getTextValue();

    /** Inserts some text at the current caret position.

        If a section of the text is highlighted, it will be replaced by
        this string, otherwise it will be inserted.

        To delete a section of text, you can use setHighlightedRegion() to
        highlight it, and call insertTextAtCaret (String()).

        @see setCaretPosition, getCaretPosition, setHighlightedRegion
    */
    void insertTextAtCaret (const juce::String& textToInsert) override;

    /** Deletes all the text from the editor. */
    void clear();

    /** Deletes the currently selected region.
        This doesn't copy the deleted section to the clipboard - if you need to do that, call copy() first.
        @see copy, paste, SystemClipboard
    */
    void cut();

    /** Copies the currently selected region to the clipboard.
        @see cut, paste, SystemClipboard
    */
    void copy();

    /** Pastes the contents of the clipboard into the editor at the caret position.
        @see cut, copy, SystemClipboard
    */
    void paste();

    //==============================================================================
    /** Returns the current index of the caret.
        @see setCaretPosition
    */
    int getCaretPosition() const;

    /** Moves the caret to be in front of a given character.
        @see getCaretPosition, moveCaretToEnd
    */
    void setCaretPosition (int newIndex);

    /** Get the graphical position of the caret.

        The rectangle returned is relative to the component's top-left corner.
        @see scrollEditorToPositionCaret
    */
    juce::Rectangle<int> getCaretRectangle() override;

    /** Selects a section of the text. */
    void setHighlightedRegion (const juce::Range<int>& newSelection) override;

    /** Returns the range of characters that are selected.
        If nothing is selected, this will return an empty range.
        @see setHighlightedRegion
    */
    juce::Range<int> getHighlightedRegion() const override            { return selection; }

    /** Returns the section of text that is currently selected. */
    juce::String getHighlightedText() const;

    /** Finds the index of the character at a given position.
        The coordinates are relative to the component's top-left.
    */
    int getTextIndexAt (int x, int y);

    /** Counts the number of characters in the text.

        This is quicker than getting the text as a string if you just need to know
        the length.
    */
    int getTotalNumChars() const;

    /** Returns the total width of the text, as it is currently laid-out.

        This may be larger than the size of the TextEditor, and can change when
        the TextEditor is resized or the text changes.
    */
    int getTextWidth() const;

    /** Returns the maximum height of the text, as it is currently laid-out.

        This may be larger than the size of the TextEditor, and can change when
        the TextEditor is resized or the text changes.
    */
    int getTextHeight() const;

    //==============================================================================
    void moveCaretToEnd();
    bool moveCaretLeft (bool moveInWholeWordSteps, bool selecting);
    bool moveCaretRight (bool moveInWholeWordSteps, bool selecting);
    bool moveCaretUp (bool selecting);
    bool moveCaretDown (bool selecting);
    bool pageUp (bool selecting);
    bool pageDown (bool selecting);
    bool scrollDown() { return false; }
    bool scrollUp()   { return false; }
    bool moveCaretToTop (bool selecting);
    bool moveCaretToStartOfLine (bool selecting);
    bool moveCaretToEnd (bool selecting);
    bool moveCaretToEndOfLine (bool selecting);
    bool deleteBackwards (bool moveInWholeWordSteps);
    bool deleteForwards (bool moveInWholeWordSteps);
    bool copyToClipboard();
    bool cutToClipboard();
    bool pasteFromClipboard();
    bool selectAll();
    bool undo();
    bool redo();

    //==============================================================================
    /** This adds the items to the popup menu.

        By default it adds the cut/copy/paste items, but you can override this if
        you need to replace these with your own items.

        If you want to add your own items to the existing ones, you can override this,
        call the base class's addPopupMenuItems() method, then append your own items.

        When the menu has been shown, performPopupMenuAction() will be called to
        perform the item that the user has chosen.

        The default menu items will be added using item IDs from the
        StandardApplicationCommandIDs namespace.

        If this was triggered by a mouse-click, the mouseClickEvent parameter will be
        a pointer to the info about it, or may be null if the menu is being triggered
        by some other means.

        @see performPopupMenuAction, setPopupMenuEnabled, isPopupMenuEnabled
    */
    virtual void addPopupMenuItems (juce::PopupMenu& menuToAddTo,
                                    const juce::MouseEvent* mouseClickEvent);

    /** This is called to perform one of the items that was shown on the popup menu.

        If you've overridden addPopupMenuItems(), you should also override this
        to perform the actions that you've added.

        If you've overridden addPopupMenuItems() but have still left the default items
        on the menu, remember to call the superclass's performPopupMenuAction()
        so that it can perform the default actions if that's what the user clicked on.

        @see addPopupMenuItems, setPopupMenuEnabled, isPopupMenuEnabled
    */
    virtual void performPopupMenuAction (int menuItemID);

    //==============================================================================
    /** Base class for input filters that can be applied to a TextEditor to restrict
        the text that can be entered.
    */
    class JUCE_API  InputFilter
    {
    public:
        InputFilter() {}
        virtual ~InputFilter() {}

        /** This method is called whenever text is entered into the editor.
            An implementation of this class should should check the input string,
            and return an edited version of it that should be used.
        */
        virtual juce::String filterNewText (SingleLineTextEditor&, const juce::String& newInput) = 0;
    };

    /** An input filter for a TextEditor that limits the length of text and/or the
        characters that it may contain.
    */
    class JUCE_API  LengthAndCharacterRestriction  : public InputFilter
    {
    public:
        /** Creates a filter that limits the length of text, and/or the characters that it can contain.
            @param maxNumChars          if this is > 0, it sets a maximum length limit; if <= 0, no
                                        limit is set
            @param allowedCharacters    if this is non-empty, then only characters that occur in
                                        this string are allowed to be entered into the editor.
        */
        LengthAndCharacterRestriction (int maxNumChars, const juce::String& allowedCharacters);

        juce::String filterNewText (SingleLineTextEditor&, const juce::String&) override;

    private:
        juce::String allowedCharacters;
        int maxLength;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LengthAndCharacterRestriction)
    };

    /** Sets an input filter that should be applied to this editor.
        The filter can be nullptr, to remove any existing filters.
        If takeOwnership is true, then the filter will be owned and deleted by the editor
        when no longer needed.
    */
    void setInputFilter (InputFilter* newFilter, bool takeOwnership);

    /** Returns the current InputFilter, as set by setInputFilter(). */
    InputFilter* getInputFilter() const noexcept                { return inputFilter; }

    /** Sets limits on the characters that can be entered.
        This is just a shortcut that passes an instance of the LengthAndCharacterRestriction
        class to setInputFilter().

        @param maxTextLength        if this is > 0, it sets a maximum length limit; if 0, no
                                    limit is set
        @param allowedCharacters    if this is non-empty, then only characters that occur in
                                    this string are allowed to be entered into the editor.
    */
    void setInputRestrictions (int maxTextLength,
                               const juce::String& allowedCharacters = juce::String());

    void setKeyboardType (VirtualKeyboardType type) noexcept    { keyboardType = type; }

    //==============================================================================
    /** This abstract base class is implemented by LookAndFeel classes to provide
     TextEditor drawing functionality.
     */
    struct JUCE_API  LookAndFeelMethods
    {
        virtual ~LookAndFeelMethods() {}

        virtual void fillSingleLineTextEditorBackground (juce::Graphics&, int width, int height, SingleLineTextEditor&) = 0;
        virtual void drawSingleLineTextEditorOutline (juce::Graphics&, int width, int height, SingleLineTextEditor&) = 0;

        virtual juce::CaretComponent* createSingleLineCaretComponent (Component* keyFocusOwner) = 0;
    };

    //==============================================================================
    /** @internal */
    void paint (juce::Graphics&) override;
    /** @internal */
    void paintOverChildren (juce::Graphics&) override;
    /** @internal */
    void mouseDown (const juce::MouseEvent&) override;
    /** @internal */
    void mouseUp (const juce::MouseEvent&) override;
    /** @internal */
    void mouseDrag (const juce::MouseEvent&) override;
    /** @internal */
    void mouseDoubleClick (const juce::MouseEvent&) override;
    /** @internal */
    void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails&) override;
    /** @internal */
    bool keyPressed (const juce::KeyPress&) override;
    /** @internal */
    bool keyStateChanged (bool) override;
    /** @internal */
    void focusGained (FocusChangeType) override;
    /** @internal */
    void focusLost (FocusChangeType) override;
    /** @internal */
    void resized() override;
    /** @internal */
    void enablementChanged() override;
    /** @internal */
    void colourChanged() override;
    /** @internal */
    void lookAndFeelChanged() override;
    /** @internal */
    bool isTextInputActive() const override;
    /** @internal */
    void setTemporaryUnderlining (const juce::Array<juce::Range<int> >&) override;
    /** @internal */
    VirtualKeyboardType getKeyboardType() override    { return keyboardType; }

protected:
    /** Used internally to dispatch a text-change message. */
    void textChanged();

    /** Begins a new transaction in the UndoManager. */
    void newTransaction();

    /** Can be overridden to intercept return key presses directly */
    virtual void returnPressed();

    /** Can be overridden to intercept escape key presses directly */
    virtual void escapePressed();

private:
    //==============================================================================
    class Iterator;
    JUCE_PUBLIC_IN_DLL_BUILD (class UniformTextSection)
    class TextHolderComponent;
    class InsertAction;
    class RemoveAction;
    friend class InsertAction;
    friend class RemoveAction;

    std::unique_ptr<TextHolderComponent> textHolder;

    bool readOnly;
    bool caretVisible;
    bool popupMenuEnabled;
    bool selectAllTextWhenFocused;
    bool wasFocused;
    bool tabKeyUsed;
    bool menuActive;
    bool valueTextNeedsUpdating;
    bool consumeEscAndReturnKeys;

    juce::UndoManager undoManager;
    std::unique_ptr<juce::CaretComponent> caret;
    juce::Range<int> selection;
    unsigned int lastTransactionTime;
    juce::Font currentFont;
    mutable int totalNumChars;
    int caretPosition;
    juce::OwnedArray<UniformTextSection> sections;
    juce::String textToShowWhenEmpty;
    juce::Colour colourForTextWhenEmpty;
    juce::OptionalScopedPointer<InputFilter> inputFilter;
    juce::Value textValue;
    VirtualKeyboardType keyboardType;
    juce::Justification justification = juce::Justification::centred;

    enum
    {
        notDragging,
        draggingSelectionStart,
        draggingSelectionEnd
    } dragType;

    juce::ListenerList<Listener> listeners;
    juce::Array<juce::Range<int> > underlinedSections;

    void moveCaret (int newCaretPos);
    void moveCaretTo (int newPosition, bool isSelecting);
    void recreateCaret();
    void handleCommandMessage (int) override;
    void coalesceSimilarSections();
    void splitSection (int sectionIndex, int charToSplitAt);
    void clearInternal (juce::UndoManager*);
    void insert (const juce::String&, int insertIndex, const juce::Font&, const juce::Colour, juce::UndoManager*, int newCaretPos);
    void reinsert (int insertIndex, const juce::OwnedArray<UniformTextSection>&);
    void remove (juce::Range<int> range, juce::UndoManager*, int caretPositionToMoveTo);
    void getCharPosition (int index, float& x, float& y, float& lineHeight) const;
    void updateCaretPosition();
    void updateValueFromText();
    void textWasChangedByValue();
    int indexAtPosition (float x, float y);
    int findWordBreakAfter (int position) const;
    int findWordBreakBefore (int position) const;
    bool moveCaretWithTransaction (int newPos, bool selecting);
    friend class TextHolderComponent;
    void drawContent (juce::Graphics&);
    float getWordWrapWidth() const;
    void timerCallbackInt();
    void repaintText (juce::Range<int>);
    bool undoOrRedo (bool shouldUndo);
    juce::UndoManager* getUndoManager() noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SingleLineTextEditor)
};
