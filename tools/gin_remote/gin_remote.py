#!/usr/bin/env python3
"""Client for gin::RemoteServer.

Command line:

    gin_remote.py ls                              # running servers
    gin_remote.py tree [selector] [--depth N]     # component tree, compact text
    gin_remote.py find "<selector>"
    gin_remote.py describe "<selector>"
    gin_remote.py at X Y
    gin_remote.py shot [selector] [--out file.png] [--scale 2] [--highlight sel ...]
    gin_remote.py click "<selector>" [--button right] [--count 2] [--mods shift,cmd]
    gin_remote.py click --x 100 --y 200
    gin_remote.py drag "<selector>" --dx 0 --dy -50
    gin_remote.py wheel "<selector>" --dy 0.1
    gin_remote.py key "cmd+s"
    gin_remote.py type "hello"
    gin_remote.py set "<selector>" 0.5
    gin_remote.py get "<selector>"
    gin_remote.py press "<selector>"
    gin_remote.py wait "<selector>" [--state visible|exists|gone] [--timeout 5000]
    gin_remote.py log
    gin_remote.py commands
    gin_remote.py raw <cmd> ['{"json": "args"}']
    gin_remote.py mcp                             # run as an MCP server over stdio

Server selection: --port N, or --app NAME (substring match against discovered servers),
or the GIN_REMOTE_PORT / GIN_REMOTE_APP environment variables. With one server running
nothing is needed.

Selectors: #id  .Class  name  ~substring  @x,y  /0/3/1  *   chained with spaces,
and step[n] picks the nth match.  e.g.  "#sidebar .TextButton[2]"

No dependencies beyond the Python standard library.
"""

import argparse
import glob
import json
import os
import socket
import sys
import tempfile

DEFAULT_PORT = 27182
DISCOVERY_DIR = os.path.join(tempfile.gettempdir(), "gin_remote")


# ----------------------------------------------------------------------------
# transport

class RemoteError(Exception):
    pass


class Client:
    def __init__(self, host="127.0.0.1", port=DEFAULT_PORT, timeout=30.0):
        self.host = host
        self.port = port
        self.timeout = timeout
        self.sock = None
        self.buf = b""
        self.next_id = 1

    def connect(self):
        if self.sock is None:
            self.sock = socket.create_connection((self.host, self.port), timeout=self.timeout)
        return self

    def close(self):
        if self.sock is not None:
            try:
                self.sock.close()
            finally:
                self.sock = None

    def call(self, cmd, args=None, timeout=None):
        self.connect()
        req_id = self.next_id
        self.next_id += 1
        payload = {"id": req_id, "cmd": cmd, "args": args or {}}
        self.sock.sendall((json.dumps(payload) + "\n").encode("utf-8"))

        if timeout is not None:
            self.sock.settimeout(timeout)
        try:
            while b"\n" not in self.buf:
                chunk = self.sock.recv(65536)
                if not chunk:
                    raise RemoteError("connection closed by server")
                self.buf += chunk
        finally:
            if timeout is not None:
                self.sock.settimeout(self.timeout)

        line, self.buf = self.buf.split(b"\n", 1)
        resp = json.loads(line.decode("utf-8"))
        if not resp.get("ok"):
            raise RemoteError(resp.get("error", "unknown error"))
        return resp.get("result")


def discover():
    """Returns a list of {app, pid, port, host} for servers that answer a ping."""
    found = []
    for path in sorted(glob.glob(os.path.join(DISCOVERY_DIR, "*.json"))):
        try:
            with open(path) as f:
                info = json.load(f)
        except Exception:
            continue
        c = Client(info.get("host", "127.0.0.1"), int(info["port"]), timeout=1.0)
        try:
            pong = c.call("ping")
            info.update(pong)
            found.append(info)
        except Exception:
            # stale file from a process that did not shut down cleanly
            try:
                os.remove(path)
            except OSError:
                pass
        finally:
            c.close()
    return found


def pick_server(port=None, app=None, host="127.0.0.1"):
    if port is None:
        env = os.environ.get("GIN_REMOTE_PORT")
        if env:
            port = int(env)
    if app is None:
        app = os.environ.get("GIN_REMOTE_APP")

    if port is not None:
        return Client(host, port)

    servers = discover()
    if app:
        servers = [s for s in servers if app.lower() in str(s.get("app", "")).lower()]
        if not servers:
            raise RemoteError("no running server matches app '%s'" % app)
    if not servers:
        # last resort: try the default port silently
        c = Client(host, DEFAULT_PORT, timeout=1.0)
        try:
            c.call("ping")
            return Client(host, DEFAULT_PORT)
        except Exception:
            raise RemoteError("no gin_remote servers found (is the app running with RemoteServer started?)")
        finally:
            c.close()
    if len(servers) > 1:
        names = ", ".join("%s:%s" % (s.get("app"), s.get("port")) for s in servers)
        raise RemoteError("several servers running, pick one with --port or --app: " + names)
    return Client(servers[0].get("host", host), int(servers[0]["port"]))


# ----------------------------------------------------------------------------
# formatting

def fmt_node(n):
    parts = [n.get("class", "?")]
    if n.get("name"):
        parts.append('"%s"' % n["name"])
    if n.get("id"):
        parts.append("#" + n["id"])
    b = n.get("bounds")
    if b:
        parts.append("[%d,%d %dx%d]" % (b[0], b[1], b[2], b[3]))
    if n.get("text") and n.get("text") != n.get("name"):
        parts.append("text=%r" % n["text"])
    if "value" in n:
        parts.append("value=%s" % json.dumps(n["value"]))
    flags = []
    if n.get("visible") is False:
        flags.append("hidden")
    if n.get("enabled") is False:
        flags.append("disabled")
    if n.get("focused"):
        flags.append("focused")
    if n.get("interceptsMouse"):
        flags.append("mouse=%s" % "".join("1" if x else "0" for x in n["interceptsMouse"]))
    if n.get("alpha") is not None:
        flags.append("alpha=%.2f" % n["alpha"])
    if n.get("tooltip"):
        flags.append("tip=%r" % n["tooltip"])
    if flags:
        parts.append("(" + " ".join(flags) + ")")
    extra = {k: v for k, v in n.items() if k not in (
        "class", "name", "id", "bounds", "screen", "text", "value", "visible", "enabled",
        "focused", "interceptsMouse", "alpha", "tooltip", "children", "childCount", "path",
        "opaque", "title", "truncated", "local", "index")}
    if extra:
        parts.append(json.dumps(extra, separators=(",", ":")))
    return " ".join(parts)


def print_tree(node, indent=0, out=sys.stdout):
    nodes = node if isinstance(node, list) else [node]
    for n in nodes:
        out.write("  " * indent + n.get("path", "") + " " + fmt_node(n) + "\n")
        for c in n.get("children", []):
            print_tree(c, indent + 1, out)
        if n.get("truncated"):
            out.write("  " * (indent + 1) + "... truncated, raise --max-nodes or narrow the target\n")


def print_json(v):
    print(json.dumps(v, indent=2))


# ----------------------------------------------------------------------------
# cli

def parse_mods(s):
    if not s:
        return None
    return [m for m in s.replace("+", ",").split(",") if m]


def parse_value(s):
    try:
        return json.loads(s)
    except Exception:
        return s


def cli(argv):
    p = argparse.ArgumentParser(description="gin::RemoteServer client")
    p.add_argument("--port", type=int)
    p.add_argument("--app")
    p.add_argument("--host", default="127.0.0.1")
    p.add_argument("--json", action="store_true", help="json output where a text form exists")
    sub = p.add_subparsers(dest="command")

    sub.add_parser("ls", help="list running servers")
    sub.add_parser("ping")
    sub.add_parser("commands")
    sub.add_parser("windows")

    s = sub.add_parser("tree")
    s.add_argument("target", nargs="?", default="")
    s.add_argument("--depth", type=int, default=-1)
    s.add_argument("--all", action="store_true", help="include hidden components")
    s.add_argument("--max-nodes", type=int, default=5000)

    s = sub.add_parser("find")
    s.add_argument("selector")
    s.add_argument("--all", action="store_true")

    s = sub.add_parser("describe")
    s.add_argument("target")

    s = sub.add_parser("at")
    s.add_argument("x", type=int)
    s.add_argument("y", type=int)

    s = sub.add_parser("shot", aliases=["screenshot"])
    s.add_argument("target", nargs="?", default="")
    s.add_argument("--out")
    s.add_argument("--scale", type=float, default=1.0)
    s.add_argument("--region", help="x,y,w,h within target")
    s.add_argument("--highlight", nargs="*", default=None)
    s.add_argument("--no-labels", action="store_true")

    for name in ("click", "drag", "wheel", "mouse"):
        s = sub.add_parser(name)
        s.add_argument("target", nargs="?", default="")
        s.add_argument("--x", type=float)
        s.add_argument("--y", type=float)
        s.add_argument("--button", default="left")
        s.add_argument("--mods")
        if name == "click":
            s.add_argument("--count", type=int, default=1)
        if name == "drag":
            s.add_argument("--dx", type=float, default=0)
            s.add_argument("--dy", type=float, default=0)
            s.add_argument("--to-x", type=float)
            s.add_argument("--to-y", type=float)
            s.add_argument("--steps", type=int, default=12)
            s.add_argument("--duration", type=int, default=250)
        if name == "wheel":
            s.add_argument("--dx", type=float, default=0)
            s.add_argument("--dy", type=float, default=0.1)
        if name == "mouse":
            s.add_argument("--action", default="move", choices=["move", "down", "up"])

    s = sub.add_parser("key")
    s.add_argument("key")
    s.add_argument("--target", default="")

    s = sub.add_parser("type")
    s.add_argument("text")
    s.add_argument("--target", default="")

    s = sub.add_parser("focus")
    s.add_argument("target")

    s = sub.add_parser("get")
    s.add_argument("target")

    s = sub.add_parser("set")
    s.add_argument("target")
    s.add_argument("value")

    s = sub.add_parser("press")
    s.add_argument("target")

    s = sub.add_parser("resize")
    s.add_argument("width", type=int)
    s.add_argument("height", type=int)
    s.add_argument("--target", default="/0")

    s = sub.add_parser("wait")
    s.add_argument("selector")
    s.add_argument("--state", default="visible", choices=["visible", "exists", "gone"])
    s.add_argument("--timeout", type=int, default=5000)

    s = sub.add_parser("log")
    s.add_argument("--keep", action="store_true", help="don't clear the buffer")
    s.add_argument("--tail", type=int, default=0)

    s = sub.add_parser("raw")
    s.add_argument("cmd")
    s.add_argument("args", nargs="?", default="{}")

    s = sub.add_parser("mcp", help="run as an MCP server on stdio")
    s.add_argument("--port", type=int, dest="mcp_port")
    s.add_argument("--app", dest="mcp_app")

    # anything that isn't a built in subcommand is sent to the server as is,
    # so product specific commands work without the raw prefix: gin_remote.py loadPreset '{"preset":"x"}'
    known = set(sub.choices.keys()) | {"-h", "--help"}
    takes_value = {"--port", "--app", "--host"}
    i = 0
    while i < len(argv):
        tok = argv[i]
        if tok in takes_value:
            i += 2
            continue
        if tok.startswith("-"):
            i += 1
            continue
        if tok not in known:
            argv = argv[:i] + ["raw"] + argv[i:]
        break

    a = p.parse_args(argv)
    if a.command is None:
        p.print_help()
        return 1

    if a.command == "ls":
        servers = discover()
        if not servers:
            print("no servers running")
        for s_ in servers:
            print("%-30s port %-6s pid %-7s juce %s" % (s_.get("app"), s_.get("port"), s_.get("pid"), s_.get("juce", "?")))
        return 0

    if a.command == "mcp":
        return mcp_main(port=a.port or a.mcp_port, app=a.app or a.mcp_app, host=a.host)

    c = pick_server(a.port, a.app, a.host)
    try:
        return run_cli_command(c, a)
    except RemoteError as e:
        print("error: %s" % e, file=sys.stderr)
        return 2
    finally:
        c.close()


def point_args(a):
    args = {}
    if a.target:
        args["target"] = a.target
    if a.x is not None:
        args["x"] = a.x
    if a.y is not None:
        args["y"] = a.y
    if getattr(a, "button", None):
        args["button"] = a.button
    mods = parse_mods(getattr(a, "mods", None))
    if mods:
        args["mods"] = mods
    return args


def run_cli_command(c, a):
    cmd = a.command

    if cmd in ("ping", "commands", "windows"):
        r = c.call(cmd)
        if cmd == "commands" and not a.json:
            for item in r:
                args = item.get("args") or {}
                argtxt = ", ".join("%s(%s)" % (k, v.split(":")[0]) for k, v in args.items()) if isinstance(args, dict) else ""
                print("%-14s %s" % (item["name"], item["description"]))
                if argtxt:
                    print("%-14s   args: %s" % ("", argtxt))
        elif cmd == "windows" and not a.json:
            for w in r:
                print(w.get("path", ""), fmt_node(w), "peer" if w.get("hasPeer") else "", "focused" if w.get("peerFocused") else "", "modal" if w.get("modal") else "")
        else:
            print_json(r)
        return 0

    if cmd == "tree":
        r = c.call("tree", {"target": a.target, "depth": a.depth, "visibleOnly": not a.all, "maxNodes": a.max_nodes})
        print_json(r) if a.json else print_tree(r)
        return 0

    if cmd == "find":
        r = c.call("find", {"selector": a.selector, "visibleOnly": not a.all})
        if a.json:
            print_json(r)
        else:
            for n in r:
                print(n.get("path", ""), fmt_node(n), "screen=%s" % n.get("screen"))
            if not r:
                print("no matches")
        return 0

    if cmd == "describe":
        print_json(c.call("describe", {"target": a.target}))
        return 0

    if cmd == "at":
        r = c.call("at", {"x": a.x, "y": a.y})
        if a.json:
            print_json(r)
        else:
            for i, n in enumerate(r):
                print("  " * i + n.get("path", ""), fmt_node(n), "local=%s" % n.get("local"))
        return 0

    if cmd in ("shot", "screenshot"):
        args = {"target": a.target, "scale": a.scale, "labels": not a.no_labels}
        if a.out:
            args["file"] = os.path.abspath(a.out)
        if a.region:
            args["region"] = [int(v) for v in a.region.split(",")]
        if a.highlight is not None:
            args["highlight"] = a.highlight
        r = c.call("screenshot", args, timeout=60)
        if a.json:
            print_json(r)
        else:
            print(r["file"], "%dx%d" % (r["width"], r["height"]))
            for h in r.get("highlighted", []):
                print("  [%d]" % h["index"], h.get("path", ""), fmt_node(h))
        return 0

    if cmd == "click":
        args = point_args(a)
        args["count"] = a.count
        print_json(c.call("click", args))
        return 0

    if cmd == "drag":
        args = point_args(a)
        args.update({"dx": a.dx, "dy": a.dy, "steps": a.steps, "durationMs": a.duration})
        if a.to_x is not None:
            args["toX"] = a.to_x
        if a.to_y is not None:
            args["toY"] = a.to_y
        print_json(c.call("drag", args, timeout=max(30, a.duration / 1000 + 10)))
        return 0

    if cmd == "wheel":
        args = point_args(a)
        args.update({"dx": a.dx, "dy": a.dy})
        print_json(c.call("wheel", args))
        return 0

    if cmd == "mouse":
        args = point_args(a)
        args["action"] = a.action
        print_json(c.call("mouse", args))
        return 0

    if cmd == "key":
        print_json(c.call("key", {"key": a.key, "target": a.target}))
        return 0

    if cmd == "type":
        print_json(c.call("type", {"text": a.text, "target": a.target}))
        return 0

    if cmd == "focus":
        print_json(c.call("focus", {"target": a.target}))
        return 0

    if cmd == "get":
        print_json(c.call("get", {"target": a.target}))
        return 0

    if cmd == "set":
        print_json(c.call("set", {"target": a.target, "value": parse_value(a.value)}))
        return 0

    if cmd == "press":
        print_json(c.call("press", {"target": a.target}))
        return 0

    if cmd == "resize":
        print_json(c.call("resize", {"target": a.target, "width": a.width, "height": a.height}))
        return 0

    if cmd == "wait":
        r = c.call("wait", {"selector": a.selector, "state": a.state, "timeoutMs": a.timeout}, timeout=a.timeout / 1000 + 10)
        print_json(r)
        return 0

    if cmd == "log":
        r = c.call("log", {"clear": not a.keep, "tail": a.tail})
        if a.json:
            print_json(r)
        else:
            for line in r:
                print(line)
        return 0

    if cmd == "raw":
        print_json(c.call(a.cmd, json.loads(a.args), timeout=120))
        return 0

    print("unknown command", cmd, file=sys.stderr)
    return 1


# ----------------------------------------------------------------------------
# MCP server (stdio, newline delimited JSON-RPC)

TYPE_MAP = {
    "string": {"type": "string"},
    "int": {"type": "integer"},
    "number": {"type": "number"},
    "bool": {"type": "boolean"},
    "array": {"type": "array"},
    "object": {"type": "object"},
    "any": {},
}


def spec_to_schema(spec):
    """Converts the server's {"arg": "type!: description"} form to a JSON schema."""
    props = {}
    required = []
    if isinstance(spec, dict):
        for name, desc in spec.items():
            if not isinstance(desc, str):
                continue
            typ, _, text = desc.partition(":")
            typ = typ.strip()
            if typ.endswith("!"):
                typ = typ[:-1]
                required.append(name)
            schema = dict(TYPE_MAP.get(typ, {}))
            if text.strip():
                schema["description"] = text.strip()
            props[name] = schema
    out = {"type": "object", "properties": props}
    if required:
        out["required"] = required
    return out


SELECTOR_HELP = (
    " Selectors: #id, .ClassName, name or button text, ~substring, @x,y screen point, /0/3/1 index path, * any;"
    " chain steps with spaces to search descendants, add [n] to pick the nth match."
)


def mcp_tools(client):
    tools = []
    for cmd in client.call("commands"):
        name = cmd["name"]
        desc = cmd.get("description", "")
        spec = cmd.get("args") or {}
        if isinstance(spec, dict) and any("selector" in str(v) for v in spec.values()):
            desc += SELECTOR_HELP
        if name == "screenshot":
            desc += " The image is returned inline; the file path is also given."
        if name == "tree":
            desc += " Returned as compact indented text, one component per line: path class \"name\" #id [x,y wxh] text value (flags)."
        tools.append({"name": name, "description": desc, "inputSchema": spec_to_schema(spec)})

    tools.append({
        "name": "servers",
        "description": "List running gin_remote servers (apps with a RemoteServer started) and which one this session talks to.",
        "inputSchema": {"type": "object", "properties": {}},
    })
    tools.append({
        "name": "use_server",
        "description": "Switch to a different running server by port or app name substring.",
        "inputSchema": {"type": "object", "properties": {"port": {"type": "integer"}, "app": {"type": "string"}}},
    })
    return tools


def mcp_main(port=None, app=None, host="127.0.0.1"):
    state = {"client": None, "port": port, "app": app, "host": host}

    def client():
        if state["client"] is None:
            state["client"] = pick_server(state["port"], state["app"], state["host"])
        return state["client"]

    def reset_client():
        if state["client"] is not None:
            state["client"].close()
        state["client"] = None

    def text(s):
        return {"content": [{"type": "text", "text": s}]}

    def call_tool(name, args):
        if name == "servers":
            servers = discover()
            cur = state["client"].port if state["client"] else None
            lines = ["%s port %s pid %s%s" % (s.get("app"), s.get("port"), s.get("pid"), "  <- current" if s.get("port") == cur else "") for s in servers]
            return text("\n".join(lines) if lines else "no servers running")

        if name == "use_server":
            reset_client()
            state["port"] = args.get("port")
            state["app"] = args.get("app")
            pong = client().call("ping")
            return text("now talking to %s on port %s" % (pong.get("app"), pong.get("port")))

        timeout = 120
        if name == "wait":
            timeout = args.get("timeoutMs", 5000) / 1000 + 10

        try:
            result = client().call(name, args, timeout=timeout)
        except (OSError, RemoteError) as e:
            # a dead connection: drop it so the next call reconnects
            if isinstance(e, OSError) or "connection closed" in str(e):
                reset_client()
            raise

        if name == "screenshot":
            data = result.pop("base64", None)
            content = [{"type": "text", "text": json.dumps(result)}]
            if data:
                content.insert(0, {"type": "image", "data": data, "mimeType": "image/png"})
            return {"content": content}

        if name == "tree":
            import io
            buf = io.StringIO()
            print_tree(result, out=buf)
            return text(buf.getvalue())

        if name == "find" or name == "at":
            lines = [n.get("path", "") + " " + fmt_node(n) + " screen=" + json.dumps(n.get("screen")) for n in result]
            return text("\n".join(lines) if lines else "no matches")

        if name == "log":
            return text("\n".join(result) if result else "(no log lines)")

        return text(json.dumps(result, indent=2) if not isinstance(result, str) else result)

    def send(msg):
        sys.stdout.write(json.dumps(msg) + "\n")
        sys.stdout.flush()

    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        try:
            msg = json.loads(line)
        except ValueError:
            continue

        method = msg.get("method")
        msg_id = msg.get("id")
        params = msg.get("params") or {}

        if msg_id is None:
            # notification: nothing to answer
            continue

        try:
            if method == "initialize":
                result = {
                    "protocolVersion": params.get("protocolVersion", "2024-11-05"),
                    "capabilities": {"tools": {"listChanged": False}},
                    "serverInfo": {"name": "gin_remote", "version": "1.0.0"},
                }
            elif method == "ping":
                result = {}
            elif method == "tools/list":
                result = {"tools": mcp_tools(client())}
            elif method == "tools/call":
                name = params.get("name")
                args = params.get("arguments") or {}
                if name == "screenshot":
                    args = dict(args)
                    args["base64"] = True
                try:
                    result = call_tool(name, args)
                except (RemoteError, OSError) as e:
                    result = {"content": [{"type": "text", "text": "error: %s" % e}], "isError": True}
            elif method in ("resources/list", "resources/templates/list"):
                result = {"resources": [], "resourceTemplates": []}
            elif method == "prompts/list":
                result = {"prompts": []}
            else:
                send({"jsonrpc": "2.0", "id": msg_id, "error": {"code": -32601, "message": "method not found: %s" % method}})
                continue
            send({"jsonrpc": "2.0", "id": msg_id, "result": result})
        except Exception as e:  # keep the server alive on unexpected errors
            send({"jsonrpc": "2.0", "id": msg_id, "error": {"code": -32000, "message": str(e)}})

    return 0


if __name__ == "__main__":
    sys.exit(cli(sys.argv[1:]))
