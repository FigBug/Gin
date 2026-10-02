/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
class RemoteServer::CapturingLogger : public juce::Logger
{
public:
    CapturingLogger (RemoteServer& s) : server (s)
    {
        previous = juce::Logger::getCurrentLogger();
        juce::Logger::setCurrentLogger (this);
    }

    ~CapturingLogger() override
    {
        if (juce::Logger::getCurrentLogger() == this)
            juce::Logger::setCurrentLogger (previous);
    }

    void logMessage (const juce::String& message) override
    {
        server.log (message);

        if (previous != nullptr)
        {
            // logMessage is protected, so reach it through a derived type that re-exports it
            struct Access : public juce::Logger { using juce::Logger::logMessage; };
            (previous->*(&Access::logMessage)) (message);
        }
    }

private:
    RemoteServer& server;
    juce::Logger* previous = nullptr;
};

//==============================================================================
static int getProcessId()
{
   #if JUCE_WINDOWS
    return int (GetCurrentProcessId());
   #else
    return int (getpid());
   #endif
}

static juce::File getDiscoveryDirectory()
{
    // Must agree with tempfile.gettempdir() in the python client, so don't use
    // juce::File::tempDirectory which is per application on macOS
    for (auto name : { "TMPDIR", "TEMP", "TMP" })
    {
        auto v = juce::SystemStats::getEnvironmentVariable (name, {});
        if (v.isNotEmpty() && juce::File::isAbsolutePath (v) && juce::File (v).isDirectory())
            return juce::File (v).getChildFile ("gin_remote");
    }

   #if JUCE_WINDOWS
    return juce::File::getSpecialLocation (juce::File::tempDirectory).getChildFile ("gin_remote");
   #else
    return juce::File ("/tmp/gin_remote");
   #endif
}

//==============================================================================
RemoteServer::RemoteServer() : RemoteServer (Options()) {}

RemoteServer::RemoteServer (const Options& o)
    : juce::Thread ("gin::RemoteServer"), options (o)
{
    moveRealCursor = options.moveRealCursor;
    addBuiltInCommands();
}

RemoteServer::~RemoteServer()
{
    stop();
}

bool RemoteServer::start()
{
    if (running)
        return true;

    auto env = juce::SystemStats::getEnvironmentVariable ("GIN_REMOTE_PORT", {});
    int firstPort = env.isNotEmpty() ? env.getIntValue() : options.port;
    int range = juce::jmax (1, options.portRange);

    listener = std::make_unique<juce::StreamingSocket>();

    for (int p = firstPort; p < firstPort + range; p++)
    {
        if (listener->createListener (p, options.bindAddress))
        {
            boundPort = listener->getBoundPort() > 0 ? listener->getBoundPort() : p;
            break;
        }
    }

    if (! listener->isConnected())
    {
        listener.reset();
        boundPort = 0;
        return false;
    }

    if (options.captureLogger)
        capturingLogger = std::make_unique<CapturingLogger> (*this);

    running = true;
    startThread();

    if (options.writeDiscoveryFile)
        writeDiscoveryFile();

    log ("gin::RemoteServer listening on " + options.bindAddress + ":" + juce::String (boundPort));
    return true;
}

void RemoteServer::stop()
{
    if (! running)
        return;

    running = false;
    signalThreadShouldExit();

    if (client != nullptr)
        client->close();
    if (listener != nullptr)
        listener->close();

    stopThread (5000);

    client.reset();
    listener.reset();
    capturingLogger.reset();

    removeDiscoveryFile();
    boundPort = 0;
}

//==============================================================================
void RemoteServer::addCommand (const juce::String& name, const juce::String& description, const juce::String& argsSpec, CommandHandler handler)
{
    juce::ScopedLock sl (commandLock);
    commands[name] = { name, description, argsSpec, std::move (handler), nullptr };
}

void RemoteServer::addAsyncCommand (const juce::String& name, const juce::String& description, const juce::String& argsSpec, AsyncCommandHandler handler)
{
    juce::ScopedLock sl (commandLock);
    commands[name] = { name, description, argsSpec, nullptr, std::move (handler) };
}

void RemoteServer::removeCommand (const juce::String& name)
{
    juce::ScopedLock sl (commandLock);
    commands.erase (name);
}

void RemoteServer::addComponentInfoProvider (ComponentInfoProvider p)
{
    juce::ScopedLock sl (commandLock);
    infoProviders.push_back (std::move (p));
}

void RemoteServer::log (const juce::String& line)
{
    juce::ScopedLock sl (logLock);
    logLines.add (juce::Time::getCurrentTime().toString (false, true, true, true) + " " + line);

    while (logLines.size() > 2000)
        logLines.remove (0);
}

//==============================================================================
juce::String RemoteServer::getAppName() const
{
    if (options.appName.isNotEmpty())
        return options.appName;

    if (auto app = juce::JUCEApplicationBase::getInstance())
        return app->getApplicationName();

   #ifdef JucePlugin_Name
    return JucePlugin_Name;
   #else
    return juce::File::getSpecialLocation (juce::File::currentExecutableFile).getFileNameWithoutExtension();
   #endif
}

void RemoteServer::writeDiscoveryFile()
{
    auto dir = getDiscoveryDirectory();
    dir.createDirectory();

    auto obj = new juce::DynamicObject();
    obj->setProperty ("app", getAppName());
    obj->setProperty ("pid", getProcessId());
    obj->setProperty ("port", boundPort);
    obj->setProperty ("host", options.bindAddress);
    obj->setProperty ("started", juce::Time::getCurrentTime().toISO8601 (true));

    dir.getChildFile (juce::String (boundPort) + ".json").replaceWithText (juce::JSON::toString (juce::var (obj)));
}

void RemoteServer::removeDiscoveryFile()
{
    if (boundPort > 0)
        getDiscoveryDirectory().getChildFile (juce::String (boundPort) + ".json").deleteFile();
}

//==============================================================================
void RemoteServer::run()
{
    while (! threadShouldExit() && listener != nullptr)
    {
        if (listener->waitUntilReady (true, 100) <= 0)
            continue;

        if (threadShouldExit())
            break;

        std::unique_ptr<juce::StreamingSocket> newClient (listener->waitForNextConnection());
        if (newClient == nullptr)
            continue;

        client = std::move (newClient);
        handleClient (*client);
        client.reset();
    }
}

// A non blocking read (which is what we use so the thread can exit) leaves the socket
// non blocking, so a big response fills the kernel buffer and send() fails with EAGAIN.
// Wait for space and carry on rather than truncating the reply.
bool RemoteServer::writeAll (juce::StreamingSocket& socket, const juce::String& text)
{
    auto utf8 = text.toRawUTF8();
    auto len = int (strlen (utf8));
    int written = 0;
    int idleMs = 0;

    while (written < len && socket.isConnected() && ! threadShouldExit())
    {
        auto ready = socket.waitUntilReady (false, 100);
        if (ready < 0)
            return false;

        if (ready == 0)
        {
            idleMs += 100;
            if (idleMs > 30000)
                return false; // client stopped reading
            continue;
        }

        auto w = socket.write (utf8 + written, len - written);
        if (w > 0)
        {
            written += w;
            idleMs = 0;
        }
        else if (w < 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
        {
            return false;
        }
    }

    return written == len;
}

void RemoteServer::handleClient (juce::StreamingSocket& socket)
{
    juce::MemoryBlock pending;
    char buffer[8192];

    while (! threadShouldExit() && socket.isConnected())
    {
        auto ready = socket.waitUntilReady (true, 100);
        if (ready < 0)
            break;
        if (ready == 0)
            continue;

        auto n = socket.read (buffer, sizeof (buffer), false);
        if (n <= 0)
            break;

        pending.append (buffer, size_t (n));

        for (;;)
        {
            auto* data = static_cast<const char*> (pending.getData());
            auto size = pending.getSize();
            auto* nl = static_cast<const char*> (memchr (data, '\n', size));
            if (nl == nullptr)
                break;

            auto lineLen = size_t (nl - data);
            auto line = juce::String::fromUTF8 (data, int (lineLen)).trim();
            pending.removeSection (0, lineLen + 1);

            if (line.isEmpty())
                continue;

            auto response = processRequest (line) + "\n";
            if (! writeAll (socket, response))
                return;
        }
    }
}

juce::String RemoteServer::processRequest (const juce::String& line)
{
    auto request = juce::JSON::parse (line);
    auto response = new juce::DynamicObject();
    juce::var responseVar (response);

    if (! request.isObject())
    {
        response->setProperty ("ok", false);
        response->setProperty ("error", "Request is not a json object");
        return juce::JSON::toString (responseVar, true);
    }

    if (request.hasProperty ("id"))
        response->setProperty ("id", request["id"]);

    auto cmd = request["cmd"].toString();
    auto args = request.getProperty ("args", juce::var());

    auto result = runCommand (cmd, args);

    response->setProperty ("ok", result.ok());
    if (result.ok())
        response->setProperty ("result", result.value);
    else
        response->setProperty ("error", result.error);

    return juce::JSON::toString (responseVar, true);
}

RemoteServer::CommandResult RemoteServer::runCommand (const juce::String& name, const juce::var& args)
{
    Command command;

    {
        juce::ScopedLock sl (commandLock);
        auto itr = commands.find (name);
        if (itr == commands.end())
            return CommandResult::fail ("Unknown command '" + name + "'. Use the commands command to list them.");
        command = itr->second;
    }

    if (command.asyncHandler)
    {
        Context ctx (*this);
        return command.asyncHandler (ctx, args);
    }

    return runOnMessageThread ([handler = command.handler, args] { return handler (args); });
}

RemoteServer::CommandResult RemoteServer::runOnMessageThread (std::function<CommandResult()> fn)
{
    auto mm = juce::MessageManager::getInstanceWithoutCreating();
    if (mm == nullptr)
        return CommandResult::fail ("No message manager");

    if (mm->isThisTheMessageThread())
        return fn();

    struct State
    {
        juce::CriticalSection lock;
        juce::WaitableEvent done;
        CommandResult result;
        bool cancelled = false;
    };

    auto state = std::make_shared<State>();

    juce::MessageManager::callAsync ([state, func = std::move (fn)]
    {
        juce::ScopedLock sl (state->lock);
        if (! state->cancelled)
            state->result = func();
        state->done.signal();
    });

    if (! state->done.wait (options.messageThreadTimeoutMs))
    {
        juce::ScopedLock sl (state->lock);
        if (! state->done.wait (0))
        {
            state->cancelled = true;
            return CommandResult::fail ("Message thread did not respond within " + juce::String (options.messageThreadTimeoutMs) + "ms. Is a native modal dialog open?");
        }
    }

    return state->result;
}

bool RemoteServer::sleepOnServerThread (int ms)
{
    auto end = juce::Time::getMillisecondCounter() + juce::uint32 (juce::jmax (0, ms));

    while (! threadShouldExit())
    {
        auto now = juce::Time::getMillisecondCounter();
        if (now >= end)
            return true;

        juce::Thread::sleep (int (juce::jmin (juce::uint32 (10), end - now)));
    }

    return false;
}

//==============================================================================
juce::var RemoteServer::getArg (const juce::var& args, const char* name, const juce::var& defaultValue)
{
    if (args.isObject() && args.hasProperty (name))
        return args[name];
    return defaultValue;
}

juce::String RemoteServer::getClassName (juce::Component& c)
{
    // demangling is slow enough to matter on a tree with thousands of components, so cache per type
    static juce::CriticalSection lock;
    static std::unordered_map<std::type_index, juce::String> cache;

    const std::type_index key (typeid (c));

    {
        juce::ScopedLock sl (lock);
        if (auto itr = cache.find (key); itr != cache.end())
            return itr->second;
    }

    juce::String res;

   #if ! JUCE_WINDOWS
    int status = 0;
    if (char* demangled = abi::__cxa_demangle (typeid (c).name(), nullptr, nullptr, &status))
    {
        res = juce::String (demangled);
        free (demangled);
    }
    else
    {
        res = typeid (c).name();
    }
   #else
    res = typeid (c).name();
    if (res.startsWith ("class ")) res = res.substring (6);
    if (res.startsWith ("struct ")) res = res.substring (7);
   #endif

    juce::ScopedLock sl (lock);
    cache[key] = res;
    return res;
}

juce::String RemoteServer::getComponentPath (juce::Component& c)
{
    juce::StringArray parts;
    auto* comp = &c;

    while (auto parent = comp->getParentComponent())
    {
        parts.insert (0, juce::String (parent->getIndexOfChildComponent (comp)));
        comp = parent;
    }

    auto& desktop = juce::Desktop::getInstance();
    int windowIndex = -1;
    for (int i = 0; i < desktop.getNumComponents(); i++)
        if (desktop.getComponent (i) == comp)
            windowIndex = i;

    parts.insert (0, juce::String (windowIndex));
    return "/" + parts.joinIntoString ("/");
}

juce::String RemoteServer::getComponentText (juce::Component& c)
{
    if (auto b = dynamic_cast<juce::Button*> (&c))        return b->getButtonText();
    if (auto l = dynamic_cast<juce::Label*> (&c))         return l->getText();
    if (auto t = dynamic_cast<juce::TextEditor*> (&c))    return t->getText();
    if (auto cb = dynamic_cast<juce::ComboBox*> (&c))     return cb->getText();
    return {};
}

juce::var RemoteServer::getComponentValue (juce::Component& c)
{
    if (auto s = dynamic_cast<juce::Slider*> (&c))
    {
        if (s->isTwoValue() || s->isThreeValue())
        {
            juce::Array<juce::var> vals;
            vals.add (s->getMinValue());
            if (s->isThreeValue())
                vals.add (s->getValue());
            vals.add (s->getMaxValue());
            return vals;
        }
        return s->getValue();
    }
    if (auto b = dynamic_cast<juce::Button*> (&c))
    {
        if (b->getClickingTogglesState() || b->getToggleState() || dynamic_cast<juce::ToggleButton*> (b) != nullptr)
            return b->getToggleState();
        return {};
    }
    if (auto cb = dynamic_cast<juce::ComboBox*> (&c))     return cb->getSelectedId();
    if (auto t = dynamic_cast<juce::TextEditor*> (&c))    return t->getText();
    if (auto l = dynamic_cast<juce::Label*> (&c))         return l->getText();

    // Accessibility value interfaces are deliberately not consulted here: creating handlers
    // for every component makes a big tree very slow. describe (full) reports them.
    return {};
}

juce::String RemoteServer::setComponentValue (juce::Component& c, const juce::var& value)
{
    if (auto s = dynamic_cast<juce::Slider*> (&c))
    {
        if (value.isArray())
        {
            auto& arr = *value.getArray();
            if (s->isThreeValue() && arr.size() >= 3)
            {
                s->setMinValue (double (arr[0]), juce::sendNotificationSync);
                s->setValue (double (arr[1]), juce::sendNotificationSync);
                s->setMaxValue (double (arr[2]), juce::sendNotificationSync);
            }
            else if (arr.size() >= 2)
            {
                s->setMinValue (double (arr[0]), juce::sendNotificationSync);
                s->setMaxValue (double (arr[1]), juce::sendNotificationSync);
            }
            else
            {
                return "Slider expects a number or an array of numbers";
            }
        }
        else if (value.isString())
        {
            s->setValue (s->getValueFromText (value.toString()), juce::sendNotificationSync);
        }
        else
        {
            s->setValue (double (value), juce::sendNotificationSync);
        }
        return {};
    }
    if (auto b = dynamic_cast<juce::Button*> (&c))
    {
        if (value.isBool() || value.isInt() || value.isDouble())
            b->setToggleState (bool (value), juce::sendNotificationSync);
        else
            b->triggerClick();
        return {};
    }
    if (auto cb = dynamic_cast<juce::ComboBox*> (&c))
    {
        if (value.isString())
        {
            auto text = value.toString();
            for (int i = 0; i < cb->getNumItems(); i++)
            {
                if (cb->getItemText (i) == text)
                {
                    cb->setSelectedItemIndex (i, juce::sendNotificationSync);
                    return {};
                }
            }
            return "ComboBox has no item '" + text + "'";
        }
        cb->setSelectedId (int (value), juce::sendNotificationSync);
        return {};
    }
    if (auto t = dynamic_cast<juce::TextEditor*> (&c))
    {
        t->setText (value.toString(), juce::sendNotification);
        return {};
    }
    if (auto l = dynamic_cast<juce::Label*> (&c))
    {
        l->setText (value.toString(), juce::sendNotificationSync);
        return {};
    }

    if (auto handler = c.getAccessibilityHandler())
    {
        if (auto vi = handler->getValueInterface())
        {
            if (vi->isReadOnly())
                return "Value is read only";
            if (value.isString())
                vi->setValueAsString (value.toString());
            else
                vi->setValue (double (value));
            return {};
        }
    }

    return "Don't know how to set a value on " + getClassName (c);
}

//==============================================================================
juce::Component* RemoteServer::getComponentAt (juce::Point<int> screenPos)
{
    auto& desktop = juce::Desktop::getInstance();

    for (int i = desktop.getNumComponents(); --i >= 0;)
    {
        auto c = desktop.getComponent (i);
        if (c == nullptr || ! c->isVisible())
            continue;

        auto local = c->getLocalPoint (nullptr, screenPos);
        if (c->getLocalBounds().contains (local))
            if (auto hit = c->getComponentAt (local))
                return hit;
    }

    return nullptr;
}

juce::ComponentPeer* RemoteServer::getPeerAt (juce::Point<int> screenPos)
{
    if (auto c = getComponentAt (screenPos))
        if (auto peer = c->getPeer())
            return peer;

    for (int i = juce::ComponentPeer::getNumPeers(); --i >= 0;)
    {
        auto peer = juce::ComponentPeer::getPeer (i);
        if (peer->getComponent().isVisible() && peer->getBounds().contains (screenPos))
            return peer;
    }

    return nullptr;
}

juce::ComponentPeer* RemoteServer::getFocusedPeer()
{
    for (int i = 0; i < juce::ComponentPeer::getNumPeers(); i++)
        if (auto peer = juce::ComponentPeer::getPeer (i))
            if (peer->isFocused())
                return peer;

    if (auto c = juce::Component::getCurrentlyFocusedComponent())
        if (auto peer = c->getPeer())
            return peer;

    return juce::ComponentPeer::getNumPeers() > 0 ? juce::ComponentPeer::getPeer (0) : nullptr;
}

juce::ComponentPeer* RemoteServer::resolvePeer (juce::Point<float> screenPos, juce::Component* target)
{
    if (target != nullptr)
        if (auto peer = target->getPeer())
            return peer;

    return getPeerAt (screenPos.toInt());
}

// JUCE drops mouse events for a window that another application's window covers at that
// point, so if the window we are driving sits behind a terminal nothing would happen.
// Raise it when that is the case, and give the window server a moment to catch up.
static bool ensurePeerIsHittable (juce::ComponentPeer& peer, juce::Point<float> screenPos)
{
    auto local = peer.globalToLocal (screenPos).toInt();

    if (peer.contains (local, true) || ! peer.getBounds().contains (screenPos.toInt()))
        return false;

    // Ordering the window to the front is not enough when another application is active,
    // so make this the foreground process too. That is what a user clicking it would do.
    juce::Process::makeForegroundProcess();
    peer.toFront (true);

    for (int i = 0; i < 200 && ! peer.contains (local, true); i++)
        juce::Thread::sleep (5);

    return true;
}

bool RemoteServer::injectMouse (juce::Point<float> screenPos, juce::ModifierKeys mods, juce::Component* target)
{
    auto peer = resolvePeer (screenPos, target);
    if (peer == nullptr)
        return false;

    // Only warp while no button is held: the warp posts a real mouse moved event, and one
    // of those arriving mid press (with no button in its modifiers) reads as a release.
    if (moveRealCursor && ! mods.isAnyMouseButtonDown() && juce::Desktop::getMousePosition().getDistanceFrom (screenPos.toInt()) > 1)
        juce::Desktop::setMousePosition (screenPos.toInt());

    // If the window had to be raised, JUCE has no idea what is under the mouse yet. A plain
    // move fixes that, otherwise a press that arrives first would be dropped.
    if (ensurePeerIsHittable (*peer, screenPos))
        peer->handleMouseEvent (juce::MouseInputSource::InputSourceType::mouse,
                                peer->globalToLocal (screenPos),
                                mods.withoutMouseButtons(),
                                juce::MouseInputSource::defaultPressure,
                                juce::MouseInputSource::defaultOrientation,
                                juce::Time::currentTimeMillis());

    peer->handleMouseEvent (juce::MouseInputSource::InputSourceType::mouse,
                            peer->globalToLocal (screenPos),
                            mods,
                            juce::MouseInputSource::defaultPressure,
                            juce::MouseInputSource::defaultOrientation,
                            juce::Time::currentTimeMillis());
    return true;
}

bool RemoteServer::injectWheel (juce::Point<float> screenPos, float deltaX, float deltaY, juce::ModifierKeys mods, juce::Component* target)
{
    auto peer = resolvePeer (screenPos, target);
    if (peer == nullptr)
        return false;

    if (moveRealCursor && ! mods.isAnyMouseButtonDown() && juce::Desktop::getMousePosition().getDistanceFrom (screenPos.toInt()) > 1)
        juce::Desktop::setMousePosition (screenPos.toInt());

    ensurePeerIsHittable (*peer, screenPos);

    // move first so the wheel lands on the right component with the right modifiers
    peer->handleMouseEvent (juce::MouseInputSource::InputSourceType::mouse, peer->globalToLocal (screenPos), mods,
                            juce::MouseInputSource::defaultPressure, juce::MouseInputSource::defaultOrientation,
                            juce::Time::currentTimeMillis());

    juce::MouseWheelDetails details;
    details.deltaX = deltaX;
    details.deltaY = deltaY;
    details.isReversed = false;
    details.isSmooth = false;
    details.isInertial = false;

    peer->handleMouseWheel (juce::MouseInputSource::InputSourceType::mouse, peer->globalToLocal (screenPos),
                            juce::Time::currentTimeMillis(), details);
    return true;
}

bool RemoteServer::injectKey (const juce::KeyPress& key, juce::Component* target)
{
    juce::ComponentPeer* peer = nullptr;

    if (target != nullptr)
        peer = target->getPeer();
    if (peer == nullptr)
        peer = getFocusedPeer();
    if (peer == nullptr)
        return false;

    peer->handleKeyUpOrDown (true);
    peer->handleKeyPress (key);
    peer->handleKeyUpOrDown (false);
    return true;
}
