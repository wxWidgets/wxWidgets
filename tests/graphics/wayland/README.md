# Run the Wayland OpenGL scale regression test

`tests/graphics/glcanvas.cpp` contains a hidden Catch test using the standard
wxWidgets test application and `WaitFor()` helpers. Its separate `test_glcanvas`
executable isolates fatal Wayland protocol errors from `test_gui`.

The external runner starts headless Sway with a fixed 3840 x 2160 virtual output.
It controls the window dimensions and output scale without using your desktop's
monitor settings. Each case starts a fresh test process.

## Build from the checkout

Install the usual wxGTK3 build dependencies, EGL and OpenGL development files,
Python 3.9 or later, pkg-config, and Sway with `swaymsg`. The compositor needs its
wlroots headless and GLES2 backends and a working EGL renderer.

From the repository root, initialise Catch and the default SVG dependency:

```sh
git submodule update --init 3rdparty/catch 3rdparty/nanosvg
cmake -S . -B tests/graphics/wayland/build/wx -G Ninja \
	-DwxBUILD_TESTS=ALL -DwxUSE_GLCANVAS_EGL=ON \
	-DwxUSE_STC=OFF -DwxUSE_WEBVIEW=OFF
cmake --build tests/graphics/wayland/build/wx --target test_glcanvas
```

The target is available for wxGTK3 builds with EGL and Wayland support. By
default, it is excluded from the normal build and has no CTest registration.
It uses the checkout's wxWidgets libraries, not an installed `wx-config`.

## Run the regression cases

```sh
python3 tests/graphics/wayland/run.py \
	--test tests/graphics/wayland/build/wx/lib/test_glcanvas
```

Every case must render two frames, process the compositor's response, complete
the Catch assertions, and exit normally. A protocol error is a test failure.
The runner exits nonzero if any case fails. This differs from the original MRE
runner, which treated an expected crash as a successful reproduction.

The cases cover odd height, odd width, even dimensions, delayed OpenGL
initialisation, stable scales, and a change to fractional scale. The failing
sequence allocates a buffer at scale 1, then presents it after GTK changes to
scale 2. The runner waits for the `initialised` marker before changing scale.
The Catch test waits for the new scale before swapping buffers.

Use `--repeat 3` for repeated trials, `--case even-size` to select a case, or
`--sway /path/to/sway --swaymsg /path/to/swaymsg` for compositor binaries outside
`PATH`. Results go into a fresh directory under `tests/graphics/wayland/results`.
Each run records the protocol traces, Catch output, compositor log, linked
libraries, and a JSON summary. `--results /path` changes the parent directory.

The Catch case has the tags `[glcanvas][wayland][.]`, so ordinary Catch test
selection excludes it. The runner explicitly selects `[wayland]` and configures
the `WX_GL_TEST_*` environment variables consumed by the test. Running that case
directly requires the same controlled scale transition.

## Enable CTest registration

```sh
cmake -S . -B tests/graphics/wayland/build/wx -DwxTEST_WAYLAND=ON
cmake --build tests/graphics/wayland/build/wx --target test_glcanvas
ctest --test-dir tests/graphics/wayland/build/wx \
	-R '^test_glcanvas_wayland$' --output-on-failure
```

Enabling the option also includes the executable in the normal build. CMake
requires Python and Sway only when the option is enabled. To specify compositor
paths, set `wxTEST_SWAY_EXECUTABLE` and `wxTEST_SWAYMSG_EXECUTABLE` at configuration.
CTest logs go under the build directory's `wayland-results` directory.

See [validation.txt](validation.txt) for verified results. The original symptom
was PrusaSlicer failing at some tiled window sizes. The protocol error matches
[wxWidgets #24662](https://github.com/wxWidgets/wxWidgets/issues/24662), but that
does not establish that the underlying cause is identical.
