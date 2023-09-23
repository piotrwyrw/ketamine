# Ketamine

A very simple HTTP server that you probably shouldn't use because of security or whatever

---

### The `routes` file

By default, ketamine looks for a file called `routes` in the same directory.
As the name suggests, this file specifies the routes for the server. The format is as follows:

```ebnf
<requested> ":" <target>
```

_Note: The parser for the routes file is very simple to enhance stability. It therefore does not support comments,
whitespaces, or other code beauty measures. It really must just be continuous lines of colon-separated key-value
pairs._

The **requested** field is the requested file or path, and **target** is what it should be mapped to.
Usually, the **requested** field corresponds to a regular path. There are, however, some builtin ketamine-intrinsic
routes that have special meanings:

| Requested Field | Purpose                                 | Example              |
|-----------------|-----------------------------------------|----------------------|
| `[root]`        | The root directory for all target files | `[root]:public_html` |

#### Integration

The functionality of the ketamine router during normal operation can be described as it being the middleware between the
request parser and the file I/O.

The following diagram describes the placement, as well as the functionality of the router

```mermaid
flowchart LR
    a[Request Parser] --> b[Router]
    g[Connection Handler Threads] --> a
    e[Route Config Parser] -- Loads Route data --> b
    f[Ketamine Init] --> e
    b --> c[Is Target a Route?]
    c -- Yes --> b
    c -- No --> d[HTTP Response Generator]
    b --> h[Route overrides?]
    h --> c
```