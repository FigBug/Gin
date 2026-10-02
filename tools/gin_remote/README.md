# gin_remote

Drive a JUCE UI from outside the process. The `gin_remote` module embeds a small
line-delimited JSON server in your app or plugin; `gin_remote.py` is a command line
client and an MCP server for AI coding agents such as Claude Code.

## In the app

```cpp
#include <gin_remote/gin_remote.h>

// keep it alive for the life of the app / plugin instance
std::unique_ptr<gin::RemoteServer> remote;

#if MY_DEVELOPMENT_BUILD
remote = std::make_unique<gin::RemoteServer>();

// optional: product specific commands
remote->addCommand ("loadPreset", "Load a preset by name", R"({"name":"string!: preset name"})",
                    [this] (const juce::var& args)
                    {
                        if (! loadPreset (args["name"].toString()))
                            return gin::RemoteServer::CommandResult::fail ("No such preset");
                        return gin::RemoteServer::CommandResult (juce::var (true));
                    });

// optional: extra fields on every component description
remote->addComponentInfoProvider ([] (juce::Component& c, juce::DynamicObject& info)
{
    if (auto skinned = dynamic_cast<SkinnedComponent*> (&c))
        info.setProperty ("skin", skinned->getSkinKey());
});

// optional, when juce_audio_processors is in the project: params, getParam, setParam, ...
gin::addAudioProcessorCommands (*remote, processor);

remote->start();
#endif
```

Nothing listens unless you create and start a server, so the module is safe to link into
release builds. Only start it in development builds: it accepts unauthenticated commands
from anything on the local machine.

Several instances can run at once. The first one takes port 27182, the next 27183, and so on.
`GIN_REMOTE_PORT` overrides the starting port. Each server writes
`<temp>/gin_remote/<port>.json` so the client can find it.

## From the command line

```
gin_remote.py ls                               running servers
gin_remote.py tree [selector] [--depth N]      component tree, compact text
gin_remote.py find "<selector>"
gin_remote.py describe "<selector>"            everything, including accessibility info
gin_remote.py at X Y                           what is under a screen point
gin_remote.py shot [selector] [--out f.png] [--scale 2] [--highlight sel ...]
gin_remote.py click "<selector>" [--button right] [--count 2] [--mods shift,cmd]
gin_remote.py click --x 100 --y 200            screen coordinates
gin_remote.py click ".ListBox" --x 20 --y 99   offset within the target
gin_remote.py drag "<selector>" --dx 0 --dy -50
gin_remote.py wheel "<selector>" --dy 0.1
gin_remote.py key "cmd+s"
gin_remote.py type "hello"
gin_remote.py get | set | press "<selector>" [value]
gin_remote.py wait "<selector>" [--state visible|exists|gone] [--timeout 5000]
gin_remote.py resize 1200 800
gin_remote.py log
gin_remote.py commands                         including the product's own
gin_remote.py raw <cmd> '{"json": "args"}'
```

Add `--json` for machine readable output, `--port N` or `--app NAME` to pick a server.

### Selectors

A selector is a space separated chain of steps. Each step searches the descendants of the
previous matches.

| step        | matches                                            |
|-------------|----------------------------------------------------|
| `#id`       | `Component::getComponentID()`                      |
| `.Class`    | class name, with or without namespace or template  |
| `name`      | component name, button / label text, or title      |
| `~text`     | substring of any of the above                      |
| `@x,y`      | deepest component at a screen point                |
| `/0/3/1`    | index path: desktop window 0, child 3, child 1     |
| `*`         | anything                                           |
| `step[n]`   | the nth match of that step                         |

`"#sidebar .TextButton[2]"`, `"Save"`, `"@120,240"`. A multi word name such as
`"Component Viewer"` works unquoted: if the chain finds nothing, the whole string is tried
as one name. Every command that takes a target looks at visible components first, then
hidden ones.

## As an MCP server

Add to `.mcp.json` in the project (or `~/.claude.json` for everywhere):

```json
{
  "mcpServers": {
    "gin_remote": {
      "command": "python3",
      "args": ["/path/to/Gin/tools/gin_remote/gin_remote.py", "mcp"]
    }
  }
}
```

Add `"--app", "Nexus"` to the args to pin it to one product when several are running.

Every server command becomes a tool, including the ones your product registers, with
argument schemas built from the `argsSpec` you passed to `addCommand`. Screenshots are
returned as inline images so the agent sees them without any copy and paste. `tree`,
`find` and `at` are returned as compact text to keep token use down.

## Notes

- Mouse input goes through `ComponentPeer::handleMouseEvent`, so it takes the same path as
  a real mouse: hit testing, modal loops, popup menus and drag thresholds all behave.
- JUCE drops mouse events for a window that another application covers at that point. The
  server raises the window before injecting if that is the case, so your app will pop in
  front of the terminal on the first click.
- Keys go to the focused component in the target's window. Use `focus` first if another
  window has focus.
- `log` returns `juce::Logger` output. `DBG` on macOS and Windows writes straight to the
  debugger, not the logger, so call `remote->log()` or `juce::Logger::writeToLog` for lines
  you want to see remotely.
- Screenshots use `createComponentSnapshot`, so OpenGL and native views do not appear.
