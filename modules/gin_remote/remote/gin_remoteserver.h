/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** A small TCP server that lets an external process drive a JUCE UI.

    Intended for UI automation, scripted testing and AI coding agents. Clients
    connect over a local socket and exchange newline-delimited JSON:

    @code
    -> {"id": 1, "cmd": "tree", "args": {"depth": 2}}
    <- {"id": 1, "ok": true, "result": {...}}
    <- {"id": 2, "ok": false, "error": "No component matches '#missing'"}
    @endcode

    Built in commands cover the component tree (tree, find, describe, at,
    windows), input (click, drag, mouse, wheel, key, type, focus), state
    (get, set, press, resize), screenshots and waiting. Products add their
    own with addCommand(), and can decorate every component description
    with addComponentInfoProvider().

    Nothing happens unless you create and start a server, so it is safe to
    compile into a shared module. Only start it in development builds.

    Components are addressed with a tiny selector language. A selector is a
    space separated list of steps, each step narrowing the search to the
    descendants of the previous matches:

      #id            component ID (Component::getComponentID)
      .ClassName     class name, with or without namespace
      name           component name, or button / label text
      ~text          substring match of name, text or id
      @x,y           deepest component at a screen point
      /0/3/1         index path: desktop window 0, child 3, child 1
      *              any component
      step[n]        nth match of that step

    e.g.  "#sidebar .TextButton[2]"  or  "Save"  or  "@120,240"

    @code
    gin::RemoteServer::Options opts;
    opts.port = 27182;
    remote = std::make_unique<gin::RemoteServer> (opts);

    remote->addCommand ("loadPreset", "Load a preset by name", R"({"name":"string!: preset name"})",
                        [this] (const juce::var& args)
                        {
                            if (! loadPreset (args["name"].toString()))
                                return gin::RemoteServer::CommandResult::fail ("No such preset");
                            return gin::RemoteServer::CommandResult (juce::var (true));
                        });

    remote->start();
    @endcode

    A command line client and MCP server live in tools/gin_remote.
*/
class RemoteServer : private juce::Thread
{
public:
    //==============================================================================
    struct Options
    {
        /** First port to try. The GIN_REMOTE_PORT environment variable overrides this. */
        int port = 27182;
        /** Ports port .. port + portRange - 1 are tried in turn, so several instances can run at once. */
        int portRange = 16;
        /** Interface to listen on. Keep this loopback unless you know what you are doing. */
        juce::String bindAddress = "127.0.0.1";
        /** How long a command may wait for the message thread before failing. */
        int messageThreadTimeoutMs = 10000;
        /** Name reported by ping and written to the discovery file. Defaults to the app or plugin name. */
        juce::String appName;
        /** Write a small json file to <temp>/gin_remote/<port>.json so clients can find running servers. */
        bool writeDiscoveryFile = true;
        /** Route juce::Logger output into the log command's buffer. */
        bool captureLogger = true;
    };

    //==============================================================================
    /** The outcome of a command. Construct from a juce::var for success, or use fail(). */
    struct CommandResult
    {
        CommandResult() = default;
        CommandResult (const juce::var& v) : value (v) {}

        static CommandResult fail (const juce::String& message)
        {
            CommandResult r;
            r.error = message.isEmpty() ? juce::String ("Command failed") : message;
            return r;
        }

        bool ok() const noexcept    { return error.isEmpty(); }

        juce::var value;
        juce::String error;
    };

    /** Handler for a command that runs on the message thread. */
    using CommandHandler = std::function<CommandResult (const juce::var& args)>;

    /** Passed to async commands so they can hop onto the message thread and pause between steps. */
    class Context
    {
    public:
        Context (RemoteServer& s) : server (s) {}

        /** Runs a function on the message thread and waits for it, honouring Options::messageThreadTimeoutMs. */
        CommandResult runOnMessageThread (std::function<CommandResult()> fn)     { return server.runOnMessageThread (std::move (fn)); }

        /** Sleeps on the server thread. Returns false if the server is shutting down. */
        bool sleep (int ms)                                                     { return server.sleepOnServerThread (ms); }

        RemoteServer& server;
    };

    /** Handler for a command that runs on the server thread. Use this when a command
        needs to span several message loop iterations, e.g. a drag or a wait. */
    using AsyncCommandHandler = std::function<CommandResult (Context&, const juce::var& args)>;

    /** Called for every component description so products can add their own fields. */
    using ComponentInfoProvider = std::function<void (juce::Component&, juce::DynamicObject& info)>;

    //==============================================================================
    RemoteServer();
    explicit RemoteServer (const Options&);
    ~RemoteServer() override;

    /** Starts listening. Returns false if no port in the configured range could be bound. */
    bool start();
    /** Stops listening and disconnects any client. */
    void stop();

    bool isRunning() const noexcept     { return running; }
    int getPort() const noexcept        { return boundPort; }
    const Options& getOptions() const noexcept { return options; }

    //==============================================================================
    /** Adds a command that runs on the message thread.

        @param name         command name, e.g. "loadPreset"
        @param description  one line of help, shown by the commands command and used as the MCP tool description
        @param argsSpec     optional json object describing the arguments, mapping name to "type: description".
                            Types are string, int, number, bool, array, object. Add ! after the type for required args.
                            e.g. R"({"name":"string!: preset name","layer":"int: 0 based layer, default 0"})"
        @param handler      the handler
    */
    void addCommand (const juce::String& name, const juce::String& description, const juce::String& argsSpec, CommandHandler handler);

    /** Adds a command that runs on the server thread. See AsyncCommandHandler. */
    void addAsyncCommand (const juce::String& name, const juce::String& description, const juce::String& argsSpec, AsyncCommandHandler handler);

    void removeCommand (const juce::String& name);

    /** Registers a hook that can add fields to every component description. */
    void addComponentInfoProvider (ComponentInfoProvider);

    //==============================================================================
    /** Appends a line to the buffer returned by the log command. Thread safe. */
    void log (const juce::String& line);

    //==============================================================================
    // Helpers for writing your own commands. All of these must be called on the message thread.

    /** Returns every component matching a selector. root == nullptr searches all desktop windows. */
    static juce::Array<juce::Component*> findComponents (const juce::String& selector, bool visibleOnly = true, juce::Component* root = nullptr);
    /** Returns the first component matching a selector, or nullptr. */
    static juce::Component* findComponent (const juce::String& selector, bool visibleOnly = true, juce::Component* root = nullptr);

    /** Describes a component as a json object, including fields from the registered providers. */
    juce::var describeComponent (juce::Component&, bool full = false);
    /** Describes a component and its descendants. depth < 0 means unlimited. */
    juce::var describeTree (juce::Component&, int depth = -1, bool visibleOnly = true, int maxNodes = 5000);

    /** Demangled class name of a component, e.g. "juce::TextButton". */
    static juce::String getClassName (juce::Component&);
    /** Index path of a component, e.g. "/0/3/1". The first index is the desktop window. */
    static juce::String getComponentPath (juce::Component&);
    /** Text shown by a component if it is a button, label, text editor or combo box. */
    static juce::String getComponentText (juce::Component&);
    /** Value of a component if it is a slider, toggle button, combo box, etc. Returns void var if none. */
    static juce::var getComponentValue (juce::Component&);
    /** Sets the value of a standard component. Returns an error message on failure. */
    static juce::String setComponentValue (juce::Component&, const juce::var& value);

    /** Reads an argument with a default. Accepts missing args object. */
    static juce::var getArg (const juce::var& args, const char* name, const juce::var& defaultValue = {});

    //==============================================================================
    /** Synthesises a mouse event through the component peer, so it travels the same route as a real one.
        pos is in screen coordinates. Pass the button in mods for a press. */
    static bool injectMouse (juce::Point<float> screenPos, juce::ModifierKeys mods, juce::Component* target = nullptr);
    static bool injectWheel (juce::Point<float> screenPos, float deltaX, float deltaY, juce::ModifierKeys mods, juce::Component* target = nullptr);
    /** Sends a key press to the focused peer, or the peer owning target. */
    static bool injectKey (const juce::KeyPress&, juce::Component* target = nullptr);

    /** The deepest component under a screen position, across all desktop windows. Unlike
        Desktop::findComponentAt this ignores windows belonging to other applications, so it
        works when the window being driven is behind a terminal. */
    static juce::Component* getComponentAt (juce::Point<int> screenPos);
    /** The peer under a screen position, if any. */
    static juce::ComponentPeer* getPeerAt (juce::Point<int> screenPos);
    /** The focused peer, falling back to the first one. */
    static juce::ComponentPeer* getFocusedPeer();

private:
    //==============================================================================
    struct Command
    {
        juce::String name, description, argsSpec;
        CommandHandler handler;
        AsyncCommandHandler asyncHandler;
    };

    class CapturingLogger;

    void run() override;
    void handleClient (juce::StreamingSocket&);
    juce::String processRequest (const juce::String& line);
    CommandResult runCommand (const juce::String& name, const juce::var& args);

    CommandResult runOnMessageThread (std::function<CommandResult()>);
    bool sleepOnServerThread (int ms);

    void addBuiltInCommands();
    void writeDiscoveryFile();
    void removeDiscoveryFile();
    juce::String getAppName() const;

    static juce::ComponentPeer* resolvePeer (juce::Point<float> screenPos, juce::Component* target);
    static juce::Array<juce::Component*> findComponents (const juce::StringArray& steps, bool visibleOnly, juce::Component* root);

    //==============================================================================
    Options options;
    std::atomic<bool> running { false };
    int boundPort = 0;

    std::unique_ptr<juce::StreamingSocket> listener;
    std::unique_ptr<juce::StreamingSocket> client;

    juce::CriticalSection commandLock;
    std::map<juce::String, Command> commands;
    std::vector<ComponentInfoProvider> infoProviders;

    juce::CriticalSection logLock;
    juce::StringArray logLines;
    std::unique_ptr<CapturingLogger> capturingLogger;

    int screenshotCounter = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RemoteServer)
};
