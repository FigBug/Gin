/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================*/

/** A debugging tool that displays component hierarchy and properties.

    ComponentViewer is a window that shows information about the component under the
    mouse cursor, including its position, size, class name, and component hierarchy.
    This is extremely useful for debugging UI layouts and understanding component
    structures during development.

    Features:
    - Shows component under mouse in real-time
    - Displays component hierarchy tree
    - Shows component properties (bounds, class name, etc.)
    - Optional window position persistence
    - Auto-cleanup when tracked component is deleted
    - Can be set to always-on-top

    Example usage:
    @code
    auto* viewer = new ComponentViewer (mainComponent, nullptr, true);
    viewer->setVisible (true);
    // ComponentViewer will delete itself when closed or when mainComponent is deleted
    @endcode

    @see onClose
*/
class ComponentViewer : public juce::DocumentWindow,
                        public juce::ComponentListener
{
public:
    /** Creates a ComponentViewer window.

        @param toTrack      Optional component to track - when this component is deleted,
                           the ComponentViewer will automatically be deleted as well.
                           Pass nullptr to disable this feature.
        @param settings     Optional PropertiesFile for storing window position.
                           Pass nullptr to use a default settings file.
        @param alwaysOnTop  If true, the window will stay on top of other windows
    */
    ComponentViewer (Component* toTrack = nullptr, juce::PropertiesFile* settings = nullptr, bool alwaysOnTop = true);

    /** Destructor. */
    ~ComponentViewer() override;

    /** Callback invoked when the user requests to close the window.

        By default, the ComponentViewer deletes itself when closed. You can override
        this behavior by setting this callback to perform custom cleanup.
    */
    std::function<void ()> onClose;

private:
    //==============================================================================*/
    void componentBeingDeleted (Component&) override;
    void closeButtonPressed() override;

    void lookAndFeelChanged() override;

    void moved() override;
    void resized() override;

    void saveWindowPosition();

    class Snapshot;
    class ContentComponent;

    //==============================================================================*/
    juce::OptionalScopedPointer<juce::PropertiesFile> settings;
    Component* toTrack = nullptr;
};
