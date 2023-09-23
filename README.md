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
routes
which have special meanings:

| Requested Field | Purpose                                 | Example              |
|-----------------|-----------------------------------------|----------------------|
| `[root]`        | The root directory for all target files | `[root]:public_html` |