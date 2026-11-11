# Reproduce a wxGLCanvas Wayland scale error

This application reproduces a Wayland protocol error when a wxGTK OpenGL canvas
allocates its first back buffer before an output scale change, then presents that
buffer after the change. An odd buffer dimension is invalid at scale 2.

The original symptom was PrusaSlicer 2.9.6 failing to start at some tiled window
sizes under Hyprland. This reproducer uses a private headless Sway instance with
one virtual output. It does not require PrusaSlicer or physical monitors.

Related report: [wxWidgets #24662](https://github.com/wxWidgets/wxWidgets/issues/24662).
The error matches that report, but this reproducer does not establish that the
underlying cause is identical.

See [validation.txt](validation.txt) for the verified versions and outcomes.

## Build and run

Install a C++17 compiler, wxGTK development files with EGL-enabled wxGLCanvas,
OpenGL development files, Python 3.9 or later, pkg-config, GTK3, and Sway with
`swaymsg`. Sway must support its wlroots headless and GLES2 backends.

From this directory, run:

```sh
./build.sh
./run.py
```

`build.sh` uses the installed `wx-config`. It does not build the wxWidgets fork.
To test a separate wxWidgets build, set `WX_CONFIG` to that build's `wx-config`
for both commands. Ensure its shared libraries are also available at runtime.
The wxWidgets configuration must enable `wxUSE_GLCANVAS_EGL`.

The runner starts a compositor with a temporary runtime directory and socket.
It sets the canvas size through `MRE_WIDTH` and `MRE_HEIGHT`, and Sway leaves the
window floating at its requested size. The label above the canvas adds 30 logical
pixels to the window height. The runner changes only the virtual output scale.
It leaves your desktop compositor and configuration untouched.

Each run saves client protocol traces, the compositor log, version information,
and `summary.json` under `results/`. The runner stops its clients and compositor
when it finishes. The default is three repetitions of each case.

```sh
./run.py --repeat 1
./run.py --sway /path/to/sway --swaymsg /path/to/swaymsg
```

## Expected result

The runner reports `PASS` when it observes each case's expected outcome:

- An odd canvas height or width fails after scale changes from 1 to 2.
- An even width and height survive that change.
- Delaying `SetCurrent()` and the initial clear until scale 2 avoids the failure.
- Starting and staying at scale 1 or scale 2 avoids the failure.
- Changing scale from 1 to 1.25 also fails because GTK3 uses buffer scale 2.

Failing cases must exit with the matching Wayland protocol error. Controls must
present a frame and remain alive for the 1.5-second observation period. The
runner terminates surviving clients, so it does not test normal window shutdown.
It exits nonzero if any case differs from its expected outcome.

The failing trace contains:

```text
wl_surface...set_buffer_scale(2)
...create_immed(..., 1012, 743, ...)
wl_display...error(..., "Buffer size (1012x743) is not divisible by scale (2)")
```

## Run on an existing Wayland desktop

```sh
GDK_BACKEND=wayland WAYLAND_DEBUG=1 MRE_WIDTH=1012 MRE_HEIGHT=743 \
	./build/wx-wayland-scale-mre
```

The application clears the initial back buffer immediately after `Show()` and
swaps during paint events. Your compositor can override the requested window
size. Reproduction depends on the initial buffer dimensions and scale timing.

The headless runner sets `MRE_WAIT_SCALE=2` to hold the first swap until GTK
reports scale 2. It changes the output scale after the application's
`initialised` log line. This makes the transition deterministic without custom
Wayland requests, a modified library, or an artificial invalid buffer.
`MRE_LATE_CURRENT=1` disables the early clear for the allocation-timing control.

The application uses the same wxWindows licence as wxWidgets.
