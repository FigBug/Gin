/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================*/

/** Shows the component under the mouse and component hierarchy.
    Useful for debugging */
class ComponentViewer : public juce::DocumentWindow,
                        public juce::ComponentListener
{
public:
    //==============================================================================*/
    /* Pass in a settings file to store window location, otherwise default file will
       be used. Pass in a component to track and when this component is deleted, the
       ComponentViewer will be deleted as well, or nullptr if you don't want this feature
     */
    ComponentViewer (Component* toTrack = nullptr, juce::PropertiesFile* settings = nullptr, bool alwaysOnTop = true);
    ~ComponentViewer() override;

    /* User wants to close window, you should delete it. By default will delete itself */
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
