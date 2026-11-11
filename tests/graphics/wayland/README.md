# Run the Wayland scale regression test

This hidden Catch test runs separately under headless Sway because a Wayland
protocol error terminates the process. It uses a virtual display, so your
monitor settings do not affect it.

Requires the wxGTK3 build dependencies, EGL and Wayland support, Python 3.9 or
later, pkg-config, and Sway with `swaymsg`. From the repository root:

```sh
git submodule update --init 3rdparty/catch 3rdparty/nanosvg
cmake -S . -B tests/graphics/wayland/build/wx -G Ninja \
    -DwxBUILD_TESTS=ALL -DwxUSE_GLCANVAS_EGL=ON \
    -DwxUSE_STC=OFF -DwxUSE_WEBVIEW=OFF
cmake --build tests/graphics/wayland/build/wx --target test_glcanvas
python3 tests/graphics/wayland/run.py \
    --test tests/graphics/wayland/build/wx/lib/test_glcanvas
```

The test is excluded from normal builds and test runs. CTest registration
requires CMake 3.18 or later:

```sh
cmake -S . -B tests/graphics/wayland/build/wx -DwxTEST_WAYLAND=ON
ctest --test-dir tests/graphics/wayland/build/wx \
    -R '^test_glcanvas_wayland$' --output-on-failure
```

The runner exits nonzero if any case fails. See `--help` for case selection,
repetitions, compositor paths, and log options.
