ibt - a better build tool
=========================

The current state of building C/C++ projects sucks.
`ibt` tries to make it better.


Building
--------

The `ibt` command is designed to be built by itself.
To get setup, there is a bit of a bootstrapping process:

```sh
# Build an initial ibt into build.strap/
$ script/strap.sh
# Use ibt to build itself
$ build.strap/ibt
```

The resulting `build/ibt` should now be ready.
When hacking on ibt,
it may be useful to run `mv build build.stable` from a working commit,
and running `build.stable/ibt` to build your changes.
