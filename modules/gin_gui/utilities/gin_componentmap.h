#pragma once

/**
    Internal class for mapping component paths to component pointers.

    ComponentMap builds and maintains a hierarchical path-based lookup system for JUCE components.
    It traverses a component hierarchy and creates path strings like "/parent/child/grandchild"
    that map to component pointers for fast lookup.

    This is primarily used by LayoutSupport to resolve component references in JSON layout files,
    allowing expressions like "getX('button')" or inset operations to find components by their
    component ID.

    @section paths Path Format

    Component paths follow a hierarchical slash-separated format:
    - Root component: ""
    - Direct child: "/childID"
    - Nested component: "/parentID/childID/grandchildID"

    Paths are case-insensitive and derived from:
    1. Component's "layoutID" property (if set)
    2. Component's componentID (if set)
    3. Component's name (as fallback)

    @section usage Usage Example

    @code
    // Create map for a component hierarchy
    ComponentMap map (myParentComponent);
    map.createMap();

    // Find a component by path
    if (auto* button = map.findComponent ("/dialog/buttons/okButton"))
        button->setBounds (x, y, w, h);

    // Get path of a component
    juce::String path = map.getComponentPath (someComponent);
    // Returns something like "/parent/child"
    @endcode

    @see LayoutSupport
*/
class ComponentMap
{
public:
    /**
        Constructs a ComponentMap for the given parent component.

        @param parent_  The root component of the hierarchy to map
    */
    ComponentMap (juce::Component& parent_);

    /**
        Builds the component map by traversing the component hierarchy.

        Recursively visits all child components and adds them to the internal map.
        Components without an ID (no componentID, name, or layoutID property) are skipped.

        @param checkChildren  Optional predicate to determine whether to recurse into
                             a component's children. If null, all children are traversed.
                             Return true to continue traversing children, false to skip.
    */
    void createMap (const std::function<bool (juce::Component&)>& checkChildren = nullptr);

    /**
        Adds a single component to the map.

        Useful for adding components dynamically after the initial map creation.
        The component must have a valid ID (componentID, name, or layoutID property).

        @param c  The component to add to the map
    */
    void addComponent (juce::Component& c);

    /**
        Gets the hierarchical path for a given component.

        Walks up the parent chain to build the full path from root to the component.
        Paths are case-insensitive and follow the format "/parent/child/grandchild".

        @param c  The component to get the path for
        @return   The component's path string, or empty string if component has no ID
    */
    juce::String getComponentPath (juce::Component& c);

    /**
        Finds a component by its path.

        @param path  The path to search for (case-insensitive). Empty string returns the root.
        @return      Pointer to the component if found, nullptr otherwise
    */
    juce::Component* findComponent (const juce::String& path);

    /**
        Clears all entries from the component map.

        Useful before rebuilding the map when the component hierarchy changes.
    */
    void clearMap();

    /**
        Returns all component paths in the map.

        Useful for debugging or inspecting the component hierarchy.

        @return  Array of all registered component paths
    */
    juce::StringArray dump();

protected:
    juce::Component& parent;

private:
    std::unordered_map<juce::String, juce::Component*> componentMap;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComponentMap)
};
