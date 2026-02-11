You can initialize submodules at the root to download external dependencies:

```sh
# Download SDL3 sources
git submodule update --init external/SDL

# Download Google Test for unit testing
git submodule update --init external/googletest

# Or download both at once
git submodule update --init
```

Having SDL3 sources allows you to debug and step into its code to see how it
interacts with your windowing system. Google Test enables building and running
the project's unit tests.
