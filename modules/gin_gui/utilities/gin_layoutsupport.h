#pragma once

class ComponentMap
{
public:
	ComponentMap ( juce::Component& parent_ );
	void createMap ( const std::function<bool ( juce::Component& )>& checkChildren = nullptr );
	void addComponent ( juce::Component& );
	std::string getComponentPath ( juce::Component& c );
	juce::Component* findComponent ( const std::string& path );
	void clearMap ();

	juce::StringArray dump();

protected:
	juce::Component&	parent;

private:
	std::unordered_map<std::string, juce::Component*>	componentMap;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR ( ComponentMap )
};

class ConstantsStack
{
public:
	void set ( const std::string& name, double value );
	std::optional<double> get ( const std::string& name );

	class ScopedSave
	{
	public:
		ScopedSave ( ConstantsStack& o ) : owner ( o )
		{
		}

		~ScopedSave ()
		{
			owner.constants.resize ( size );
		}

		ConstantsStack& owner;
		size_t size { owner.constants.size () };
	};

private:
	std::vector<std::pair<std::string, double>> constants;
};

class Parser
{
public:
	Parser ( const juce::String equation );

	double evaluate ();

	// Need to be set before each evaluation
	ComponentMap* 		compMap = nullptr;
	juce::Component*	curComponent = nullptr;
	ConstantsStack*		constants = nullptr;

private:
	juce::Component* getComp ( const juce::String& cid );

	std::vector<std::string> usedConstants;
	size_t varStackPtr = 0;
	double varStack[ 10 ];

	gin::EquationParser parser;
};

class LayoutSupport : private gin::FileSystemWatcher::Listener
{
public:
	LayoutSupport ( juce::Component& parent_, std::function<std::pair<juce::Component*,bool>(const juce::String&)> factory_ = nullptr );

    ~LayoutSupport () override;
    static void dumpComponents ( juce::Component& c );

	bool isLayoutSet ();

    void setLayout ( const juce::Array<juce::File>& files );
	void setLayout ( const juce::StringArray& resourceNames );
	void setLayout ( const juce::String& rawJson );

	void setConstant ( const std::string& name, int value );

	std::function<void ()> layoutChanged;

private:
	juce::String findContent ( const juce::String& name );
	juce::Component* getComp ( const juce::String& cid );

	juce::Component&	parent;
	
	struct Bounds
	{
		int x;
		int y;
		int w;
		int h;
		bool hasPosition = false;
		bool hasSize = false;
	};

	struct JsonFile
	{
		juce::String name;
		juce::String contents;
	};

	void setLayoutInternal ( const juce::Array<JsonFile>& files );

	int parse ( const juce::var& equation, int equationIndex );

	void setComponentsLayout ( const std::string& currentPath, const juce::var& components );

	void loadConstants ( juce::var& j );
	void loadMacros ( juce::var& j );

	Bounds getBounds ( int idIdx, const juce::var& component );

	juce::Component* setPosition ( const std::string& currentPath,
								   const std::string& id,
								   int idIdx,
								   const juce::var& component );

    juce::var expandMacro ( const juce::var& component );

	void setGridPositions ( const std::string& currentPath, const juce::var& component );

    inline static int						refCount = 0;
    inline static std::unique_ptr<FileSystemWatcher> watcher;

    // gin::FileSystemWatcher::Listener
    void fileChanged ( const juce::File&, gin::FileSystemWatcher::FileSystemEvent ) override;

	ConstantsStack							constants;
	std::map<std::string, juce::var>    	macros;

	juce::Array<juce::File>					layoutFiles;
	bool									layoutSet = false;

	juce::Component* 						curComponent = nullptr;

	juce::OwnedArray<juce::Component>		createdComponents;

	ComponentMap	compMap;
	std::function<std::pair<juce::Component*,bool>(const juce::String&)> componentFactory;
};
